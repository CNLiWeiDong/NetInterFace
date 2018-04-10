//
//  NetSocket.hpp
//  BesiegedCity
//
//  Created by lwd on 2017/11/28.
//  socket 接口方式，使用起来接近socket方式 支持多线程使用

#ifndef NetSocket_hpp
#define NetSocket_hpp

#include <stdio.h>
#include <deque>
#include <vector>
#include <pthread.h>
#include <unordered_map>
#include "NetHead.h"

class NetSocket
{
private:
    int id;
    string ip;
    int port;
    deque<shared_ptr<tcp_net_main_data_type>> receive_datas;
    pthread_mutex_t mutex;
public:
    inline int getId(){return id;}
    NetSocket(int _id)//,string _ip,int _port):id(_id),ip(_ip),port(_port)
    {
        pthread_mutex_init(&mutex, NULL);
    }
    ~NetSocket()
    {
        pthread_mutex_destroy(&mutex);
    }
    void append(shared_ptr<tcp_net_main_data_type> item);
    deque<shared_ptr<tcp_net_main_data_type>> receive();
    shared_ptr<tcp_net_main_data_type> receive_top();
};
class NetSocketGrounp
{
private:
    unordered_map<int, shared_ptr<NetSocket>> sockets;
    vector<shared_ptr<tcp_net_main_data_type>> accept_sockets;
    vector<shared_ptr<tcp_net_main_data_type>> connect_sockets;
    pthread_mutex_t send_mutex;
    pthread_mutex_t sockets_mutex;
    pthread_mutex_t accept_sockets_mutex;
    pthread_mutex_t connect_sockets_mutex;
public:
    static NetSocketGrounp* getInstanse();
    NetSocketGrounp();
    ~NetSocketGrounp();
    void readData();
    shared_ptr<NetSocket> findSocket(int id);
protected:
    void insertAccecpt(shared_ptr<tcp_net_main_data_type> item);
    void insertConnect(shared_ptr<tcp_net_main_data_type> item);
    void insertSockets(int socketid,shared_ptr<NetSocket>);
    
};
#endif /* NetSocket_hpp */
