#ifndef MOTORCAN_H
#define MOTORCAN_H
#include "driver/twai.h"
#include "esp_err.h"
typedef struct 
{
    /* data */
    float Vol;
    int8_t Temp;
    uint8_t Error;
} Data1;
typedef struct 
{
    /* data */
    int8_t Temp;
    int16_t TorCurrent;
    int16_t Speed;
    uint16_t Position;
} Data2;
typedef struct 
{
    /* data */
    int8_t Temp;
    int16_t A_PhaseCur;
    int16_t B_PhaseCur;
    int16_t C_PhaseCur;
} Data3;
typedef struct 
{
    /* data */
    esp_err_t err;
    uint8_t Cmd;
    Data1 data1;
    Data2 data2;
    Data3 data3;
} DataClaim;
esp_err_t send_off_motor(uint32_t id);
esp_err_t send_on_motor(uint32_t id);
esp_err_t send_stop_motor(uint32_t id);
esp_err_t send_torque_control(uint32_t id, int16_t Torque);
esp_err_t send_single_loop_angle_control(uint32_t id, uint8_t Direction , uint16_t MaxSpeed, uint32_t Angle);
esp_err_t send_increment_angle_control(uint32_t id, uint16_t MaxSpeed, uint32_t Angle);
esp_err_t send_Clear_error(uint32_t id);
esp_err_t command_read_state_1(uint32_t id);
esp_err_t command_read_state_2(uint32_t id);
esp_err_t command_read_state_3(uint32_t id);
DataClaim receive_data();
#endif