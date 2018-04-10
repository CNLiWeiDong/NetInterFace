//
//  TcpUser.cpp
//  BattleServer
//
//  Created by lwd on 2017/11/21.
//  Copyright © 2017年 lwd. All rights reserved.
//

#include "TcpUser.hpp"
#include "TcpGroup.hpp"
#include "PipeNetUser.hpp"


extern void free_char_data(void* data);
int createUserId()
{
    static int id = 1;
    ++id;
    if(id>1000000)
        id = 1;
    return id;
}

TcpUser::TcpUser(shared_ptr<asio::ip::tcp::socket> _fd,string _ip,int _port):
fd(_fd),
ip(_ip),
port(_port)
{
    userid = createUserId();
}
TcpUser::~TcpUser()
{
    fd->close();
    fd = nullptr;
}
void TcpUser::readData()
{
    auto p_item = new(tcp_net_main_data_type);
    auto &item = *p_item;
    item.command = NET_TCP_DATA_TRANS;
    item.userid = this->getUserid();
    item.ip = getUserIP();
    item.port = getUserPort();
    auto io_fd = fd;
    asio::async_read(*io_fd, asio::buffer(p_item->pb_head,4), [this,io_fd,p_item](const asio::error_code &ec, std::size_t)
                     {
                         if(!ec)
                         {
                             auto &item = *p_item;
                             char *buffer = item.pb_head;
                             unsigned int data_len = 0;//包长度
                             for (int i = 0; i <= 1; i++) { //根据后端要求改反了。应该是大端是高位
                                 data_len <<= 8;
                                 data_len |= (0xff & buffer[i]);
                             }
                             item.len = data_len;
                             unsigned int pb_id = 0;
                             pb_id |= (0xff & buffer[2]);
                             pb_id *=256;
                             pb_id |= (0xff & buffer[3]);
                             item.pb_id = pb_id;
                             char* buf = (char*)malloc(data_len+1);
                             std::shared_ptr<char> buf_ptr(buf,free_char_data);
                             item.msg = buf_ptr;
                             
                             asio::async_read(*io_fd, asio::buffer(buf,data_len), [this,p_item](const asio::error_code &ec, std::size_t)
                                              {
                                                  if(!ec)
                                                  {
                                                      
                                                      PipeNetUser::getInstanse()->sendCommand(p_item);
                                                      // asio::write(*(this->fd), asio::buffer(item.msg,bytes_transferred));
                                                      this->readData();
                                                  }else //if (ec != asio::error::operation_aborted)
                                                  {
                                                      TcpGroup::getInstanse()->removeUser(this->getUserid());//close socket
                                                      delete p_item;
                                                  }
                                              });
                         }else //if (ec != asio::error::operation_aborted)
                         {
                             TcpGroup::getInstanse()->removeUser(this->getUserid());//close socket
                             delete p_item;
                         }
                     });
}
void TcpUser::sendData(shared_ptr<tcp_main_net_data_type> item)
{
    write_items.push_back(item);
    if(write_items.size()>1)
        return;
    writeData();
}
void TcpUser::popFistItem()
{
    write_items.pop_front();
}
void TcpUser::writeData()
{
    if(write_items.size()==0)
        return;
    auto &item = write_items[0];
    asio::async_write(*fd, asio::buffer(item->msg,item->len),[this](const asio::error_code &ec, std::size_t)
                      {
                          if(!ec)
                          {
                              this->popFistItem();
                              this->writeData();
                          }
                          else //if (ec != asio::error::operation_aborted)
                          {
                              TcpGroup::getInstanse()->removeUser(this->getUserid());//close socket
                          }
                      });
}
