//
//  NetInterface.cpp
//  BattleServer
//
//  Created by lwd on 2017/11/22.
//  Copyright © 2017年 lwd. All rights reserved.
//

#include "NetInterface.hpp"

using namespace std;

extern int g_pipe_net_to_main[2];
extern int g_pipe_net_write;
extern int g_pipe_main_read;
extern int g_pipe_main_to_net[2];
extern int g_pipe_main_write;
extern int g_pipe_net_read;

extern void free_char_data(void* data);
extern std::vector<std::string> splitStr(std::string str,std::string decollator);
void * writeThread(void * arg);
pthread_mutex_t m_writeDataMutex;
pthread_mutex_t        m_writeTheradMutex;
pthread_cond_t        c_writeTheradCondition;

static NetInterface * net_interface_instanse = NULL;
NetInterface* NetInterface::getInstanse()
{
    if(net_interface_instanse)
        return net_interface_instanse;
    net_interface_instanse = new NetInterface();
    return net_interface_instanse;
}

NetInterface::NetInterface()
{
    io_main_write = g_pipe_main_write;
    io_main_read = g_pipe_main_read;
    pthread_mutex_init(&m_writeDataMutex, NULL);
    pthread_mutex_init(&m_writeTheradMutex, NULL);
    pthread_cond_init(&c_writeTheradCondition, NULL);
    pthread_t tid;
    pthread_create(&tid, NULL, writeThread, NULL);
    pthread_detach(tid);
}
NetInterface::~NetInterface()
{
    pthread_mutex_destroy(&m_writeDataMutex);
    pthread_mutex_destroy(&m_writeTheradMutex);
    pthread_cond_destroy(&c_writeTheradCondition);
}
void NetInterface::doCommand(tcp_net_main_data_type* p_item)
{
//    if(p_item->len>0)
//    {
//        string t(p_item->msg.get(),p_item->len);
//        printf("main receive userid:%d,command:%d,data:%s \n",p_item->userid,p_item->command,t.c_str());
//    }
    shared_ptr<tcp_net_main_data_type> item(p_item);
    readed_datas.push_back(item);
}
int NetInterface::readCommand()
{
    readed_datas.clear();
    char* data = MAIN_PIPE_COMMAND_READE_BUF;
//    std::shared_ptr<char> data_ptr(data,free_char_data);
    auto read_size = read(io_main_read, data, PIPE_TRANS_SPEED);
    if(read_size<=0)
        return 0;
    intptr_t *intptr_data = (intptr_t*)data;
    auto len = read_size/sizeof(uintptr_t);
    for(int i=0;i<len;i++)
    {
        intptr_t data_p_num = *((intptr_t*)(intptr_data+i));
        auto p_item = (tcp_net_main_data_type*)data_p_num;
        this->doCommand(p_item);
    }
    auto end_num =  read_size%sizeof(uintptr_t);
    if(end_num!=0)
    {
        auto num = 0;
        while (num<end_num) {
            usleep(2000);//sleep 2 haomiao
            auto n = read(io_main_read, data+read_size+num, end_num-num);
            if(n>0)
                num +=n;
        }
        intptr_t data_p_num = *((intptr_t*)(data+len));
        auto p_item = (tcp_net_main_data_type*)data_p_num;
        this->doCommand(p_item);
    }
    return readed_datas.size();
}
void NetInterface::sendCommand(tcp_main_net_data_type *item)
{
    intptr_t content = reinterpret_cast<intptr_t>(item);
    pthread_mutex_lock(&m_writeDataMutex);
    ready_items.push_back(content);
    pthread_mutex_unlock(&m_writeDataMutex);
    pthread_cond_signal(&c_writeTheradCondition);
}
void * writeThread(void * arg)
{
    while (true) {
        NetInterface::getInstanse()->writeData();
    }
    return nullptr;
}
void NetInterface::writeData()
{
    vector<intptr_t> write_items;
    pthread_mutex_lock(&m_writeDataMutex);
    swap(write_items, ready_items);
    pthread_mutex_unlock(&m_writeDataMutex);
    if(write_items.size()>0)
    {
        auto write_size = write(io_main_write, write_items.data(), write_items.size()*sizeof(intptr_t));
        if(write_size<0)
            write_size = 0;
        if(write_size<write_items.size()*sizeof(intptr_t))
        {
            auto remain_num = write_items.size()*sizeof(intptr_t)-write_size;
            auto num = 0;
            while (num<remain_num) {
                usleep(2000);//sleep 2 haomiao
                auto n = write(io_main_write, write_items.data()+write_size+num, remain_num-num);
                if(n>0)
                    num +=n;
            }
        }
    }
    pthread_mutex_lock(&m_writeDataMutex);
    auto new_size = ready_items.size();
    pthread_mutex_unlock(&m_writeDataMutex);
    if(new_size==0)
        pthread_cond_wait(&c_writeTheradCondition, &m_writeTheradMutex);  //实现，线程更高效
}

void NetInterface::connectTcp(const char *ip,int port)
{
    auto p_item = new(tcp_main_net_data_type);
    auto &item = *p_item;
    item.command = NET_TCP_CONNECT;
    item.ips.push_back(ip);
    item.ports.push_back(port);
    sendCommand(p_item);
}
void NetInterface::closeTcp(int id)
{
    auto p_item = new(tcp_main_net_data_type);
    auto &item = *p_item;
    item.command = NET_TCP_CLOSE;
    item.userids.push_back(id);
    item.len = 0;
    sendCommand(p_item);
}
void NetInterface::sendMsg(vector<int> &userids,int data_len,const char* data)
{
    if(userids.size()==0||data_len<=0)
        return;
    auto remain_len = data_len;
    auto send_len = data_len;
    auto idx = 0;
    do
    {
        if(remain_len>DATA_TRANS_SPEED)
        {
            send_len = DATA_TRANS_SPEED;
            remain_len -= DATA_TRANS_SPEED;
        }else
        {
            send_len = remain_len;
            remain_len = 0;
        }
        auto p_item = new(tcp_main_net_data_type);
        auto &item = *p_item;
        item.command = NET_TCP_DATA_TRANS;
        item.userids = userids;
        item.len = send_len;
        item.msg.assign(data+idx,send_len);
        sendCommand(p_item);
        idx+=send_len;
    }while(remain_len>0);
}

void NetInterface::sendMsg(string userids,int data_len,const char* data)
{
    auto users = splitStr(userids, ",");
    vector<int> int_users;
    for(auto &it:users)
    {
        int_users.push_back(stringToNumber(it));
    }
    sendMsg(int_users, data_len, data);
}
void NetInterface::sendUdp(vector<string> &ips,vector<int>&ports,int data_len,const char* data)
{
    auto p_item = new(tcp_main_net_data_type);
    auto &item = *p_item;
    item.command = NET_UDP_DATA_TRANS;
    item.ips = ips;
    item.ports = ports;
    item.len = data_len;
    item.msg.assign(data,data_len);
    sendCommand(p_item);
}
void NetInterface::sendUdp(string ips,string ports,int data_len,const char* data)
{
    auto s_ips = splitStr(ips, ",");
    auto s_ports = splitStr(ports, ",");
    vector<int> i_ports;
    for(auto &it:s_ports)
    {
        i_ports.push_back(stringToNumber(it));
    }
    sendUdp(s_ips,i_ports, data_len, data);
}
