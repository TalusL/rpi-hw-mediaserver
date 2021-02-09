//
// Created by wind on 2021/2/9.
//

#include "MediaServer.h"


void MediaServer::start() {
    _httpSrv = make_shared<TcpServer>();
    _rtmpSrv = make_shared<TcpServer>();
    _rtspSrv = make_shared<TcpServer>();

    _httpSrv->start<HttpSession>(8080);
    _rtmpSrv->start<RtmpSession>(1935);
    _rtspSrv->start<RtspSession>(554);
}
