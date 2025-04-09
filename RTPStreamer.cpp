// #include "liveMedia.hh"
// #include "BasicUsageEnvironment.hh"
// #include "GroupsockHelper.hh"
// #include "V4L2VideoSource.hh"
// #include "ALSAAudioSource.hh"
// #include <arpa/inet.h>
// #include "Groupsock.hh"
// #include <netinet/in.h>

// int main(int argc, char** argv) {
//     if (argc != 2) {
//         fprintf(stderr, "Usage: %s <destination-ip>\n", argv[0]);
//         return 1;
//     }

//     TaskScheduler* scheduler = BasicTaskScheduler::createNew();
//     UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

//     // Destination address and ports
//     struct in_addr destAddr;
//     // destAddr.s_addr = inet_addr(argv[1]);    


//     destAddr.s_addr = inet_addr(argv[1]);
//     if (destAddr.s_addr == (in_addr_t)(-1)) {
//         *env << "Invalid IP address: " << argv[1] << "\n";
//         return 1;
//     }
//     const Port videoPort(5000);
//     const Port audioPort(5002);



//     // Video setup
//     Groupsock videoGroupsock(*env, destAddr, videoPort, 255);
//     V4L2VideoSource* videoSource = V4L2VideoSource::createNew(*env);
//     if (videoSource == NULL) {
//         *env << "Failed to create video source\n";
//         return 1;
//     }
//     RTPSink* videoSink = JPEGVideoRTPSink::createNew(*env, &videoGroupsock);
//     videoSink->startPlaying(*videoSource, NULL, NULL);

//     // Audio setup
//     Groupsock audioGroupsock(*env, destAddr, audioPort, 255);
//     ALSAAudioSource* audioSource = ALSAAudioSource::createNew(*env);
//     if (audioSource == NULL) {
//         *env << "Failed to create audio source\n";
//         return 1;
//     }
//     RTPSink* audioSink = SimpleRTPSink::createNew(*env, &audioGroupsock, 96, 48000, "audio", "OPUS", 2);
//     audioSink->startPlaying(*audioSource, NULL, NULL);

//     *env << "Streaming RTP to " << argv[1] << " (video port 5000, audio port 5002)\n";

//     env->taskScheduler().doEventLoop();
//     return 0;
// }



// #include "liveMedia.hh"
// #include "BasicUsageEnvironment.hh"
// #include "GroupsockHelper.hh"
// #include "V4L2VideoSource.hh"
// #include "ALSAAudioSource.hh"
// #include <arpa/inet.h>      // For inet_addr
// #include <netinet/in.h>     // For sockaddr_in, AF_INET
// #include <cstring>          // For memset, memcpy

// int main(int argc, char** argv) {
//     if (argc != 2) {
//         fprintf(stderr, "Usage: %s <destination-ip>\n", argv[0]);
//         return 1;
//     }

//     // Create scheduler and environment
//     TaskScheduler* scheduler = BasicTaskScheduler::createNew();
//     UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

//     // Destination address setup
//     struct in_addr destAddr;
//     destAddr.s_addr = inet_addr(argv[1]);
//     if (destAddr.s_addr == (in_addr_t)(-1)) {
//         *env << "Invalid IP address: " << argv[1] << "\n";
//         env->reclaim();  // Clean up environment
//         delete scheduler; // Clean up scheduler
//         return 1;
//     }

//     // Convert struct in_addr to struct sockaddr_storage
//     struct sockaddr_in sin;
//     memset(&sin, 0, sizeof(sin));         // Clear the structure
//     sin.sin_family = AF_INET;             // IPv4 address family
//     sin.sin_addr = destAddr;              // Set the IP address

//     struct sockaddr_storage groupAddr;
//     memcpy(&groupAddr, &sin, sizeof(sin)); // Copy to sockaddr_storage

//     // Define ports
//     const Port videoPort(5000);
//     const Port audioPort(5002);

//     // Video setup
//     Groupsock videoGroupsock(*env, groupAddr, videoPort, 255);
//     V4L2VideoSource* videoSource = V4L2VideoSource::createNew(*env);
//     if (videoSource == NULL) {
//         *env << "Failed to create video source\n";
//         env->reclaim();
//         delete scheduler;
//         return 1;
//     }
//     RTPSink* videoSink = JPEGVideoRTPSink::createNew(*env, &videoGroupsock);
//     videoSink->startPlaying(*videoSource, NULL, NULL);

//     // Audio setup
//     Groupsock audioGroupsock(*env, groupAddr, audioPort, 255);
//     ALSAAudioSource* audioSource = ALSAAudioSource::createNew(*env);
//     if (audioSource == NULL) {
//         *env << "Failed to create audio source\n";
//         env->reclaim();
//         delete scheduler;
//         return 1;
//     }
//     RTPSink* audioSink = SimpleRTPSink::createNew(*env, &audioGroupsock, 96, 48000, "audio", "OPUS", 2);
//     audioSink->startPlaying(*audioSource, NULL, NULL);


//     //

//     FramedSource* videoSource1 = V4L2VideoSource::createNew(*env);
//     if (videoSource1 == NULL) {
//         *env << "Failed to create video source\n";
//         return 1;
//     }
//     // Inform user of streaming details

//     videoSink->startPlaying(*videoSource, NULL, NULL);

//     //


//     *env << "Streaming RTP to " << argv[1] << " (video port 5000, audio port 5002)\n";



//     /// FILE METHOD .sdp

//     const char* sdpFileName = "stream.sdp";
// FILE* sdpFile = fopen(sdpFileName, "w");
// if (sdpFile == NULL) {
//     *env << "Failed to create SDP file\n";
//     env->reclaim();
//     delete scheduler;
//     return 1;
// }

// // Write the SDP content
// fprintf(sdpFile,
//     "v=0\n"
//     "o=- 0 0 IN IP4 %s\n"
//     "s=RTP Stream from RTPStreamer\n"
//     "c=IN IP4 %s\n"
//     "t=0 0\n"
//     "m=video 5000 RTP/AVP 26\n"
//     "a=rtpmap:26 JPEG/90000\n"
//     "m=audio 5002 RTP/AVP 96\n"
//     "a=rtpmap:96 opus/48000/2\n",
//     argv[1], argv[1]);

// fclose(sdpFile);
// *env << "SDP file 'stream.sdp' generated. Open it in VLC.\n";




//     // Start the event loop
//     env->taskScheduler().doEventLoop();

//     // Cleanup (unreachable due to doEventLoop, but good practice)
//     env->reclaim();
//     delete scheduler;
//     return 0;
// }




#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "GroupsockHelper.hh"
#include "V4L2VideoSource.hh"
#include "ALSAAudioSource.hh"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <destination-ip>\n", argv[0]);
        return 1;
    }

    // 1. Setup Live555 environment
    TaskScheduler* scheduler = BasicTaskScheduler::createNew();
    UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

    // 2. Parse destination IP
    struct in_addr destAddr;
    destAddr.s_addr = inet_addr(argv[1]);
    if (destAddr.s_addr == (in_addr_t)(-1)) {
        *env << "Invalid IP address: " << argv[1] << "\n";
        env->reclaim();
        delete scheduler;
        return 1;
    }

    // 3. Convert to sockaddr_storage
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr = destAddr;

    struct sockaddr_storage groupAddr;
    memcpy(&groupAddr, &sin, sizeof(sin));

    // Ports
    const Port videoPort(5000);
    const Port audioPort(5002);

    // 4. VIDEO stream setup (MJPEG)
    Groupsock videoGroupsock(*env, groupAddr, videoPort, 255);
    V4L2VideoSource* videoSource = V4L2VideoSource::createNew(*env);
    if (!videoSource) {
        *env << "Failed to create video source\n";
        env->reclaim();
        delete scheduler;
        return 1;
    }
    RTPSink* videoSink = JPEGVideoRTPSink::createNew(*env, &videoGroupsock);
    videoSink->startPlaying(*videoSource, NULL, NULL);
   

    // 5. AUDIO stream setup (Opus)
    Groupsock audioGroupsock(*env, groupAddr, audioPort, 255);
    ALSAAudioSource* audioSource = ALSAAudioSource::createNew(*env);
    if (!audioSource) {
        *env << "Failed to create audio source\n";
        env->reclaim();
        delete scheduler;
        return 1;
    }
    RTPSink* audioSink = SimpleRTPSink::createNew(*env, &audioGroupsock, 96, 48000, "audio", "OPUS", 2);
    audioSink->startPlaying(*audioSource, NULL, NULL);
    *env << "Streaming RTP to " << argv[1] << " (video port 5000, audio port 5002)\n";

    
    env->taskScheduler().doEventLoop();

    // 8. Cleanup (unreachable in normal operation)
    env->reclaim();
    delete scheduler;
    return 0;
}



