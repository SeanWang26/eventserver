#ifndef NMC_ERROR_CODE_H
#define NMC_ERROR_CODE_H

#define NMC_SERVER_RETURN_ERROR               -89 //服务器响应返回失败
#define NMC_ERROR_PUSHNEWAFFAIR               -90 //推入AFFAIR失败
#define NMC_ERROR_PARAMETER                   -91 //参数错误
#define NMC_RECEIVE_REQ_FAILED                -92 //发送请求失败
#define NMC_PASER_RSP_FAILED                  -93 //解析响应失败
#define NMC_SEND_REQ_FAILED                   -94 //发送请求失败
#define NMC_GEN_REQ_FAILED                    -95 //生成请求失败
#define NMC_REQUEST_FAILED                    -96 //请求失败
#define NMC_CONNECT_FAILED                    -97 //连接失败
#define NMC_LONIN_FAILED                      -98 //登陆失败
#define NMC_INVALIED_HANDLE                   -99 //非法的句柄

#define NMC_SEND_DATA_FAILED                  -150 //数据发送失败
#define NMC_RECEIVE_DATA_FAILED               -151 //数据接收失败

#define NMC_ERROR_LOAD_LIB_FAILED             -496 //库加载失败
#define NMC_ERROR_NOT_INIT_FAILED             -497 //调用次序错误
#define NMC_ERROR_CALL_ORDER                  -498 //调用次序错误
#define NMC_ERROR_UNKNOWN                     -499 //未知的错误
#define NMC_NOT_IMPLEMENT                     -500 //未实现

















#endif