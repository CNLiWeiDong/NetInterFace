//
//  TcpGroup.cpp
//  BattleServer
//
//  Created by lwd on 2017/11/21.
//  Copyright © 2017年 lwd. All rights reserved.
//

#include "TcpGroup.hpp"
#include "PipeNetUser.hpp"
#include "NetHead.h"

using namespace std;

extern asio::io_service g_io_service;
extern void free_char_data(void* data);
extern int local_ai_family;

static TcpGroup * tcp_group_instanse = NULL;
TcpGroup* TcpGroup::getInstanse()
{
    if(tcp_group_instanse)
        return tcp_group_instanse;
    tcp_group_instanse = new TcpGroup();
    return tcp_group_instanse;
}
#ifdef NET_LIB_FOR_SERVER
TcpGroup::TcpGroup():
server_endpoint(asio::ip::tcp::v4(), TCP_SERVER_PORT),
server_acceptor(g_io_service, server_endpoint)
{
    server_acceptor.listen();
    accept();
}
#else
TcpGroup::TcpGroup()
{
    
}
#endif
void TcpGroup::addUser(int userid,shared_ptr<TcpUser> user,NET_COMMAND_TYPE user_type)
{
    users.insert(decltype(users)::value_type(userid,user));
    //notic main thread connect success
    auto p_item = new(tcp_net_main_data_type);
    auto &item = *p_item;
    item.command = user_type;
    item.userid = userid;
    item.ip = user->getUserIP();
    item.port = user->getUserPort();
    item.pb_id = 0;
    item.len = 0;
//    string addr(user->getUserIP());
//    addr.push_back(',');
//    addr.append(numberToString(user->getUserPort()));
//    item.len = addr.size();
//
//    char* buf = (char*)malloc(item.len+1);
//    std::shared_ptr<char> buf_ptr(buf,free_char_data);
//    item.msg = buf_ptr;
//
//    memcpy(buf,addr.data(),item.len);
    PipeNetUser::getInstanse()->sendCommand(p_item);
    user->readData();// begin read
}
void TcpGroup::removeUser(int userid)
{
    auto it = users.find(userid);
    if(it!=users.end())
    {
        //notic main thread close success
        auto &user = it->second;
        auto p_item = new(tcp_net_main_data_type);
        auto &item = *p_item;
        item.command = NET_TCP_CLOSE;
        item.ip = user->getUserIP();
        item.port = user->getUserPort();
        item.userid = userid;
        item.pb_id = 0;
        item.len = 0;
        
//        string addr(user->getUserIP());
//        addr.push_back(',');
//        addr.append(numberToString(user->getUserPort()));
//        item.len = addr.size();
//        char* buf = (char*)malloc(item.len+1);
//        std::shared_ptr<char> buf_ptr(buf,free_char_data);
//        item.msg = buf_ptr;
//
//        memcpy(buf,addr.data(),item.len);
        PipeNetUser::getInstanse()->sendCommand(p_item);
        users.erase(it);
    }
}
shared_ptr<TcpUser> TcpGroup::findUser(int userid)
{
    auto it = users.find(userid);
    if(it==users.end())
        return NULL;
    return it->second;
}
void TcpGroup::connect(string &ip,int port)
{
    for(auto &it:users)
    {
        if(it.second->getUserIP()==ip&&it.second->getUserPort()==port)
            return;
    }
    string adr_ip(ip);
    asio::ip::tcp::endpoint endpoint;
    if(adr_ip.find(".com")!=string::npos || adr_ip.find(".net")!=string::npos || adr_ip.find(".cn")!=string::npos )
    {
        asio::ip::tcp::resolver resolver(g_io_service);
        asio::ip::tcp::resolver::query query(ip,numberToString(port));
        asio::error_code ec;
        auto it = resolver.resolve(query, ec);//ASIO_DISABLE_THREADS 关闭了线程，内部无法用线程实现异步 这里同步解析dns
        if(ec)
        {
            printf("querydns can't resolve %s",adr_ip.c_str());
            return;
        }
        endpoint = *it;
    }
    else
    {
        asio::ip::address addr;
        addr = addr.from_string(ip);
        asio::ip::tcp::endpoint n_endpoint(addr,port);
        endpoint = n_endpoint;
    }
    shared_ptr<asio::ip::tcp::socket> fd;
    auto a = gethostbyname("localhost");
    auto l_ai_family = a->h_addrtype;
    if(l_ai_family==ASIO_OS_DEF(AF_INET))
        fd = make_shared<asio::ip::tcp::socket>(g_io_service,asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 0));
    else
        fd = make_shared<asio::ip::tcp::socket>(g_io_service,asio::ip::tcp::endpoint(asio::ip::tcp::v6(), 0));
//    shared_ptr<asio::ip::tcp::socket> fd(new asio::ip::tcp::socket(g_io_service));
    fd->async_connect(endpoint,
                      [this,fd,adr_ip,port](std::error_code ec)
                      {
//                          auto point = fd->local_endpoint();
//                          auto a = point.address().to_string();
//                          auto b= a;
                          if (!ec)
                          {
                              shared_ptr<TcpUser> user = make_shared<TcpUser>(fd,adr_ip,port);
                              this->addUser(user->getUserid(),user,NET_TCP_CONNECT);
                          }else
                              fd->close();
                      });
    
}
void TcpGroup::accept()
{
#ifdef NET_LIB_FOR_SERVER
    shared_ptr<asio::ip::tcp::socket> fd(new asio::ip::tcp::socket(g_io_service));
    server_acceptor.async_accept(*fd, [this,fd](std::error_code ec)
                                 {
                                     if (!ec)
                                     {
                                         auto endpoint = fd->remote_endpoint();
                                         shared_ptr<TcpUser> user = make_shared<TcpUser>(fd,endpoint.address().to_string(),endpoint.port());
                                         this->addUser(user->getUserid(),user,NET_TCP_ACCEPT);
                                     }else
                                     {
                                         fd->close();
                                     }
                                     accept();
                                 });
#endif
}
