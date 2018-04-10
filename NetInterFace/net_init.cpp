#include <stdio.h>
#include <string>
#include <vector>
#include <errno.h>
#include <unistd.h>
#include "NetThread.hpp"
#include <asio.hpp>
#include "NetInterface.hpp"

int g_pipe_net_to_main[2];
int g_pipe_net_write;
int g_pipe_main_read;
int g_pipe_main_to_net[2];
int g_pipe_main_write;
int g_pipe_net_read;
int local_ai_family;
void free_char_data(void* data)
{
    free(data);
}
//字符串拆分，str：字符串，decollator：拆分字符
std::vector<std::string> splitStr(std::string str,std::string decollator)
{
    std::string::size_type pos;
    std::vector<std::string> result;
    str += decollator;
    int size = str.size();
    
    for(int i = 0; i<size; i++)
    {
        pos = str.find(decollator,i);
        if(pos < size)
        {
            std::string s = str.substr(i,pos-i);
            result.push_back(s);
            i = pos + decollator.size() - 1;
        }else
        {
            std::string s=str.substr(i,size);
            if (s.length()>0)
            {
                result.push_back(s);
            }
        }
    }
    return result;
}
string numberToString(int num)
{
    stringstream ss;
    ss<<num;   //像流中传值
    string result;
    ss>>result;  //将流中的值写入到result
    return result;
}
//字符创类型转换为整型
int stringToNumber(string strNum)
{
    stringstream ss;
    ss<<strNum;
    int result;
    ss>>result;
    return result;
}
void initNetInterFace()
{
    if( ::pipe(g_pipe_net_to_main) != 0 )
        return;
    if( ::pipe(g_pipe_main_to_net) != 0 )
        return;
    ::fcntl(g_pipe_net_to_main[0], F_SETFL, O_NONBLOCK);
    ::fcntl(g_pipe_net_to_main[0], F_SETFD, FD_CLOEXEC);
    ::fcntl(g_pipe_net_to_main[1], F_SETFL, O_NONBLOCK);
    ::fcntl(g_pipe_net_to_main[1], F_SETFD, FD_CLOEXEC);
    ::fcntl(g_pipe_main_to_net[0], F_SETFL, O_NONBLOCK);
    ::fcntl(g_pipe_main_to_net[0], F_SETFD, FD_CLOEXEC);
    ::fcntl(g_pipe_main_to_net[1], F_SETFL, O_NONBLOCK);
    ::fcntl(g_pipe_main_to_net[1], F_SETFD, FD_CLOEXEC);
    g_pipe_net_write = g_pipe_net_to_main[1];//1必须用于写
    g_pipe_main_read = g_pipe_net_to_main[0];//0用于读
    g_pipe_main_write = g_pipe_main_to_net[1];
    g_pipe_net_read = g_pipe_main_to_net[0];
    
    auto a = gethostbyname("localhost");
    local_ai_family = a->h_addrtype;
    createNetThread();
}

