//
//  PipeNetUser.cpp
//  BattleServer
//
//  Created by lwd on 2017/11/21.
//  Copyright © 2017年 lwd. All rights reserved.
//

#include "PipeNetUser.hpp"
#include "TcpGroup.hpp"
#include "UdpUser.hpp"

extern int g_pipe_net_to_main[2];
extern int g_pipe_net_write;
extern int g_pipe_main_read;
extern int g_pipe_main_to_net[2];
extern int g_pipe_main_write;
extern int g_pipe_net_read;
extern asio::io_service g_io_service;

extern void free_char_data(void* data);

static PipeNetUser * pipe_net_user_instanse = NULL;
PipeNetUser* PipeNetUser::getInstanse()
{
    if(pipe_net_user_instanse)
        return pipe_net_user_instanse;
    pipe_net_user_instanse = new PipeNetUser(g_io_service,g_pipe_net_write,g_pipe_net_read);
    return pipe_net_user_instanse;
}

PipeNetUser::PipeNetUser(asio::io_service &io_service,int write_fd,int read_fd):
io_net_write(io_service,write_fd),
io_net_read(io_service,read_fd)
{
    
}
PipeNetUser::~PipeNetUser()
{
    io_net_write.close();
    io_net_read.close();
}
void PipeNetUser::doCommand(tcp_main_net_data_type* p_item)
{
    shared_ptr<tcp_main_net_data_type> item(p_item); //auto delete ptr
    auto &cur_read = *p_item;
    if(cur_read.command==NET_TCP_DATA_TRANS)
    {
        for(auto &it:cur_read.userids)
        {
            auto user = TcpGroup::getInstanse()->findUser(it);
            if(user)
            {
                user->sendData(item);//auto delete ptr
            }
        }
    }
    else if(cur_read.command==NET_TCP_CONNECT)//connect
    {
        if(cur_read.ips.size()<1 || cur_read.ports.size()<1)
            return;
//        string addr(cur_read.msg,cur_read.len);
//        auto split = find(addr.begin(),addr.end(),',');
//        string ip(addr.begin(),split);
//        string port(split+1,addr.end());
        TcpGroup::getInstanse()->connect(cur_read.ips[0],cur_read.ports[0]);
    }
    else if(cur_read.command==NET_TCP_CLOSE)
    {
        for(auto &it:cur_read.userids)
        {
            TcpGroup::getInstanse()->removeUser(it);
        }
    }
    else if (cur_read.command==NET_UDP_DATA_TRANS)
    {
        auto user = UdpUser::getInstanse();
        user->sendData(item);
    }
}
void PipeNetUser::readCommand()
{
    auto data = NET_PIPE_COMMAND_READE_BUF;//(char*)malloc(PIPE_TRANS_SPEED);
//    std::shared_ptr<char> data_ptr(data,free_char_data);
    io_net_read.async_read_some(asio::buffer(data,PIPE_TRANS_SPEED), [this,data](const asio::error_code &ec, std::size_t bytes_transferred)
                        {
                            if(!ec && bytes_transferred>0)
                            {
                                intptr_t *intptr_data = (intptr_t*)data;
                                auto len = bytes_transferred/sizeof(uintptr_t);
                                for(int i=0;i<len;i++)
                                {
                                    intptr_t data_p_num = *((intptr_t*)(intptr_data+i));
                                    auto p_item = (tcp_main_net_data_type*)data_p_num;
                                    this->doCommand(p_item);
                                }
                                auto end_num =  bytes_transferred%sizeof(uintptr_t);
                                if(end_num!=0)
                                {
                                    asio::read(io_net_read, asio::buffer(data+len,sizeof(uintptr_t)-end_num));
                                    intptr_t data_p_num = *((intptr_t*)(data+len));
                                    auto p_item = (tcp_main_net_data_type*)data_p_num;
                                    this->doCommand(p_item);
                                }
                            }else
                            {
                                //如果有传过来的对象则内存泄漏
                            }
                            this->readCommand();
                        });
}
void PipeNetUser::sendCommand(tcp_net_main_data_type *item)
{
    intptr_t content = reinterpret_cast<intptr_t>(item);
    ready_items.push_back(content);
    if(write_items.size()==0)
        writeData();
}
void PipeNetUser::popFistItem()
{
//    write_items.pop_front();
}
void PipeNetUser::writeData()
{
    //异步写，每次写一点内容可能影响管道效率。考虑改为同步写。
    //同步写，如果逻辑层不能及时取走数据，会影响网络层效率。
    //最终解决方案：使用两个vector,一个接收，一个发送中
    if(write_items.size()>0)
        return;
    if(ready_items.size()==0)
        return;
    swap(write_items,ready_items);
    asio::async_write(io_net_write, asio::buffer(write_items.data(),write_items.size()*sizeof(intptr_t)),[this](const asio::error_code &ec, std::size_t)
                      {
                          if(!ec)
                          {
//                              this->popFistItem();
                          }
                          write_items.clear();//没有成功也要清除 否则，不能继续写了
                          this->writeData();
                      });
}
