//
// Created by bg2edg on 2020/11/24.
//

#include "serialFactory.h"
#include "serialInfantry.h"

namespace hitcrt
{

    std::string SerialCom::findDevice(){
        DIR* pDir;
        struct dirent* ptr;
        std::vector<std::string> files;
        if(!(pDir=opendir("/dev/"))){
            return "faild";
        }
        const std::string path0="/dev/";
        std::string subFile;
        while ((ptr=readdir(pDir))!=0){
            subFile=ptr->d_name;
            if(subFile.substr(0,subFile.size()-1)=="ttyUSB"){
                files.emplace_back(path0+subFile);
            }
        }
        if(!files.empty()){
            std::cout<<"find Serial:"<<std::endl;
        } else{
            std::cerr<<"Error:no Serial find!"<<std::endl;
            return "faild";
        }
        for(auto device:files){
            std::cout<<device<<std::endl;
        }
        return files[0];
    }
///str为"auto"时自动查找并打开第一个找到的设备
    SerialCom::SerialCom(const std::string& str, int baudRate)
    {
        if(str!="auto"){
            m_serialBase = new SerialBase(str, baudRate);///串口基层，负责收发具体数据
        } else{
            std::string device=findDevice();
            if(device=="failed!"){
                exit(1);
            }
            m_serialBase = new SerialBase(device, baudRate);///串口基层，负责收发具体数据
        }
    }

    SerialCom::~SerialCom()
    {
        delete m_serialBase;
    }

    void SerialCom::error()
    {
        std::cerr << "wrong arguments for SerialFactory::createSerial" << std::endl;
        throw(0);
    }



    /*
     * Copyright(C) 2021,HITCRT_VISION,all rights reserved
     * @brief 产生串口类的实例
     * @param int ROLE_NUM: 兵种编号，详见2021RM规则手册
     * @return 串口类的实例
     * @author
     *  -bg2edg
     */
    SerialCom * SerialFactory::createSerial(int ROLE_NUM, const std::string& str, int baudRate)
    {
        SerialCom * Serial;
        switch (ROLE_NUM)
        {
            case 1:
                //Serial = new SerialHero(str,baudRate);
                break;
            case 3:
                Serial = new SerialInfantry(str,baudRate);
                break;
            case 4:
                Serial = new SerialInfantry(str,baudRate);
                break;
            case 5:
                Serial = new SerialInfantry(str,baudRate);
                break;
        }

        return Serial;
    }
}