//
// Created by bg2edg on 2020/11/24.
//

#ifndef RM_SERIALFACTORY_H
#define RM_SERIALFACTORY_H
#include <iostream>
#include <string>
#include <boost/thread/mutex.hpp>
#include <boost/any.hpp>
#include <dirent.h>
#include <exception>
#include "serialBase.h"

namespace hitcrt
{

    class SerialCom{
    public:
        SerialCom(const std::string& str, int baudRate);
        ~SerialCom();
        virtual void send(std::vector<boost::any>)=0;
        virtual void receive(std::vector<boost::any> &)=0;

    protected:
        SerialBase* m_serialBase;
        std::string findDevice();
        void error();

    };

    union Float2uchar
    {
        float fl;
        unsigned char ch[4];
    };

    class SerialFactory {
    public:
        static SerialCom * createSerial(int ROLE_NUM, const std::string& str, int baudRate);
    };

    class SerialData
    {
    public:

    };
}
#endif //RM_SERIALFACTORY_H
