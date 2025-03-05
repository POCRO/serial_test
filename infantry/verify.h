//
// Created by ljy on 19-7-8.
//

#ifndef INFANTRY_LJY_V1_VERIFY_H
#define INFANTRY_LJY_V1_VERIFY_H
#include <iostream>

#define u8 unsigned char
#define u16 uint16_t
#define u32 uint32_t

u32 Verify_CRC16_Check_Sum(u8 *pchMessage, u32 dwLength);
void Append_CRC16_Check_Sum(u8 * pchMessage,u32 dwLength);

#endif //INFANTRY_LJY_V1_VERIFY_H
