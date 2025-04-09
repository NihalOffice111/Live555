// #include "V4L2VideoSource.hh"
// #include <fcntl.h>
// #include <sys/ioctl.h>
// #include <unistd.h>
// #include <string.h>
// #include <sys/mman.h>
// #include <turbojpeg.h>

// V4L2VideoSource* V4L2VideoSource::createNew(UsageEnvironment& env) {
//     return new V4L2VideoSource(env);
// }

// V4L2VideoSource::V4L2VideoSource(UsageEnvironment& env)
//     : FramedSource(env), fd(-1), rawBuffer(NULL), jpegBuffer(NULL), jpegSize(0), tjInstance(tjInitCompress()) {
//     // Open V4L2 device
//     fd = open("/dev/video0", O_RDWR);
//     if (fd < 0) {
//         envir() << "Failed to open video device\n";
//         return;
//     }

//     // Configure V4L2 (simplified)
//     struct v4l2_format fmt = {0};
//     fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//     fmt.fmt.pix.width = 640;
//     fmt.fmt.pix.height = 480;
//     fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
//     ioctl(fd, VIDIOC_S_FMT, &fmt);

//     struct v4l2_requestbuffers req = {0};
//     req.count = 1;
//     req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//     req.memory = V4L2_MEMORY_MMAP;
//     ioctl(fd, VIDIOC_REQBUFS, &req);

//     struct v4l2_buffer buf = {0};
//     buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//     buf.memory = V4L2_MEMORY_MMAP;
//     buf.index = 0;
//     ioctl(fd, VIDIOC_QUERYBUF, &buf);
//     rawBuffer = (unsigned char*)mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
//     jpegBuffer = new unsigned char[buf.length]; // Allocate enough space

//     // Start streaming
//     int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//     ioctl(fd, VIDIOC_STREAMON, &type);
// }

// V4L2VideoSource::~V4L2VideoSource() {
//     int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//     ioctl(fd, VIDIOC_STREAMOFF, &type);
//     munmap(rawBuffer, buf.length);
//     ::close(fd);    
//     delete[] jpegBuffer;
//     tjDestroy(tjInstance);
// }

// void V4L2VideoSource::doGetNextFrame() {
//     captureFrame();
//     if (jpegSize > 0) {
//         // Deliver the frame
//         if (jpegSize > fMaxSize) {
//             fFrameSize = fMaxSize;
//             fNumTruncatedBytes = jpegSize - fMaxSize;
//         } else {
//             fFrameSize = jpegSize;
//         }
//         memcpy(fTo, jpegBuffer, fFrameSize);
//         gettimeofday(&fPresentationTime, NULL);
//         FramedSource::afterGetting(this);
//     }
// }

// void V4L2VideoSource::captureFrame() {
//     // Queue and dequeue buffer
//     buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//     buf.memory = V4L2_MEMORY_MMAP;
//     ioctl(fd, VIDIOC_QBUF, &buf);
//     ioctl(fd, VIDIOC_DQBUF, &buf);

//     // Compress to JPEG
//     long unsigned int outSize = 0;
//     tjCompress2(tjInstance, rawBuffer, 640, 0, 480, TJPF_YUY420, &jpegBuffer, &outSize, TJSAMP_420, 75, 0);
//     jpegSize = outSize;
// }





#include "V4L2VideoSource.hh"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <turbojpeg.h>

#define WIDTH 640
#define HEIGHT 480

V4L2VideoSource* V4L2VideoSource::createNew(UsageEnvironment& env) {
    return new V4L2VideoSource(env);
}

V4L2VideoSource::V4L2VideoSource(UsageEnvironment& env)
    : FramedSource(env), fd(-1), rawBuffer(NULL),  yuvBuffer(NULL) ,jpegBuffer(NULL), jpegSize(0), tjInstance(tjInitCompress()), bufLength(0) {
    // Open V4L2 device
    fd = open("/dev/video0", O_RDWR);
    if (fd < 0) {
        envir() << "Failed to open video device\n";
        return;
    }

    // Configure V4L2
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        envir() << "Failed to set video format\n";
        ::close(fd);
        fd = -1;
        return;
    }

    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        envir() << "Failed to request buffers\n";
        ::close(fd);
        fd = -1;
        return;
    }

    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
        envir() << "Failed to query buffer\n";
        ::close(fd);
        fd = -1;
        return;
    }
    rawBuffer = (unsigned char*)mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
    if (rawBuffer == MAP_FAILED) {
        envir() << "Failed to map buffer\n";
        ::close(fd);
        fd = -1;
        return;
    }
    bufLength = buf.length; // Store buffer length
   // jpegBuffer = new unsigned char[buf.length]; // Allocate enough space
   yuvBuffer = new unsigned char[WIDTH * HEIGHT * 3 / 2]; // YUV420
   jpegBuffer = new unsigned char[WIDTH * HEIGHT * 2];


   mjpegFile = fopen("output.mjpeg", "wb");
   if(!mjpegFile){
    envir() << "Failed to open MJPEG file for writing\n";
   }

    // Start streaming
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        envir() << "Failed to start streaming\n";
        munmap(rawBuffer, bufLength);
        ::close(fd);
        fd = -1;
        return;
    }
}

V4L2VideoSource::~V4L2VideoSource() {
    if (fd >= 0) {
        int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ioctl(fd, VIDIOC_STREAMOFF, &type);
        munmap(rawBuffer, bufLength);
        ::close(fd);
    }
    delete[] jpegBuffer;
    if (tjInstance) tjDestroy(tjInstance);
    if(mjpegFile){
        fclose(mjpegFile);
        envir() << "MJPEG file closed\n";
    }
}

void V4L2VideoSource::doGetNextFrame() {
    captureFrame();
    if (jpegSize > 0) {
        envir() << "Sending video frame, size: " << jpegSize << "\n"; // Debug
        // Deliver the frame
        if (jpegSize > fMaxSize) {
            fFrameSize = fMaxSize;
            fNumTruncatedBytes = jpegSize - fMaxSize;
        } else {
            fFrameSize = jpegSize;
        }
        memcpy(fTo, jpegBuffer, fFrameSize);
        gettimeofday(&fPresentationTime, NULL);
        envir() << "Frame delivered to RTP , sink : \n"; // Debug
        FramedSource::afterGetting(this);
    }
    else{
        envir() << "No video frame captured\n"; // Debug
    }
}

void V4L2VideoSource::captureFrame() {
    envir() << "[captureFrame] Entered method\n";

    if (fd < 0) {
        envir() << "[captureFrame] Invalid camera file descriptor (fd = " << fd << ")\n";
        return;
    }

    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;

    if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
        envir() << "[captureFrame] Failed to queue buffer: " << strerror(errno) << "\n";
        return;
    }

    if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
        envir() << "[captureFrame] Failed to dequeue buffer: " << strerror(errno) << "\n";
        return;
    }

    envir() << "[captureFrame] Frame captured from camera\n";

    // Prepare YUV420 buffer layout
    unsigned char* yPlane = yuvBuffer;
    unsigned char* uPlane = yPlane + WIDTH * HEIGHT;
    unsigned char* vPlane = uPlane + (WIDTH * HEIGHT) / 4;

    // Convert YUYV to YUV420
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x += 2) {
            int index = y * WIDTH * 2 + x * 2;

            unsigned char y0 = rawBuffer[index];
            unsigned char u  = rawBuffer[index + 1];
            unsigned char y1 = rawBuffer[index + 2];
            unsigned char v  = rawBuffer[index + 3];

            yPlane[y * WIDTH + x]     = y0;
            yPlane[y * WIDTH + x + 1] = y1;

            if ((y % 2 == 0) && (x % 2 == 0)) {
                int uvIndex = (y / 2) * (WIDTH / 2) + (x / 2);
                uPlane[uvIndex] = u;
                vPlane[uvIndex] = v;
            }
        }
    }

    // Compress to JPEG
    unsigned long outSize = 0;
    int result = tjCompress2(tjInstance,
                             yuvBuffer,
                             WIDTH, 0, HEIGHT,
                             TJPF_RGB,
                             &jpegBuffer,
                             &outSize,
                             TJSAMP_420,
                             75, // quality
                             0);

    // Clean up YUV buffer
    delete[] yuvBuffer;
    
    yuvBuffer = nullptr;

    if (result < 0) {
        envir() << "[captureFrame] JPEG compression failed: " << tjGetErrorStr2(tjInstance) << "\n";
        jpegSize = 0;
        return;
    }

    jpegSize = outSize;
    if (mjpegFile && jpegSize > 0) {
        fwrite(jpegBuffer, 1, jpegSize, mjpegFile);
        fflush(mjpegFile);  // flush ensures it's written in real-time
    }
    
    envir() << "[captureFrame] Frame compressed to JPEG, size: " << jpegSize << " bytes\n";
}


