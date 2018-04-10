//
//  PipeNetUser.hpp
//  BattleServer
//
//  Created by lwd on 2017/11/21.
//  Copyright © 2017年 lwd. All rights reserved.
//

#ifndef PipeNetUser_hpp
#define PipeNetUser_hpp

#include <stdio.h>
#include <asio.hpp>
#include <vector>
#include <deque>
#include "NetHead.h"

class PipeNetUser
{
public:
    static PipeNetUser* getInstanse();
    PipeNetUser(asio::io_service &io_service,int write_fd,int read_fd);
    ~PipeNetUser();
    void doCommand(tcp_main_net_data_type* p_item);
    void readCommand();
    void sendCommand(tcp_net_main_data_type *item);
    void popFistItem();
    void writeData();
private:
    asio::posix::stream_descriptor io_net_write;
    asio::posix::stream_descriptor io_net_read;
    vector<intptr_t> write_items;
    vector<intptr_t> ready_items;
};
#endif /* PipeNetUser_hpp */
