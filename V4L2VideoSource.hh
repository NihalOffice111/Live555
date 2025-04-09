#ifndef _V4L2_VIDEO_SOURCE_HH
#define _V4L2_VIDEO_SOURCE_HH

#include "FramedSource.hh"
#include <linux/videodev2.h>
#include <turbojpeg.h>

class V4L2VideoSource: public FramedSource {
public:
    static V4L2VideoSource* createNew(UsageEnvironment& env);

protected:
    V4L2VideoSource(UsageEnvironment& env);
    virtual ~V4L2VideoSource();

private:
    virtual void doGetNextFrame();
    void captureFrame();

    int fd;                    // V4L2 file descriptor
    struct v4l2_buffer buf;    // V4L2 buffer
    unsigned char* rawBuffer;  // Raw frame buffer
    unsigned char* jpegBuffer; // Compressed JPEG buffer
    unsigned char* yuvBuffer;
    unsigned int jpegSize;
    tjhandle tjInstance;       // TurboJPEG handle
    unsigned int bufLength;    // Length of the mapped buffer (added)

    FILE* mjpegFile; // File pointer for MJPEG file (added)
};

#endif
