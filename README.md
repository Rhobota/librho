LIBRHO
======

This library compiles under:
 - Linux
 - Cygwin
 - OSX
 - iOS
 - Mingw (on Windows)

Following are instructions for compiling on each platform.


Linux, Cygwin, OSX
------------------

Compiling and testing under Linux, Cygwin, and OSX is easy.  Do the following:

To make the library, run:

          make

That will put a bunch of object files and a static library file in 'objects/'.

You can now run the tests with the command:

          make test

If you'd like to install the header files and the static lib permanently, run:

          sudo make install

That will put stuff in '/usr/local/include' and in '/usr/local/lib'. You can
now compile your programs by passing '-lrho' to `g++`. Try it out with the
examples in 'examples/'.

To clean-up, run:

          make clean


iOS
---

Compiling for iOS takes a little love, but not too much. Do the following:

1. Copy the 'source/' and 'include/' directories into your iOS Xcode project.
2. Rename tImageCapFactory.cpp to tImageCapFactory.mm
3. Make sure tImageCapFactory.mm is set to compile without ARC. To do this,
   add a flag in the project settings to tImageCapFactory.mm that disables
   ARC for just that file. That flag is '-fno-objc-arc'.
4. Add the following frameworks to the project (if they are not already
   present):
      - AVFoundation.framework
      - CoreVideo.framework
      - CoreMedia.framework


Mingw
-----

Compiling for Mingw is easy if you do so using the Cygwin environment. Simply
install Cygwin (make sure the Mingw package is installed along with it), then
within the Cygwin environment, set the $TARGET environment variable equal to
the mingw-prefix (which was 'i686-pc-mingw32-' last time I checked). You can
set the $TARGET environment variable and run make on one line as follows:

    TARGET=i686-pc-mingw32- make

You can then proceed as the Linux instructions dictate, setting the $TARGET
variable for each command as is done above.


Other compilation notes
-----------------------

1. To compile the library for x86 (as opposed to for x86_64), you must pass '-m32'
to `g++`. You can easily do this by running `CC_FLAGS=-m32 make`. That sets
$CC_FLAGS equal to '-m32' just for the duration of the `make` command.

2. If you want to run a version of `g++` that contains a prefix (for example,
for doing some sort of cross-compilation), you can set the $TARGET variable
to the desired g++-prefix before running `make`. Do this in the same way as
described in the Mingw section.


Usage notes
-----------

1. This library contains static data which gets initialized in some arbitrary
order by the compiler. Within the library there are no dependencies between
any of the static data, so it doesn't matter what order initialization happens.
Users of the library should be careful about what they allocate in static space,
however, because if that application-specific static data gets initialized
before some required part of librho, then bad things could happen. For example,
librho does some socket initialization stuff (like setting signals to be
ignored) as a part of its static initialization; thus, if a user's application
creates a socket in the static area, it may happen before librho gets a chance
to do its own socket initialization stuff. The best thing to do is avoid using
static allocations, and also read the next note.

2. Users of librho must understand the refc templated class. It is awesome.
Also, it is okay to create a NULL refc in static space (since a NULL refc does
not depend on any static initialization within librho). Doing so is useful if
you *really* need a global object. Simply set the NULL global refc equal to a
real object as a part of main(), then use you global object happily.

3. Be careful if your application does a fork()+exec(). Librho sets most of
the file descriptors that it opens to close-on-exec, but not all of them.
It cannot set close-on-exec when a file is opened via the C++ fstream library.
Currently only geo::tMesh and audio::tWaveMaker use the fstream library, so
chances are that you are okay. Also, currently, reading an image from file
using the img::tImage constructor opens a file without setting close-on-
exec. If you do any of these in an application that does a fork()+exec(),
you could end up leaking file descriptors. Using tFileReadable, tFileWritable,
ip::tcp::tSocket, and ip::tcp::tServer are all safe, however.


Dependencies
------------

This library is very self-contained. Many of the "dependencies" are built into the library and get compiled _with_ the library. The only dependencies which must be built separately and installed on the system are:

1. libvpx from the [WebM project](http://www.webmproject.org/code/)


Other
-----

Here are a few things I'd like the library to have soon:

    - non-blocking tcp & udp sockets
    - graph library

Things left to do are marked with a comment that contains 'libtodo' or 'LIBTODO'.


Contributing
------------

This library is built with portability in mind (e.g. code that is agnostic to platform and CPU architecture). We welcome pull requests, but we only accept changelists that contain portable C(++) code. :)
