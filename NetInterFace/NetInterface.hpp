//
//  NetInterface.hpp
//  BattleServer
//
//  Created by lwd on 2017/11/22.
//  Copyright © 2017年 lwd. All rights reserved.
//  命令方式，直接接收管道中的命令处理连接，数据，关闭操作。只支持单线程调用

#ifndef NetInterface_hpp
#define NetInterface_hpp

#include <stdio.h>
#include <asio.hpp>
#include <vector>
#include <deque>
#include "NetHead.h"

class NetInterface
{
public:
    static NetInterface* getInstanse();
    NetInterface();
    ~NetInterface();
    void doCommand(tcp_net_main_data_type* p_item);
    int readCommand();
    void sendCommand(tcp_main_net_data_type *item);
    void writeData();
    void connectTcp(const char *ip,int port);
    void closeTcp(int id);
    void sendMsg(vector<int> &userids,int data_len,const char* data);
    void sendMsg(string userids,int data_len,const char* data);
    void sendUdp(vector<string> &ips,vector<int>&ports,int data_len,const char* data);
    void sendUdp(string ips,string ports,int data_len,const char* data);
    inline int getDataCommand(int idx){
        if(idx<readed_datas.size())
            return readed_datas[idx]->command;
        return -1;
    }
    inline int getDataUserid(int idx){
        if(idx<readed_datas.size())
            return readed_datas[idx]->userid;
        return -1;
    }
    inline int getDataLen(int idx){
        if(idx<readed_datas.size())
            return readed_datas[idx]->len;
        return -1;
    }
    inline int getDataPbid(int idx){
        if(idx<readed_datas.size())
            return readed_datas[idx]->pb_id;
        return -1;
    }
    inline const char* getDataBuf(int idx){
        if(idx<readed_datas.size())
            return readed_datas[idx]->msg.get();
        return nullptr;
    }
    inline int getPort(int idx){
        if(idx<readed_datas.size())
            return readed_datas[idx]->port;
        return -1;
    }
    inline const char* getIP(int idx){
        if(idx<readed_datas.size())
            return readed_datas[idx]->ip.c_str();
        return nullptr;
    }
    inline vector<shared_ptr<tcp_net_main_data_type>>& getReadedDatas()
    {
        return readed_datas;
    }
private:
    int io_main_write;
    int io_main_read;
    vector<intptr_t> ready_items;
    vector<shared_ptr<tcp_net_main_data_type>> readed_datas;
};
#endif /* NetInterface_hpp */
