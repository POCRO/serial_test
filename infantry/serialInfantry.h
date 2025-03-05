#ifndef RM_SERIALINFANTRY_H
#define RM_SERIALINFANTRY_H

#include <boost/thread/mutex.hpp>
#include <string>

#include "serialBase.h"
#include "serialFactory.h"
#include "verify.h"


#define SEND_BUFF_SIZE 26
#define RECEIVE_FLOAT_NUM 7
#define RECEIVE_BUFF_SIZE 5+4*RECEIVE_FLOAT_NUM
namespace hitcrt {

class SerialInfantry : public SerialCom {
   public:
    union Float2uchar {
        float fl;
        unsigned char ch[4];
    };
    enum SEND_FLAG { SEND_MASTER_GIMBAL_DATA, SEND_SERVANT_GIMBAL_DATA };

    enum RECEIVE_FLAG {
        RECEIVE_HEART_BEAT,
        RECEIVE_ASSIST_AIM,  /// 辅助瞄准 1开始小装甲 2开始大装甲 0结束
        RECEIVE_ANGLE,
        RECEIVE_ASSIST_RED,
        RECEIVE_ASSIST_BLUE,
    };

    SerialInfantry(const std::string& str, int baudrate);
    ~SerialInfantry();
    void send(std::vector<boost::any> vData) override;
    void receive(std::vector<boost::any>& vData) override;

   private:
    struct timeval timer;
    double m_firsttime;
    unsigned char FIRST_ONE;
    unsigned char FIRST_TWO;
    unsigned char MAX_RECEIVE_FLOAT_LENGTH;

    bool decode(unsigned char* buff, size_t& received_length, std::vector<float>& num,
                SerialInfantry::RECEIVE_FLAG& receive_flag, unsigned char& RECEIVED_ID,
                unsigned char& which_gyro);
    bool decode_num(unsigned char buff, float& num);
    //    void send(unsigned char send_distance, std::vector<float> num = std::vector<float>());
    void send(unsigned char send_distance, std::vector<float> num);
    void receive(SerialInfantry::RECEIVE_FLAG& receive_flag, std::vector<float>& num,
                 unsigned char& received_id, unsigned char& which_gyro);
    //    void Send_Data_To_DataScope(std::vector<float> _data);

    /// 线程锁，很重要！！！！！！
    /// 避免多个线程同时发送数据
    boost::mutex m_send_mutex;

    unsigned char tempsavebuff[RECEIVE_BUFF_SIZE] = {0xff};  /// 解码过程中所存部分
};

}  // namespace hitcrt

#endif  // RM_SERIALINFANTRY_H
