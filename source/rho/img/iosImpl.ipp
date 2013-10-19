#include <rho/img/ebImg.h>
#include <rho/img/iImageCap.h>
#include <rho/img/iImageCapParamsEnumerator.h>
#include <rho/img/nImageFormat.h>
#include <rho/img/tImageCapParams.h>
#include <rho/sync/tMutex.h>
#include <rho/sync/tPCQ.h>

#include <vector>

#include <AVFoundation/AVFoundation.h>
#include <CoreMedia/CoreMedia.h>
#include <CoreVideo/CoreVideo.h>
#include <Foundation/Foundation.h>


namespace rho
{
namespace img
{

    class tImageCap;

}    // namespace img
}    // namespace rho


@interface MyVideoCaptureDelegate : NSObject
                        <AVCaptureVideoDataOutputSampleBufferDelegate>
{
    rho::img::tImageCap* m_imageCap;
}

- (id) initWithImageCap:(rho::img::tImageCap*)imageCap;

@end


namespace rho
{
namespace img
{


class tImageCapParamsEnumerator : public iImageCapParamsEnumerator
{
    public:

        tImageCapParamsEnumerator()
        {
            NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

            NSArray* devices =
                [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];

            for (unsigned int i = 0; i < devices.count; i++)
            {
                AVCaptureDevice* device = [devices objectAtIndex:i];

                tImageCapParams params;
                params.deviceIndex = i;
                params.inputIndex = 0;
                params.inputDescription = [device.localizedName UTF8String];
                params.displayFormat = kBGRA;
                params.displayFormatDescription = "BGRA";
                params.captureFormat = kBGRA;
                params.captureFormatDescription = "BGRA";
                params.frameIntervalNumerator = 1;
                params.frameIntervalDenominator = 30;

                if ([device supportsAVCaptureSessionPreset:AVCaptureSessionPreset352x288])
                {
                    params.imageWidth = 352;
                    params.imageHeight = 288;
                    m_params.push_back(params);
                }

                if ([device supportsAVCaptureSessionPreset:AVCaptureSessionPreset640x480])
                {
                    params.imageWidth = 640;
                    params.imageHeight = 480;
                    m_params.push_back(params);
                }

                if ([device supportsAVCaptureSessionPreset:AVCaptureSessionPreset1280x720])
                {
                    params.imageWidth = 1280;
                    params.imageHeight = 720;
                    m_params.push_back(params);
                }

                if ([device supportsAVCaptureSessionPreset:AVCaptureSessionPreset1920x1080])
                {
                    params.imageWidth = 1920;
                    params.imageHeight = 1080;
                    m_params.push_back(params);
                }
            }

            [pool release];
        }

        int size() const
        {
            return m_params.size();
        }

        const tImageCapParams& operator[] (int i)
        {
            return m_params[i];
        }

    private:

        std::vector<tImageCapParams> m_params;
};


class tAutoObjcRelease
{
    public:

        tAutoObjcRelease(void* obj) { m_obj = obj; }
        ~tAutoObjcRelease() { [(id)m_obj release]; }

    private:

        void* m_obj;
};


class tImageCap : public iImageCap
{
    public:

        tImageCap(const tImageCapParams& params)
            : m_session(nil), m_delegate(nil),
              m_inQueue(4, sync::kBlock),
              m_outQueue(4, sync::kBlock),
              destructed(false)
        {
            m_params = params;

            if (params.captureFormat != kBGRA)
                throw eInvalidDeviceAttributes(
                        "You must capture in BGRA format.");

            NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
            tAutoObjcRelease ar0(pool);

            NSArray* devices =
                [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];

            if (params.deviceIndex >= (u32)devices.count)
                throw eInvalidDeviceAttributes("Bad device index.");

            AVCaptureDevice* device =
                [devices objectAtIndex:params.deviceIndex];

            NSString* sessionPreset = nil;

            if (params.imageWidth == 352 && params.imageHeight == 288)
                sessionPreset = AVCaptureSessionPreset352x288;
            else if (params.imageWidth == 640 && params.imageHeight == 480)
                sessionPreset = AVCaptureSessionPreset640x480;
            else if (params.imageWidth == 1280 && params.imageHeight == 720)
                sessionPreset = AVCaptureSessionPreset1280x720;
            else if (params.imageWidth == 1920 && params.imageHeight == 1080)
                sessionPreset = AVCaptureSessionPreset1920x1080;
            else
                throw eInvalidDeviceAttributes(
                        "Unsupported image size.");

            if (! [device supportsAVCaptureSessionPreset:sessionPreset])
                throw eInvalidDeviceAttributes(
                        "Cannot set specified image dimensions.");

            m_session = [[AVCaptureSession alloc] init];
            tAutoObjcRelease ar1(m_session);

            [m_session beginConfiguration];

            if ([m_session canSetSessionPreset:sessionPreset])
                m_session.sessionPreset = sessionPreset;
            else
                throw eInvalidDeviceAttributes(
                        "Cannot set session's preset.");

            AVCaptureDeviceInput* input =
               [AVCaptureDeviceInput deviceInputWithDevice:device error:NULL];

            if (! input)
                throw eInvalidDeviceAttributes(
                        "Cannot create input object.");

            if ([m_session canAddInput:input])
                [m_session addInput:input];
            else
                throw eInvalidDeviceAttributes(
                        "Cannot add input to session.");

            AVCaptureVideoDataOutput* output =
                [[AVCaptureVideoDataOutput alloc] init];
            tAutoObjcRelease ar2(output);

            NSArray* availableFormats =
                [output availableVideoCVPixelFormatTypes];

            NSNumber* desiredFormat =
                [NSNumber numberWithInt:kCVPixelFormatType_32BGRA];

            if (! [availableFormats containsObject:desiredFormat])
                throw eInvalidDeviceAttributes(
                        "The device doesn't support BGRA format.");

            NSNumber* desiredWidth =
                [NSNumber numberWithInt:params.imageWidth];

            NSNumber* desiredHeight =
                [NSNumber numberWithInt:params.imageHeight];

            NSDictionary* videoSettings =
                [NSDictionary dictionaryWithObjectsAndKeys:
                    desiredFormat, (NSString*)kCVPixelBufferPixelFormatTypeKey,
                    desiredWidth, (NSString*)kCVPixelBufferWidthKey,
                    desiredHeight, (NSString*)kCVPixelBufferHeightKey,
                    nil
                ];

            output.videoSettings = videoSettings;
            output.alwaysDiscardsLateVideoFrames = YES;

            m_delegate =
                [[MyVideoCaptureDelegate alloc] initWithImageCap:this];
            tAutoObjcRelease ar3(m_delegate);

            dispatch_queue_t queue =
                dispatch_queue_create("com.rhobota.rho.img.imagecap", NULL);

            [output setSampleBufferDelegate:m_delegate queue:queue];

            dispatch_release(queue);

            if ([m_session canAddOutput:output])
                [m_session addOutput:output];
            else
                throw eInvalidDeviceAttributes(
                        "Cannot add output.");

            [m_session commitConfiguration];

            int bufSize = getRequiredBufSize();
            while (m_outQueue.size() < m_outQueue.capacity())
                m_outQueue.push(new u8[bufSize]);

            [m_session startRunning];

            [m_session retain];
            [m_delegate retain];
        }

        ~tImageCap()
        {
            [m_session stopRunning];
            for (AVCaptureOutput* output in m_session.outputs)
                [m_session removeOutput:output];
            [m_session release];
            m_session = nil;

            [m_delegate release];
            m_delegate = nil;

            {
                destructMutex.acquire();

                while (m_inQueue.size())
                {
                    u8* buf = m_inQueue.pop();
                    delete [] buf;
                }

                while (m_outQueue.size())
                {
                    u8* buf = m_outQueue.pop();
                    delete [] buf;
                }

                destructed = true;

                destructMutex.release();
            }
        }

        const tImageCapParams& getParams() const
        {
            return m_params;
        }

        u32 getRequiredBufSize() const
        {
            return m_params.imageWidth * m_params.imageHeight * 4;
        }

        void getFrame(tImage* image)
        {
            u8* b = m_inQueue.pop();
            if (image->bufSize() < getRequiredBufSize())
                image->setBufSize(getRequiredBufSize());
            u32 bufUsed = colorspace_conversion(
                        m_params.captureFormat, m_params.displayFormat,
                        b, getRequiredBufSize(),
                        image->buf(), image->bufSize());
            image->setBufUsed( bufUsed );
            image->setWidth( m_params.imageWidth );
            image->setHeight( m_params.imageHeight );
            image->setFormat( m_params.displayFormat );
            m_outQueue.push(b);
        }

        void addFrame(u8* buf, int bufSize)
        {
            destructMutex.acquire();
            if (!destructed)
            {
                u8* b = m_outQueue.pop();
                for (int i = 0; i < bufSize; i++)
                    b[i] = buf[i];
                m_inQueue.push(b);
            }
            destructMutex.release();
        }

    private:

        AVCaptureSession* m_session;
        MyVideoCaptureDelegate* m_delegate;

    private:

        tImageCapParams m_params;

        sync::tPCQ<u8*> m_inQueue;
        sync::tPCQ<u8*> m_outQueue;

        sync::tMutex destructMutex;
        bool         destructed;
};


}    // namespace img
}    // namespace rho


@implementation MyVideoCaptureDelegate

- (id) initWithImageCap:(rho::img::tImageCap*)imageCap
{
    self = [self init];
    if (self)
    {
        self->m_imageCap = imageCap;
    }
    return self;
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);
    size_t width = CVPixelBufferGetWidth(imageBuffer);
    size_t height = CVPixelBufferGetHeight(imageBuffer);
    void *baseAddress = CVPixelBufferGetBaseAddress(imageBuffer);
    size_t bufferSize = CVPixelBufferGetDataSize(imageBuffer);
    if (bufferSize < bytesPerRow * height)
        throw rho::eLogicError("The given buffer is too small!");
    if ((size_t)self->m_imageCap->getRequiredBufSize() != bytesPerRow * height)
        throw rho::eLogicError(
                "The captured frame's dimensions must be wrong...");
    if (bytesPerRow / width != 4)
        throw rho::eLogicError("The format must be wrong...");
    self->m_imageCap->addFrame((rho::u8*)baseAddress, bytesPerRow * height);
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput didDropSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
}

@end
