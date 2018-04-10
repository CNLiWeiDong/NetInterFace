//
//  UdpUser.cpp
//  BesiegedCity
//
//  Created by lwd on 2017/12/2.
//

#include "UdpUser.hpp"
#include "PipeNetUser.hpp"

extern asio::io_service g_io_service;
extern void free_char_data(void* data);
extern int local_ai_family;

static UdpUser * udp_user_instanse = NULL;
UdpUser* UdpUser::getInstanse()
{
    if(udp_user_instanse)
        return udp_user_instanse;
    udp_user_instanse = new UdpUser();
    return udp_user_instanse;
}
#ifdef NET_LIB_FOR_SERVER
UdpUser::UdpUser()
{
    fd = make_shared<asio::ip::udp::socket>(g_io_service,asio::ip::udp::endpoint(asio::ip::udp::v4(), UDP_SERVER_PORT));
    readData();
}
#else
UdpUser::UdpUser()
{
    auto a = gethostbyname("localhost");
    auto l_ai_family = a->h_addrtype;
    if(l_ai_family==ASIO_OS_DEF(AF_INET))
        fd = make_shared<asio::ip::udp::socket>(g_io_service,asio::ip::udp::endpoint(asio::ip::udp::v4(), 0));
    else
        fd = make_shared<asio::ip::udp::socket>(g_io_service,asio::ip::udp::endpoint(asio::ip::udp::v6(), 0));
    readData();
}
#endif

UdpUser::~UdpUser()
{
    fd->close();
    fd = nullptr;
}
void UdpUser::readData()
{
    fd->async_receive_from(asio::buffer(UDT_RECEIVE_DATA,UDT_RECEIVE_DATA_LEN),sender_endpoint,[this](const asio::error_code &ec, std::size_t size)
                              {
                                  if(size>4)
                                      if(!ec)
                                      {
                                          auto p_item = new(tcp_net_main_data_type);
                                          auto &item = *p_item;
                                          item.command = NET_UDP_DATA_TRANS;
                                          item.userid = 0;
                                          item.ip = this->getSenderPoint().address().to_string();
                                          item.port = this->getSenderPoint().port();
                                          char *buffer = UDT_RECEIVE_DATA;
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
                                          memcpy(buf,UDT_RECEIVE_DATA+4,item.len);
                                          std::shared_ptr<char> buf_ptr(buf,free_char_data);
                                          item.msg = buf_ptr;
                                          
                                          PipeNetUser::getInstanse()->sendCommand(p_item);
                                      }
                                  readData();
                                  
                              });
}
void UdpUser::sendData(shared_ptr<tcp_main_net_data_type> item)
{
    for(int i=0;i<item->ips.size();i++)
    {
        udp_cur_write_data_type write_item;
        write_item.item = item;
        write_item.ip = item->ips[i];
        write_item.port = item->ports[i];
        write_items.push_back(write_item);
    }
    if(write_items.size()>1)
        return;
    writeData();
}
void UdpUser::popFistItem()
{
    write_items.pop_front();
}
void UdpUser::writeData()
{
    if(write_items.size()==0)
        return;
    auto &item = write_items[0];
    asio::ip::address address = asio::ip::address::from_string(item.ip);
    asio::ip::udp::endpoint endpoint(address,item.port);
    auto len = item.item->len;
    fd->async_send_to(asio::buffer(item.item->msg,len),endpoint,[this,len](const asio::error_code &ec, std::size_t size)
                      {
                          if(size>=len)
                              this->popFistItem();
                          this->writeData();
                      });
    
}

