//
// Created by wind on 2021/2/9.
//

#ifndef RPI_HW_MEDIASERVER_MEDIASERVER_H
#define RPI_HW_MEDIASERVER_MEDIASERVER_H

#include <Network/TcpServer.h>
#include <Http/HttpSession.h>
#include <Rtmp/RtmpSession.h>
#include <Rtsp/RtspSession.h>

class MediaServer {
public:
    typedef shared_ptr<MediaServer> Ptr;
    //server handles
    TcpServer::Ptr _rtspSrv;
    TcpServer::Ptr _rtmpSrv;
    TcpServer::Ptr _httpSrv;

public:
    void start();
};


#endif //RPI_HW_MEDIASERVER_MEDIASERVER_H
