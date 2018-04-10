//
//  NetSocket.cpp
//  BesiegedCity
//
//  Created by lwd on 2017/11/28.
//

#include "NetSocket.hpp"
#include <unistd.h>
#include "NetInterface.hpp"

void NetSocket::append(shared_ptr<tcp_net_main_data_type> item)
{
    pthread_mutex_lock(&mutex);
    receive_datas.push_back(item);
    pthread_mutex_unlock(&mutex);
}
deque<shared_ptr<tcp_net_main_data_type>> NetSocket::receive()
{
    deque<shared_ptr<tcp_net_main_data_type>> ret;
    pthread_mutex_lock(&mutex);
    receive_datas.swap(ret);
    pthread_mutex_unlock(&mutex);
    return ret;
}
shared_ptr<tcp_net_main_data_type> NetSocket::receive_top()
{
    shared_ptr<tcp_net_main_data_type> ret = nullptr;
    pthread_mutex_lock(&mutex);
    if(receive_datas.size()>0)
    {
        ret = *(receive_datas.begin());
        receive_datas.pop_front();
    }
    pthread_mutex_unlock(&mutex);
    return ret;
}


void * readThread(void * arg);
static NetSocketGrounp * net_socketgrounp_instanse = NULL;
NetSocketGrounp* NetSocketGrounp::getInstanse()
{
    if(net_socketgrounp_instanse)
        return net_socketgrounp_instanse;
    net_socketgrounp_instanse = new NetSocketGrounp();
    return net_socketgrounp_instanse;
}
NetSocketGrounp::NetSocketGrounp()
{
    pthread_mutex_init(&send_mutex, NULL);
    pthread_mutex_init(&sockets_mutex, NULL);
    pthread_mutex_init(&accept_sockets_mutex, NULL);
    pthread_mutex_init(&connect_sockets_mutex, NULL);
    pthread_t tid;
    pthread_create(&tid, NULL, readThread, NULL);
    pthread_detach(tid);
}
NetSocketGrounp::~NetSocketGrounp()
{
    pthread_mutex_destroy(&send_mutex);
    pthread_mutex_destroy(&sockets_mutex);
    pthread_mutex_destroy(&accept_sockets_mutex);
    pthread_mutex_destroy(&connect_sockets_mutex);
}
void * readThread(void * arg)
{
    while (true) {
        NetSocketGrounp::getInstanse()->readData();
        usleep(10000);//sleep 10 haomiao
    }
    return nullptr;
}
shared_ptr<NetSocket> NetSocketGrounp::findSocket(int id)
{
    shared_ptr<NetSocket> ret = nullptr;
    pthread_mutex_lock(&sockets_mutex);
    auto it = sockets.find(id);
    if(it!=sockets.end())
        ret = it->second;
    pthread_mutex_unlock(&sockets_mutex);
    return ret;
}
void NetSocketGrounp::insertAccecpt(shared_ptr<tcp_net_main_data_type> item)
{
    
}
void NetSocketGrounp::insertConnect(shared_ptr<tcp_net_main_data_type> item)
{
    
}
void NetSocketGrounp::insertSockets(int socketid,shared_ptr<NetSocket>)
{
    
}
void NetSocketGrounp::readData()
{
    auto interface = NetInterface::getInstanse();
    interface->readCommand();
    auto &data = interface->getReadedDatas();
    for(auto &it:data)
    {
        if(it->command==NET_TCP_ACCEPT)
        {
            shared_ptr<NetSocket> socket = make_shared<NetSocket>(it->userid);
            insertSockets(it->userid, socket);
            insertAccecpt(it);
        }
        else if(it->command==NET_TCP_CONNECT)
        {
            shared_ptr<NetSocket> socket = make_shared<NetSocket>(it->userid);
            insertSockets(it->userid, socket);
            insertConnect(it);
        }else if (it->command==NET_TCP_DATA_TRANS)
        {
            auto socket = findSocket(it->userid);
            if(socket)
            {
                socket->append(it);
            }
        }
    }
}
