//
// Created by bg2edg on 2020/11/10.
//

#ifndef RM_HERO_SERIALBASE_H
#define RM_HERO_SERIALBASE_H

#include <iostream>
#include <boost/asio.hpp>
namespace hitcrt
{
    class SerialBase
    {
    public:
        //输入：端口号，波特率
        SerialBase(std::string str,int baud_rate);
        ~SerialBase();
        void send(unsigned char* ch, size_t length);
        void receive(unsigned char* buff, size_t& length);
    private:
        struct timeval timer;
        double firstTime;
        boost::asio::io_service* io;
        boost::asio::serial_port* port;
        static const int MAX_BUFFER_LENGTH = 100;
    };
}
#endif //RM_HERO_SERIALBASE_H
