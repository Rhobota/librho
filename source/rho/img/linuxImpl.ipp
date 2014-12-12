#include <rho/img/ebImg.h>
#include <rho/img/iImageCap.h>
#include <rho/img/iImageCapParamsEnumerator.h>
#include <rho/img/nImageFormat.h>
#include <rho/img/tImageCapParams.h>

#include <errno.h>            // for errno
#include <fcntl.h>            // for open()
#include <linux/videodev2.h>  // for all the v4l2 stuff
#include <string.h>           // for memset()
#include <sys/ioctl.h>        // for ioctl()
#include <sys/mman.h>         // for mmap()
#include <sys/select.h>       // for select()
#include <unistd.h>           // for close()

#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>


namespace rho
{
namespace img
{


static
nImageFormat  v4l2_pixelformat_to_nImageFormat(int v4l2_pixelformat)
{
    switch (v4l2_pixelformat)
    {
        case V4L2_PIX_FMT_YUYV:
            return kYUYV;
        case V4L2_PIX_FMT_RGB24:
            return kRGB24;
        default:
            return kUnknown;
    }
}


static
int  nImageFormat_to_v4l2_pixelformat(nImageFormat fmt)
{
    switch (fmt)
    {
        case kYUYV:
            return V4L2_PIX_FMT_YUYV;
        case kRGB24:
            return V4L2_PIX_FMT_RGB24;
        default:
            return -1;
    }
}


static
int xioctl(int fd, int request, void* data)
{
    int ret;
    do
        ret = ::ioctl(fd, request, data);
    while (ret == -1 &&
            (errno == EINTR || errno == EAGAIN || errno == ETIMEDOUT));
    return ret;
}


static
void verrifyDeviceCapabilities(int fd)
{
    struct v4l2_capability caps;
    memset(&caps, 0, sizeof(caps));

    if (xioctl(fd, (int)VIDIOC_QUERYCAP, &caps) == -1)
    {
        throw eInvalidDeviceCapabilities("Querying device capabilities failed. :(");
    }

    if ((caps.capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0)
    {
        throw eInvalidDeviceCapabilities("Device is not capable of video capture. :(");
    }

    if ((caps.capabilities & V4L2_CAP_STREAMING) == 0)
    {
        throw eInvalidDeviceCapabilities("Device is not capable of streaming. :(");
    }

    if ((caps.capabilities & V4L2_CAP_READWRITE) == 0)
    {
        // Don't care...
        //throw eInvalidDeviceCapabilities("Device is not capable of read/write I/O. :(");
    }
}


static
int openDevice(std::string devicePath)
{
    int fd = ::open(devicePath.c_str(), O_RDWR | O_NONBLOCK, 0); // try: remove O_NONBLOCK
    if (fd < 0)
    {
        std::ostringstream o;
        o << "Cannot open device: " << devicePath;
        throw eDeviceOpenError(o.str());
    }
    return fd;
}


static
void closeDevice(int fd)
{
    if (fd >= 0)
        ::close(fd);
}


class tAutoDeviceCloser
{
    public:

        tAutoDeviceCloser(int fd) : m_fd(fd) { }
        ~tAutoDeviceCloser() { closeDevice(m_fd); }

    private:

        int m_fd;
};


typedef std::vector<tImageCapParams> tParamsVector;


static
void setInput(int fd, int inputIndex)
{
    if (xioctl(fd, (int)VIDIOC_S_INPUT, &inputIndex) == -1)
    {
        throw eInvalidDeviceAttributes("Device input index is invalid.");
    }
}


static
void enumFrameintervals(int fd, unsigned int pixelformat, unsigned int width, unsigned int height,
                        tParamsVector& params, tImageCapParams currParams)
{
    struct v4l2_frmivalenum posinterval;
    memset(&posinterval, 0, sizeof(posinterval));
    posinterval.index = 0;
    posinterval.pixel_format = pixelformat;
    posinterval.width = width;
    posinterval.height = height;
    posinterval.type = V4L2_FRMIVAL_TYPE_DISCRETE;

    for (; xioctl(fd, (int)VIDIOC_ENUM_FRAMEINTERVALS, &posinterval) == 0;
            ++posinterval.index)
    {
        if (posinterval.type == V4L2_FRMIVAL_TYPE_DISCRETE)
        {
            currParams.frameIntervalNumerator   = posinterval.discrete.numerator;
            currParams.frameIntervalDenominator = posinterval.discrete.denominator;
            params.push_back(currParams);
        }
    }
}


static
void enumFramesizes(int fd, unsigned int pixelformat,
                    tParamsVector& params, tImageCapParams currParams)
{
    struct v4l2_frmsizeenum possize;
    memset(&possize, 0, sizeof(possize));
    possize.index = 0;
    possize.pixel_format = pixelformat;
    possize.type = V4L2_FRMSIZE_TYPE_DISCRETE;

    for (; xioctl(fd, (int)VIDIOC_ENUM_FRAMESIZES, &possize) == 0;
            ++possize.index)
    {
        if (possize.type == V4L2_FRMSIZE_TYPE_DISCRETE)
        {
            currParams.imageWidth  = possize.discrete.width;
            currParams.imageHeight = possize.discrete.height;
            enumFrameintervals(fd, pixelformat,
                    possize.discrete.width, possize.discrete.height,
                    params, currParams);
        }
    }
}


static
void enumFormats(int fd, tParamsVector& params, tImageCapParams currParams)
{
    struct v4l2_fmtdesc posfmt;
    memset(&posfmt, 0, sizeof(posfmt));
    posfmt.index = 0;
    posfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    posfmt.flags |= V4L2_FMT_FLAG_COMPRESSED;
    posfmt.flags |= V4L2_FMT_FLAG_EMULATED;

    for (; xioctl(fd, (int)VIDIOC_ENUM_FMT, &posfmt) == 0; ++posfmt.index)
    {
        nImageFormat format = v4l2_pixelformat_to_nImageFormat(posfmt.pixelformat);
        if (format != kUnknown && format != kUnspecified)
        {
            currParams.captureFormat = format;
            currParams.captureFormatDescription = (char*) posfmt.description;
            enumFramesizes(fd, posfmt.pixelformat, params, currParams);
        }
    }
}


static
void enumInputs(int fd, tParamsVector& params, tImageCapParams currParams)
{
    struct v4l2_input posinput;
    memset(&posinput, 0, sizeof(posinput));
    posinput.index = 0;

    for (; xioctl(fd, (int)VIDIOC_ENUMINPUT, &posinput) == 0; ++posinput.index)
    {
        currParams.inputIndex       = posinput.index;
        currParams.inputDescription = (char*) posinput.name;
        setInput(fd, posinput.index);
        enumFormats(fd, params, currParams);
    }
}


static
void enumDevices(tParamsVector& params)
{
    for (int i = 0; i < 16; i++)
    {
        std::ostringstream o;
        o << "/dev/video" << i;
        try
        {
            int fd = openDevice(o.str());
            tAutoDeviceCloser autoCloser(fd);
            verrifyDeviceCapabilities(fd);
            tImageCapParams currParams;
            currParams.deviceIndex = i;
            enumInputs(fd, params, currParams);
        }
        catch (ebImg& e)
        {
        }
    }
}


static
void setFormat(int fd, unsigned int pixelformat, unsigned int width, unsigned int height)
{
    struct v4l2_pix_format pixinf;
    memset(&pixinf, 0, sizeof(pixinf));
    pixinf.pixelformat = pixelformat;
    pixinf.width = width;
    pixinf.height = height;
    pixinf.colorspace = V4L2_COLORSPACE_SRGB; // try: delete this?
    pixinf.field = V4L2_FIELD_ANY;

    struct v4l2_format format;
    memset(&format, 0, sizeof(format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix = pixinf;

    if (xioctl(fd, (int)VIDIOC_S_FMT, &format) == -1)
    {
        throw eInvalidDeviceAttributes("System call to set the video format failed.");
    }

    if (format.type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
    {
        throw eInvalidDeviceAttributes("Bad driver! It changed the format type!");
    }

    pixinf = format.fmt.pix;

    if (pixinf.pixelformat != pixelformat)
    {
        std::ostringstream o;
        o << "Pixelformat was coerced to: " << pixinf.pixelformat;
        throw eInvalidDeviceAttributes(o.str());
    }

    if (pixinf.width != width)
    {
        std::ostringstream o;
        o << "Width was coerced to: " << pixinf.width;
        throw eInvalidDeviceAttributes(o.str());
    }

    if (pixinf.height != height)
    {
        std::ostringstream o;
        o << "Height was coerced to: " << pixinf.height;
        throw eInvalidDeviceAttributes(o.str());
    }

    // try: check field == interlaced?
}


static
void printCurrentFormat(int fd)                 // use for debugging!!!
{
    struct v4l2_format format;
    memset(&format, 0, sizeof(format));

    if (xioctl(fd, (int)VIDIOC_G_FMT, &format) == -1)
    {
        std::cout << "Reading current pixel format is not supported by this device." << std::endl;
        std::cout << std::endl;
        return;
    }

    if (format.type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
    {
        std::cerr << "The device is not set to caputr video, oh no... :(" << std::endl;
    }

    struct v4l2_pix_format pix = format.fmt.pix;
    std::cout << "Capture width: " << pix.width << std::endl;
    std::cout << "Capture height: " << pix.height << std::endl;
    std::cout << "Capture bytesperline: " << pix.bytesperline << std::endl;
    std::cout << "Capture sizeimage: " << pix.sizeimage << std::endl;
    std::cout << std::endl;
}


static
void setStreamParameters(int fd, unsigned int timeperframeNum, unsigned int timeperframeDenom)
{
    // try: set starndard?

    struct v4l2_captureparm capparm;
    memset(&capparm, 0, sizeof(capparm));
    capparm.capturemode = 0;                        // could be V4L2_MODE_HIGHQUALITY ?
    capparm.capability = V4L2_CAP_TIMEPERFRAME;
    capparm.timeperframe.numerator = timeperframeNum;
    capparm.timeperframe.denominator = timeperframeDenom;

    struct v4l2_streamparm streamparm;
    memset(&streamparm, 0, sizeof(streamparm));
    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    streamparm.parm.capture = capparm;

    if (xioctl(fd, (int)VIDIOC_S_PARM, &streamparm) == -1)
    {
        throw eInvalidDeviceAttributes("Cannot set stream parameters. :(");
    }

    if (streamparm.type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
    {
        throw eInvalidDeviceAttributes("Bad driver! It changed the streamparam type.");
    }

    capparm = streamparm.parm.capture;

    if ((capparm.capability & V4L2_CAP_TIMEPERFRAME) == 0)
    {
        throw eInvalidDeviceAttributes("Driver doesn't support timeperframe settings. :(");
    }

    if (capparm.timeperframe.numerator != timeperframeNum ||
        capparm.timeperframe.denominator != timeperframeDenom)
    {
        std::ostringstream o;
        o << "Timeperframe was coerced to: "
          << capparm.timeperframe.numerator << "/"
          << capparm.timeperframe.denominator;
        throw eInvalidDeviceAttributes(o.str());
    }
}


static
void printCurrentStreamParameters(int fd)         // use for debugging!!!
{
    struct v4l2_streamparm streamparm;
    memset(&streamparm, 0, sizeof(streamparm));
    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (xioctl(fd, (int)VIDIOC_G_PARM, &streamparm) == -1)
    {
        std::cout << "Reading the current stream parameters is not supported by this device."
                  << std::endl;
        std::cout << std::endl;
        return;
    }

    if (streamparm.type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
    {
        std::cerr << "The stream parameter type is incorrect! :(" << std::endl;
    }

    struct v4l2_captureparm capparm = streamparm.parm.capture;
    if (capparm.capability & V4L2_CAP_TIMEPERFRAME)
        std::cout << "Time-per-frame = "
             << capparm.timeperframe.numerator << "/"
             << capparm.timeperframe.denominator
             << " seconds." << std::endl;
    else
        std::cout << "Time-per-frame not supported on this device. :(" << std::endl;
    if (capparm.capturemode & V4L2_MODE_HIGHQUALITY)
        std::cout << "High-quality mode ON." << std::endl;
    else
        std::cout << "High-quality mode OFF." << std::endl;
    std::cout << std::endl;
}


static
void enableStreaming(int fd)
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_STREAMON, &type) == -1)
    {
        throw eInvalidDeviceAttributes("Cannot turn on video stream. :(");
    }
}


static
void disableStreaming(int fd)
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(fd, VIDIOC_STREAMOFF, &type) == -1)
    {
        throw eInvalidDeviceAttributes("Cannot turn off video stream. :(");
    }
}


static
void obtainBuffers(int fd, unsigned int numBuffers, u8* buffers[], int bufSizes[])
{
    struct v4l2_buffer bufs[512];

    if (numBuffers >= 512)
    {
        throw eBufferOverflow("Cannot allocate >=512 v4l2 buffers.");
    }

    struct v4l2_requestbuffers rbufs;
    memset(&rbufs, 0, sizeof(rbufs));
    rbufs.count = numBuffers;
    rbufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    rbufs.memory = V4L2_MEMORY_MMAP;

    if (xioctl(fd, (int)VIDIOC_REQBUFS, &rbufs) == -1)
    {
        throw eInvalidDeviceAttributes("Requesting buffers failed. :(");
    }

    if (rbufs.count != numBuffers)
    {
        throw eInvalidDeviceAttributes("Didn't get as many buffers as we wanted. :(");
    }

    for (unsigned int i = 0; i < rbufs.count; i++)
    {
        struct v4l2_buffer& buf = bufs[i];
        memset(&buf, 0, sizeof(buf));
        buf.index = i;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (xioctl(fd, (int)VIDIOC_QUERYBUF, &buf) == -1)
        {
            throw eInvalidDeviceAttributes("Cannot request buffer info... :(");
        }

        if (buf.index != i)
        {
            throw eInvalidDeviceAttributes("Bad driver! It changed the buffer index we asked for.");
        }

        buffers[i] = (u8*) mmap(NULL, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED,
                                fd, buf.m.offset);
        bufSizes[i] = buf.length;

        if (buffers[i] == MAP_FAILED)
        {
            throw eInvalidDeviceAttributes("Bad driver! Couldn't map buffer.");
        }
    }

    for (unsigned int i = 0; i < rbufs.count; i++)
    {
        struct v4l2_buffer& buf = bufs[i];
        memset(&buf, 0, sizeof(buf));
        buf.index = i;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (xioctl(fd, (int)VIDIOC_QBUF, &buf) == -1)
        {
            throw eInvalidDeviceAttributes("Cannot enqueue a buffer update. :(");
        }
    }
}


static
void relinquishBuffers(int numBuffers, u8* buffers[], int bufSizes[])
{
    for (int i = 0; i < numBuffers; i++)
    {
        if (buffers[i])
        {
            munmap(buffers[i], bufSizes[i]);
            buffers[i] = NULL;
            bufSizes[i] = 0;
        }
    }
}


static
int readFrame(int fd, int numBuffers, u8* buffers[], u8* outData, int outSize)
{
    bool ready = false;
    while (!ready)
    {
        fd_set fset;
        FD_ZERO(&fset);
        FD_SET(fd, &fset);
        ready = (select(fd+1, &fset, NULL, NULL, NULL) > 0) && (FD_ISSET(fd, &fset));
    }

    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (xioctl(fd, (int)VIDIOC_DQBUF, &buf) == -1)
        throw eInvalidDeviceAttributes("Buffer dequeue failed. :(");

    int bufIndex  = buf.index;
    int bufLength = buf.bytesused;
    //int frameSeq  = buf.sequence;

    if (bufIndex < 0 || bufIndex >= numBuffers)
        throw eInvalidDeviceAttributes("Bad driver! It gave an invalid buffer index.");

    u8* bufPtr = buffers[bufIndex];

    if (outSize < bufLength)
        throw eBufferOverflow("The given buffer is not long enough to hold the captured frame.");

    for (int i = 0; i < bufLength; i++)
        outData[i] = bufPtr[i];

    if (xioctl(fd, (int)VIDIOC_QBUF, &buf) == -1)
        throw eInvalidDeviceAttributes("Cannot re-queue buffer. :(");

    return bufLength;
}


class tImageCapParamsEnumerator : public iImageCapParamsEnumerator
{
    public:

        tImageCapParamsEnumerator();

        int size() const;

        const tImageCapParams& operator[] (int i);

    private:

        std::vector<tImageCapParams> m_params;
};


tImageCapParamsEnumerator::tImageCapParamsEnumerator()
{
    enumDevices(m_params);
}


int tImageCapParamsEnumerator::size() const
{
    return (int) m_params.size();
}


const tImageCapParams& tImageCapParamsEnumerator::operator[] (int i)
{
    return m_params[i];
}


class tImageCap : public iImageCap
{
    public:

        tImageCap(const tImageCapParams& params);
        ~tImageCap();

        const tImageCapParams&  getParams() const;
        u32                     getRequiredBufSize() const;
        void                    getFrame(tImage* image);

    private:

        void m_finalize();

        void m_debugPrint();

    private:

        tImageCapParams m_params;
        int             m_fd;

        static const int kNumBuffers = 8;  // try: more buffers? 256

        u8* m_buffers[kNumBuffers];
        int m_bufSizes[kNumBuffers];

        u8* m_tempBuffer;
        u32 m_tempBufferSize;
};


tImageCap::tImageCap(const tImageCapParams& params)
    : m_params(params),
      m_fd(-1),
      m_tempBuffer(NULL),
      m_tempBufferSize(0)
{
    for (int i = 0; i < kNumBuffers; i++)
    {
        m_buffers[i] = NULL;
        m_bufSizes[i] = 0;
    }

    if (params.captureFormat == kUnknown     || params.displayFormat == kUnknown  ||
        params.captureFormat == kUnspecified || params.displayFormat == kUnspecified)
    {
        throw eInvalidDeviceAttributes("You must specify the capture and display formats.");
    }

    try
    {
        std::ostringstream o;
        o << "/dev/video" << params.deviceIndex;
        m_fd = openDevice(o.str());
        verrifyDeviceCapabilities(m_fd);
        setFormat(m_fd, nImageFormat_to_v4l2_pixelformat(params.captureFormat),
                  params.imageWidth, params.imageHeight);
        setInput(m_fd, params.inputIndex);
        setStreamParameters(m_fd, params.frameIntervalNumerator, params.frameIntervalDenominator);
        obtainBuffers(m_fd, kNumBuffers, m_buffers, m_bufSizes);
        enableStreaming(m_fd);
    }
    catch (ebObject& e)
    {
        m_finalize();
        throw;
    }

    m_tempBufferSize = getRequiredBufSize();
    m_tempBuffer = new u8[m_tempBufferSize];
}


tImageCap::~tImageCap()
{
    m_finalize();
}


void tImageCap::m_finalize()
{
    try
    {
        disableStreaming(m_fd);
    }
    catch (...) { }

    relinquishBuffers(kNumBuffers, m_buffers, m_bufSizes);
    closeDevice(m_fd);
    m_fd = -1;

    delete [] m_tempBuffer;
    m_tempBuffer = NULL;
    m_tempBufferSize = 0;
}


void tImageCap::m_debugPrint()
{
    printCurrentFormat(m_fd);
    printCurrentStreamParameters(m_fd);
}


const tImageCapParams& tImageCap::getParams() const
{
    return m_params;
}


u32 tImageCap::getRequiredBufSize() const
{
    // The following returns a worst-case size. We could
    // optimize this later if needed.
    return m_params.imageWidth * m_params.imageHeight * 4;
}


void tImageCap::getFrame(tImage* image)
{
    image->setWidth( m_params.imageWidth );
    image->setHeight( m_params.imageHeight );
    image->setFormat( m_params.displayFormat );

    if (image->bufSize() < getRequiredBufSize())
        image->setBufSize(getRequiredBufSize());

    int bufUsed = readFrame(m_fd, kNumBuffers, m_buffers,
                            image->buf(), image->bufSize());
    image->setBufUsed( bufUsed );

    if (m_params.captureFormat == m_params.displayFormat)
        return;

    bufUsed = colorspace_conversion(
                m_params.captureFormat, m_params.displayFormat,
                image->buf(), image->bufUsed(),
                m_tempBuffer, std::min(image->bufSize(), m_tempBufferSize));
    image->setBufUsed( bufUsed );

    memcpy(image->buf(), m_tempBuffer, bufUsed);
}


}   // namespace img
}   // namespace rho
