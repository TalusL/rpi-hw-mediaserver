//
// Created by wind on 2021/2/9.
//
#include <csignal>
#include "MediaServer.h"

#include "V4l2Capture.h"

#include "VideoEncoder.h"

int main(){
    //set logger
    Logger::Instance().add(std::make_shared<ConsoleChannel>("ConsoleChannel", LTrace));

    //start media server
//    MediaServer::Ptr server = make_shared<MediaServer>();
//    server->start();

    int width = 1920,height = 1080;

    V4L2DeviceParameters param("/dev/video0", V4L2_PIX_FMT_YUV420, width, height, 25, 0);
    V4l2Capture* videoCapture = V4l2Capture::create(param, V4l2Access::IOTYPE_MMAP);


    ofstream f("aaa.h264",iostream::binary);
    VideoEncoder venc(width,height,1000000);
    venc.init();
    venc.setOnEncodedFrameCallback([&f](uint8_t *outBuffer,size_t bufferLen){
        f.write(reinterpret_cast<const char *>(outBuffer), bufferLen);
        f.flush();
    });
    int size = width*height*3/2;
    char *buff = new char[size];
    for (int i = 0; i < 1000; ++i) {
        timeval timeout = {0,4000};
        if(videoCapture->isReadable(&timeout) == 1){
            videoCapture->read(buff,size);
            venc.encodeFrame(reinterpret_cast<uint8_t *>(buff), size);
        }
    }

    //set interrupt signal
    static semaphore sem;
    signal(SIGINT, [](int) {
        InfoL << "SIGINT:exit";
        exit(0);
    });
    sem.wait();
}