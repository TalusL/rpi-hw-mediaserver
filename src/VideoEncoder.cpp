//
// Created by admin on 2021/2/14.
//

#include "VideoEncoder.h"
#include "Util/logger.h"

using namespace toolkit;
bool VideoEncoder::init() {
    bcm_host_init();

    _client = shared_ptr<ILCLIENT_T>(ilclient_init(),&ilclient_destroy);

    if (!_client) {
        ErrorL << "init client error.";
        return false;
    }

    if (OMX_Init() != OMX_ErrorNone) {
        return false;
    }

    OMX_ERRORTYPE error;
    COMPONENT_T * pEnc;
    // create video_encode
    error = static_cast<OMX_ERRORTYPE>(ilclient_create_component(_client.get(), &pEnc, "video_encode",
                                                                 static_cast<ILCLIENT_CREATE_FLAGS_T>(
                                                                         ILCLIENT_DISABLE_ALL_PORTS |
                                                                         ILCLIENT_ENABLE_INPUT_BUFFERS |
                                                                         ILCLIENT_ENABLE_OUTPUT_BUFFERS)));
    _video_encode = shared_ptr<COMPONENT_T>(pEnc,[](COMPONENT_T * comp){
        DebugL << "disabling port buffers for 200 and 201...";
        ilclient_disable_port_buffers(comp, 200, nullptr, nullptr, nullptr);
        ilclient_disable_port_buffers(comp, 201, nullptr, nullptr, nullptr);


        COMPONENT_T *list[5];
        list[0] = comp;
        ilclient_state_transition(list, OMX_StateIdle);
        ilclient_state_transition(list, OMX_StateLoaded);

        ilclient_cleanup_components(list);

        OMX_Deinit();
    });
    if (error != 0) {
        ErrorL << "ilclient_create_component() for video_encode failed with "<<error;
    }


    // get current settings of video_encode component from port 200
    memset(&_def, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
    _def.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    _def.nVersion.nVersion = OMX_VERSION;
    _def.nPortIndex = 200;

    if (OMX_GetParameter
        (ILC_GET_HANDLE(_video_encode.get()), OMX_IndexParamPortDefinition,
         &_def) != OMX_ErrorNone) {
        ErrorL<< " OMX_GetParameter() for video_encode port 200 failed!";
        return false;
    }


    // Port 200: in 1/1 115200 16 enabled,not pop.,not cont. 320x240 320x240 @1966080 20
    _def.format.video.nFrameWidth = _width;
    _def.format.video.nFrameHeight = _height;
    _def.format.video.xFramerate = 30 << 16;
    _def.format.video.nSliceHeight = ALIGN_UP(_def.format.video.nFrameHeight, 16);
    _def.format.video.nStride = _def.format.video.nFrameWidth;
    _def.format.video.eColorFormat = OMX_COLOR_FormatYUV420PackedPlanar;

    error = OMX_SetParameter(ILC_GET_HANDLE(_video_encode.get()),
                         OMX_IndexParamPortDefinition, &_def);
    if (error != OMX_ErrorNone) {
        ErrorL<< " OMX_SetParameter() for video_encode port 200 failed with "<< error <<"!\n";
        return false;
    }

    memset(&_format, 0, sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE));
    _format.nSize = sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE);
    _format.nVersion.nVersion = OMX_VERSION;
    _format.nPortIndex = 201;
    _format.eCompressionFormat = OMX_VIDEO_CodingAVC;

    InfoL<<"OMX_SetParameter for video_encode:201...";
    error = OMX_SetParameter(ILC_GET_HANDLE(_video_encode.get()),
                         OMX_IndexParamVideoPortFormat, &_format);
    if (error != OMX_ErrorNone) {
        ErrorL<< " OMX_SetParameter() for video_encode port 201 failed with "<< error <<"!";
        return false;
    }

    OMX_VIDEO_PARAM_BITRATETYPE bitrateType;
    // set current bitrate to 1Mbit
    memset(&bitrateType, 0, sizeof(OMX_VIDEO_PARAM_BITRATETYPE));
    bitrateType.nSize = sizeof(OMX_VIDEO_PARAM_BITRATETYPE);
    bitrateType.nVersion.nVersion = OMX_VERSION;
    bitrateType.eControlRate = OMX_Video_ControlRateVariable;
    bitrateType.nTargetBitrate = _bitRate;
    bitrateType.nPortIndex = 201;
    error = OMX_SetParameter(ILC_GET_HANDLE(_video_encode.get()),
                         OMX_IndexParamVideoBitrate, &bitrateType);
    if (error != OMX_ErrorNone) {
        ErrorL<< " OMX_SetParameter() for bitrate for video_encode port 201 failed with "<< error <<"!";
        return false;
    }


    // get current bitrate
    memset(&bitrateType, 0, sizeof(OMX_VIDEO_PARAM_BITRATETYPE));
    bitrateType.nSize = sizeof(OMX_VIDEO_PARAM_BITRATETYPE);
    bitrateType.nVersion.nVersion = OMX_VERSION;
    bitrateType.nPortIndex = 201;

    if (OMX_GetParameter
        (ILC_GET_HANDLE(_video_encode.get()), OMX_IndexParamVideoBitrate,
         &bitrateType) != OMX_ErrorNone) {
        ErrorL<< " OMX_GetParameter() for video_encode for bitrate port 201 failed!";
        return false;
    }
    InfoL << "Current Bitrate=" << bitrateType.nTargetBitrate;


    InfoL<<"encode to idle...";
    if (ilclient_change_component_state(_video_encode.get(), OMX_StateIdle) == -1) {
        ErrorL<< " ilclient_change_component_state(video_encode, OMX_StateIdle) failed";
    }

    InfoL << "enabling port buffers for 200...";
    if (ilclient_enable_port_buffers(_video_encode.get(), 200, nullptr, nullptr, nullptr) != 0) {
        ErrorL<< " enabling port buffers for 200 failed!";
        return false;
    }

    InfoL << "enabling port buffers for 201...";
    if (ilclient_enable_port_buffers(_video_encode.get(), 201, nullptr, nullptr, nullptr) != 0) {
        ErrorL<<"enabling port buffers for 201 failed!";
        return false;
    }

    InfoL<<"encode to executing...";
    ilclient_change_component_state(_video_encode.get(), OMX_StateExecuting);
    
    return true;
}

void VideoEncoder::encodeFrame(uint8_t * yuvBuffer,size_t bufferLen){
    OMX_BUFFERHEADERTYPE *buf;
    OMX_BUFFERHEADERTYPE *out;
    buf = ilclient_get_input_buffer(_video_encode.get(), 200, 1);
    if (buf == nullptr) {
        ErrorL<< "Doh, no buffers for me!";
    } else {
        /* fill it */
        buf->nFilledLen = bufferLen;
        memmove(buf->pBuffer, yuvBuffer, buf->nFilledLen);

        if (OMX_EmptyThisBuffer(ILC_GET_HANDLE(_video_encode.get()), buf) !=
            OMX_ErrorNone) {
            ErrorL<<"Error emptying buffer!";
        }

        out = ilclient_get_output_buffer(_video_encode.get(), 201, 1);

        if (out != nullptr) {
            if (_onEncodedFrameCallback&&out->nFilledLen){
                _onEncodedFrameCallback(out->pBuffer, out->nFilledLen);
            }
            out->nFilledLen = 0;
        } else {
            WarnL<<"Not getting it!";
        }

        OMX_ERRORTYPE error = OMX_FillThisBuffer(ILC_GET_HANDLE(_video_encode.get()), out);
        if (error != OMX_ErrorNone) {
            ErrorL<<"Error sending buffer for filling: " << error;
        }
    }
}

