#include <atomic>
#include <thread>
#include <iostream>
#include <vector>
#include <boost/thread.hpp>
#include <mutex>
#include <signal.h>
#include "infantry/serialInfantry.h"
#include "infantry/serialFactory.h"

// 原子变量
std::atomic<int> programIsRunning = 1; 
static float temp_PitchDegree;
using namespace hitcrt;
// send_data线程
void send_data(std::shared_ptr<SerialCom>& serial)
{
    int aCnt = 1;
    float x = 2.534;
    while (programIsRunning)
    {
        if (aCnt >= 1000)
        {
            std::cout << "send_data thread running" << std::endl;
            aCnt = 1;
        }
        aCnt++;
        //打包数据和发送，应该在回调函数中进行
        std::vector<boost::any> send_data;      
        std::vector<float> num; //传输的数据(float)
        num.emplace_back(x);   //传输几个就emplace_back几个，这里只演示单个
        send_data.emplace_back((unsigned char)1); //暂时没用上，意义为传输长度
        send_data.emplace_back(num);   
        serial->send(send_data);  //发送数据包       
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::cout << "send_data thread exiting" << std::endl;
}

// receive_data线程
void receive_data(std::shared_ptr<SerialCom>& serial)
{
    int bCnt = 1;
    while (programIsRunning)
    {
        if (bCnt >= 1000)
        {
            std::cout << "receive_data thread running" << std::endl;
            bCnt = 1;
        }
        bCnt++;
        //接收和读取数据，另开线程
        std::vector<boost::any> receive_data;
        //std::cout << "ready to rec" << std::endl;
        serial->receive(receive_data);
        auto data = boost::any_cast<std::vector<float>>(receive_data.at(1));

        ///前二位为角度信息       
        temp_PitchDegree = data[0];
        std::cout << "received temp_PitchDegree=" << temp_PitchDegree << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::cout << "rec_data thread exiting" << std::endl;
}

void killThisProgram(int sigNum)
{
    std::cout<<"按下了CTRL-C，准备结束程序"<<std::endl;
    programIsRunning = 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

int main(void)
{
    signal(SIGINT, killThisProgram); //设置监听ctrl+C信号，当收到这个信号执行killThisProgram()
    //创建串口，只需创建一次！！！！  
    auto serial = std::shared_ptr<SerialCom>(SerialFactory::createSerial(3, "auto", 460800));       
    boost::thread aThread = boost::thread(&send_data, std::ref(serial));
    boost::thread bThread = boost::thread(&receive_data, std::ref(serial));
    while (programIsRunning)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // 等待线程退出，确保资源释放
    std::cout << "out while" << std::endl;
    aThread.join();
    bThread.join();
    std::cout << "resouces released" << std::endl;
    return 0;
}
