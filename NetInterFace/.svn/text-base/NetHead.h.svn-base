//
//  NetHead.h
//  BattleServer
//
//  Created by lwd on 2017/11/13.
//  Copyright © 2017年 lwd. All rights reserved.
//

#ifndef NetHead_h
#define NetHead_h
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

#define  DATA_TRANS_SPEED  512 //n%8=0
#define PIPE_TRANS_SPEED 256 //n%8=0
#define UDT_RECEIVE_DATA_LEN 1024*20
//#define TCP_SERVER_IP "127.0.0.1"
#define TCP_SERVER_PORT 9800
//#define UDP_SERVER_IP "127.0.0.1"
#define UDP_SERVER_PORT 9801

static char MAIN_PIPE_COMMAND_READE_BUF[PIPE_TRANS_SPEED];
static char NET_PIPE_COMMAND_READE_BUF[PIPE_TRANS_SPEED];
static char UDT_RECEIVE_DATA[UDT_RECEIVE_DATA_LEN];

typedef enum
{
    NET_TCP_DATA_TRANS,
    NET_HTTP_DATA_SEND,
    NET_TCP_CONNECT,
    NET_TCP_CLOSE,
    NET_TCP_ACCEPT,
    NET_UDP_DATA_TRANS
}NET_COMMAND_TYPE;
//send 分包发送，限制流量
typedef struct _socket_main_net_data_type
{
    NET_COMMAND_TYPE command;
    vector<int32_t> userids;//tcp时使用userid. udp不使用
    vector<string> ips;//udp时直接使用ip发送多人,tcp连接，关闭时使用
    vector<int> ports;//udp时直接使用port发送多人,tcp连接，关闭时使用
    int32_t len=0;//=包头长度+数据长度
    string msg;//tcp分包发送，限制流量。 udp，考虑对方是服务器时udp不能分包发送
}tcp_main_net_data_type;
//receive 接收完整包
typedef struct _socket_net_main_data_type
{
    NET_COMMAND_TYPE command;
    int32_t userid=0;
    string ip;
    int port=0;
    int32_t pb_id=0;
    int32_t len=0;//=数据长度 接收的时候已经解析
    char pb_head[4];// data header
    shared_ptr<char> msg=NULL;//根据包体长度收包，这样不用再组包，可直接使用。以免浪费内存拷贝。由服务器和客户端的分包发送来限制流量
    
}tcp_net_main_data_type;

typedef struct _udp_cur_write_data_type
{
    shared_ptr<tcp_main_net_data_type> item;
    string ip;
    int port;
}udp_cur_write_data_type;

string numberToString(int num);
int stringToNumber(string strNum);
std::vector<std::string> splitStr(std::string str,std::string decollator);
void initNetInterFace();
#endif /* NetHead_h */
