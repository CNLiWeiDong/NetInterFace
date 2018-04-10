//
//  TcpUser.hpp
//  BattleServer
//
//  Created by lwd on 2017/11/21.
//  Copyright © 2017年 lwd. All rights reserved.
//

#ifndef TcpUser_hpp
#define TcpUser_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <deque>
#include <asio.hpp>
#include "NetHead.h"

using namespace std;
class TcpUser
{
protected:
    int userid;
    string ip;
    int port;
    shared_ptr<asio::ip::tcp::socket> fd;
    deque<shared_ptr<tcp_main_net_data_type>> write_items;
public:
    TcpUser(shared_ptr<asio::ip::tcp::socket> _fd,string _ip,int _port);
    ~TcpUser();
    inline int getUserid(){return userid;}
    inline string getUserIP(){return ip;}
    inline int getUserPort(){return port;}
    void readData();
    void sendData(shared_ptr<tcp_main_net_data_type> item);
    void popFistItem();
protected:
    void writeData();
};
#endif /* TcpUser_hpp */
