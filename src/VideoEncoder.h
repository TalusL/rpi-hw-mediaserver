//
// Created by admin on 2021/2/14.
//

#ifndef RPI_HW_MEDIASERVER_VIDEOENCODER_H
#define RPI_HW_MEDIASERVER_VIDEOENCODER_H

extern "C"{
#include <ilclient.h>
#include <bcm_host.h>
};
#include <memory>
#include <functional>
#include <utility>

using namespace std;
class VideoEncoder {

private:
    OMX_VIDEO_PARAM_PORTFORMATTYPE _format{};
    OMX_PARAM_PORTDEFINITIONTYPE _def{};
    shared_ptr<COMPONENT_T> _video_encode;
    shared_ptr<ILCLIENT_T> _client;
    int _status = 0;
    int _framenumber = 0;
    int _width = 0;
    int _height = 0;
    ulong _bitRate = 0;
    function<void(uint8_t *outBuffer,size_t bufferLen)> _onEncodedFrameCallback = nullptr;

public:
    VideoEncoder(int width,int height,ulong bitRate){
        _width = width;
        _height = height;
        _bitRate = bitRate;
    }
    bool init();
    void encodeFrame(uint8_t * yuvBuffer,size_t bufferLen);
    void setOnEncodedFrameCallback(function<void(uint8_t *outBuffer,size_t bufferLen)> cb){
        _onEncodedFrameCallback = std::move(cb);
    }
};


#endif //RPI_HW_MEDIASERVER_VIDEOENCODER_H
