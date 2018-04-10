//
//  NetThread.cpp
//  BattleServer
//
//  Created by lwd on 2017/11/3.
//  Copyright © 2017年 lwd. All rights reserved.
//
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "NetThread.hpp"
#include <string>
#include<errno.h>
#include <unistd.h>
#include "pthread.h"
#include <asio.hpp>
#include "PipeNetUser.hpp"
#include "TcpGroup.hpp"


asio::io_service g_io_service(0);

void * net_thread(void * arg)
{
    asio::io_service::work work(g_io_service);
    auto pipe_net_user = PipeNetUser::getInstanse();
    pipe_net_user->readCommand();
    TcpGroup::getInstanse();
    g_io_service.run();
    return nullptr;
}
void createNetThread()
{
    pthread_t tid;
    pthread_create(&tid, NULL, net_thread, NULL);
    pthread_detach(tid);
}
