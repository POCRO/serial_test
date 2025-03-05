/********************************************************

串口协议为 55 11 ID NUM NUM*float 22 AA

比如 功能ID为 03
发送数据为5个float 则协议为
55 11 03 05 xx xx xx xx xx 22 AA

该程序应用了boost asio 库

*********************************************************/

#include "serialInfantry.h"

#include <dirent.h>

#include <exception>

#include "main.h"

namespace hitcrt {

SerialInfantry::SerialInfantry(const std::string& str, int baudrate) : SerialCom(str, baudrate) {
    MAX_RECEIVE_FLOAT_LENGTH = 20;
    /// 步兵
    FIRST_ONE = 0x55;
    FIRST_TWO = 0x11;

    gettimeofday(&timer, NULL);
    m_firsttime = timer.tv_sec * 1e3 + timer.tv_usec * 1e-3;
}

SerialInfantry::~SerialInfantry() = default;

void SerialInfantry::send(std::vector<boost::any> vData) {
    // unsigned char g_send_distance,30
    send(boost::any_cast<unsigned char>(vData[0]), boost::any_cast<std::vector<float>>(vData[1]));
}

void SerialInfantry::receive(std::vector<boost::any>& vData) {
    SerialInfantry::RECEIVE_FLAG _receive_flag;
    std::vector<float> _num;
    unsigned char _received_id;
    unsigned char _which_gyro;
    receive(_receive_flag, _num, _received_id, _which_gyro);
    vData.emplace_back(_receive_flag);
    vData.emplace_back(_num);
    vData.emplace_back(_received_id);
    vData.emplace_back(_which_gyro);
}

void SerialInfantry::send(unsigned char send_distance, std::vector<float> num) {

    int send_length = num.size() * 4 + 5; // 这里后面是数字是帧头长度+非数据本身长度+CRC帧尾的长度
    assert(send_length < 100);
    unsigned char buff[100] = {};

    buff[0] = FIRST_ONE;
    buff[1] = FIRST_TWO;
    buff[2] = static_cast<unsigned char>(num.size());

    for (int i = 0; i < num.size(); i++) {
        Float2uchar temp;
        temp.fl = num[i];
        for (int j = 0; j < 4; j++) buff[i * 4 + 3 + j] = temp.ch[j];
    }
    Append_CRC16_Check_Sum(buff, send_length);  

    m_serialBase->send(buff, send_length);

    // print send buff
            //         std::cout << "send buff contents: ";

            // for (int j = 0; j < send_length; j++) {
            //         std::cout << std::hex << static_cast<int>(buff[j]) << " ";
            //     }
            //     std::cout << std::endl;
}


void SerialInfantry::receive(SerialInfantry::RECEIVE_FLAG& receive_flag, std::vector<float>& num,
                             unsigned char& RECEIVED_ID, unsigned char& which_gyro) {
    while (1) {
        unsigned char buff[100] = {0xff};
        size_t received_length;
        m_serialBase->receive(buff, received_length);

        // std::cout << "raw_data----------------------" << std::endl ;
        // std::cout << "received_length = " << received_length << std::endl ;
        // for(int i=0;i<100;i++)
        // {
        //     std::cout << std::hex << (unsigned int)buff[i] << " " ;
        // }
        // std::cout << std::endl;
        
        if (decode(buff, received_length, num, receive_flag, RECEIVED_ID, which_gyro)||(programIsRunning == 0)) 
        {
            // std::cout << "receive_flag = " << receive_flag << std::endl;
            break;
        }

    }
}

bool SerialInfantry::decode(unsigned char* buff, size_t& received_length, std::vector<float>& num,
                            SerialInfantry::RECEIVE_FLAG& receive_flag, unsigned char& RECEIVED_ID,
                            unsigned char& which_gyro) {
    static int flag = 1;                    /// 接收状态机
    static RECEIVE_FLAG temp_receive_flag;  /// 接收标志量
    static std::vector<float> vec_num;      /// 接收数据
    static int temp_num_length;
    static bool return_flag;
    static unsigned char temp_received_ID = 0x00;
    return_flag = false;
    // std::cout << "flag=" << flag << std::endl ;
    for (size_t i = 0; i < received_length; i++) {
        switch (flag) {
            case 1:  /// 报头1
                            //    std::cout << "1" << std::endl;

                vec_num.clear();
                if (FIRST_ONE == buff[i]) {
                    flag = 2;
                    tempsavebuff[0] = buff[i];
                } else
                    flag = 1;
                break;
            case 2:  /// 报头2
                            //    std::cout << "2" << std::endl;
                if (FIRST_TWO == buff[i]) {
                    flag = 3;
                    tempsavebuff[1] = buff[i];
                } else {
                    if (buff[i] == 0x55) {
                        flag = 2;
                    } else {
                        flag = 1;
                    }
                }
                break;
            case 3:  /// 浮点数长度
                // std::cout << "temp_len=" << temp_num_length<< std::endl;
                temp_num_length = static_cast<int>(buff[i]);
                if (temp_num_length == RECEIVE_FLOAT_NUM) {
                    flag = 4;
                    tempsavebuff[2] = buff[i];
                }
                break;
            case 4:
                float temp;
                if (decode_num(buff[i], temp)) vec_num.push_back(temp);
                
            //    std::cout << " temp ="<<temp << std::endl;

                if (vec_num.size() == temp_num_length) {
                    flag = 5;
                    for (int j = 0; j < vec_num.size(); j++) {
                        Float2uchar temp_one_num;
                        temp_one_num.fl = vec_num[j];
                        for (int k = 0; k < 4; k++) {
                            tempsavebuff[j * 4 + k + 3] = temp_one_num.ch[k];
                        }
                    }
                }
                break;
            case 5:
                tempsavebuff[RECEIVE_BUFF_SIZE - 2] = buff[i];
                flag = 6;
                break;
            case 6:
                tempsavebuff[RECEIVE_BUFF_SIZE - 1] = buff[i];

                // std::cout << "tempsavebuff contents: ";
                // for (int j = 0; j < RECEIVE_BUFF_SIZE; j++) {
                //     std::cout << std::hex << static_cast<int>(tempsavebuff[j]) << " ";
                // }
                // std::cout << std::endl;

                if (Verify_CRC16_Check_Sum(tempsavebuff, RECEIVE_BUFF_SIZE)) {
                    // std::cout << "CRC_OK  "<<std::endl;
                    receive_flag = temp_receive_flag;
                    num = vec_num;
                    RECEIVED_ID = temp_received_ID;
                    return_flag = true;
                }
                flag = 1;
                break;
        }
    }
    //std::cout << "return_flag = "<< return_flag<<std::endl;
    return return_flag;
}

bool SerialInfantry::decode_num(unsigned char buff, float& num) {
    static Float2uchar temp_num;
    static int pos = 0;
    temp_num.ch[pos] = buff;
    pos++;
    if (pos == 4) {
        num = temp_num.fl;
        pos = 0;
        return true;
    }
    return false;
}

}  // namespace hitcrt
