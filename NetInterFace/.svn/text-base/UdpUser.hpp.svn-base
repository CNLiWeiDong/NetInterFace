//
//  UdpUser.hpp
//  BesiegedCity
//
//  Created by lwd on 2017/12/2.
//

#ifndef UdpUser_hpp
#define UdpUser_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <deque>
#include <asio.hpp>
#include "NetHead.h"

using namespace std;
class UdpUser
{
protected:
    string ip;
    int port;
    shared_ptr<asio::ip::udp::socket> fd;
    deque<udp_cur_write_data_type> write_items;
    asio::ip::udp::endpoint sender_endpoint;
public:
    UdpUser();
    ~UdpUser();
    static UdpUser* getInstanse();
    void readData();
    void sendData(shared_ptr<tcp_main_net_data_type> item);
    void popFistItem();
    inline asio::ip::udp::endpoint& getSenderPoint(){return sender_endpoint;};
protected:
    void writeData();
};

#endif /* UdpUser_hpp */
