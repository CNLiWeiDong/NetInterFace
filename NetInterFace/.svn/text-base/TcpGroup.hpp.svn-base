//
//  TcpGroup.hpp
//  BattleServer
//
//  Created by lwd on 2017/11/21.
//  Copyright © 2017年 lwd. All rights reserved.
//

#ifndef TcpGroup_hpp
#define TcpGroup_hpp

#include <stdio.h>
#include <map>
#include <unordered_map>
#include "TcpUser.hpp"

using namespace std;
class TcpGroup
{
private:
    unordered_map<int,shared_ptr<TcpUser> > users;//int buf_id is bufer's socket fd
#ifdef NET_LIB_FOR_SERVER
    asio::ip::tcp::endpoint server_endpoint;
    asio::ip::tcp::acceptor server_acceptor;
#endif
public:
    static TcpGroup* getInstanse();
    TcpGroup();
    void addUser(int userid,shared_ptr<TcpUser> user,NET_COMMAND_TYPE user_type);
    void removeUser(int userid);
    shared_ptr<TcpUser> findUser(int userid);
    void connect(string &ip,int port);
    void beginAcceptConnect();
protected:
    void accept();
};
#endif /* TcpGroup_hpp */
