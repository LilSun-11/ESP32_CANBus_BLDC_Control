//#include "MotorCAN.h"
#include "driver/twai.h"
typedef struct 
{
    /* data */
    uint16_t Vol;
    int8_t Temp;
    uint8_t Error;
} Data1;
typedef struct 
{
    /* data */
    int8_t Temp;
    int16_t TorCurrent;
    int16_t Speed;
    float Position;
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

    Data1 data1;
    Data2 data2;
    Data3 data3;
    esp_err_t err;
    uint8_t Cmd;
} DataClaim;
// Hàm gửi lệnh tắt motor:
esp_err_t send_off_motor(uint32_t id) 
{
    twai_message_t tx_message;
    tx_message.identifier = id;
    tx_message.data_length_code = 8; 
    // Sao chép dữ liệu vào tin nhắn
    tx_message.data[0] = 0x80;
    tx_message.data[1] = 0x00;
    tx_message.data[2] = 0x00;
    tx_message.data[3] = 0x00;
    tx_message.data[4] = 0x00;
    tx_message.data[5] = 0x00;
    tx_message.data[6] = 0x00;
    tx_message.data[7] = 0x00;

    // Gửi lệnh
    return twai_transmit(&tx_message, pdMS_TO_TICKS(1000));
}
// Hàm gửi lệnh bật motor:
esp_err_t send_on_motor(uint32_t id) 
{
    twai_message_t tx_message;
    tx_message.identifier = id;
    tx_message.data_length_code = 8; 
    // Sao chép dữ liệu vào tin nhắn
    tx_message.data[0] = 0x88;
    tx_message.data[1] = 0x00;
    tx_message.data[2] = 0x00;
    tx_message.data[3] = 0x00;
    tx_message.data[4] = 0x00;
    tx_message.data[5] = 0x00;
    tx_message.data[6] = 0x00;
    tx_message.data[7] = 0x00;

    // Gửi lệnh
    return twai_transmit(&tx_message, pdMS_TO_TICKS(1000));
}
// Hàm gửi lệnh dừng tạm thời motor:
esp_err_t send_stop_motor(uint32_t id) 
{
    twai_message_t tx_message;
    tx_message.identifier = id;
    tx_message.data_length_code = 8; 
    // Sao chép dữ liệu vào tin nhắn
    tx_message.data[0] = 0x81;
    tx_message.data[1] = 0x00;
    tx_message.data[2] = 0x00;
    tx_message.data[3] = 0x00;
    tx_message.data[4] = 0x00;
    tx_message.data[5] = 0x00;
    tx_message.data[6] = 0x00;
    tx_message.data[7] = 0x00;

    // Gửi lệnh
    return twai_transmit(&tx_message, pdMS_TO_TICKS(1000));
}
// Hàm điều khiển torque:
/**
 * Lưu ý khi sử dụng hàm
 * id : 11bit
 * Torque = giá trị momen tính ra x 245
 * Giá trị Torque không được để quá 1250 (ảnh hưởng tuổi thọ động cơ)
 **/
esp_err_t send_torque_control(uint32_t id, int16_t Torque) 
{
    //
    twai_message_t tx_message;
    tx_message.identifier = id;
    tx_message.data_length_code = 8; 
    // Sao chép dữ liệu vào tin nhắn
    tx_message.data[0] = 0xA1;
    tx_message.data[1] = 0x00;
    tx_message.data[2] = 0x00;
    tx_message.data[3] = 0x00;
    tx_message.data[4] = Torque & 0xFF;
    tx_message.data[5] = Torque >> 8;
    tx_message.data[6] = 0x00;
    tx_message.data[7] = 0x00;

    // Gửi lệnh
    return twai_transmit(&tx_message, pdMS_TO_TICKS(1000));
}
// Hàm điều khiển tốc độ động cơ:
/**
 *  Giá trị speed là 0.01dps/ LSB (độ trên giây)
 *  Muốn động cơ quay theo giá trị thật thì nhân với 1000 (100*10 vì hộp số 1:10)
 *  Lưu ý điều chỉnh tốc độ vừa phải.
 **/
esp_err_t send_speed_control(uint32_t id, int32_t Speed) // 
{
    twai_message_t tx_message;
    tx_message.identifier = id;
    tx_message.data_length_code = 8; 
    // Sao chép dữ liệu vào tin nhắn
    tx_message.data[0] = 0xA2;
    tx_message.data[1] = 0x00;
    tx_message.data[2] = 0x00;
    tx_message.data[3] = 0x00;
    tx_message.data[4] = Speed & 0xFF;
    tx_message.data[5] = Speed >> 8;
    tx_message.data[6] = Speed >> 16;
    tx_message.data[7] = Speed >> 24;

    // Gửi lệnh
    return twai_transmit(&tx_message, pdMS_TO_TICKS(1000));
}
// Hàm điều khiển vị trí chính xác theo góc (giới hạn trong 1 vòng):
/**
 *  Phù hợp để làm điều khiển vị trí cho robot.
 *  Điều khiển đến góc mong muốn của động cơ so với góc 0 (của động cơ) (điều khiển tuyệt đối)
 *  Lưu ý : - Direction có 2 giá trị là 0x00 và 0x01, 0x00 quay ngược chiềU kim đồng hồ, 0x01 ngược lại.
 *          - MaxSpeed = tốc độ mong muốn thực tế * 1000.
 *          - Angle = góc mong muốn * 1000.
 *          - Giá trị Angle của lệnh này nên từ 0 - 360 độ (0 - 360 000)
 **/
esp_err_t send_single_loop_angle_control(uint32_t id, uint8_t Direction , uint16_t MaxSpeed, uint32_t Angle) //Direction: 0x01 or 0x00; Angle nhân với 1000 sẵn mới nhập.
{
    //Direction: 0x00 quay ngược chiều kim đồng hồ, 0x01 quay thuận.
    twai_message_t tx_message;
    // Nhập tốc độ và góc ở giá trị thật để hàm tự xử lý
    // MaxSpeed = MaxSpeed*100;
    tx_message.identifier = id;
    tx_message.data_length_code = 8;  
    // Sao chép dữ liệu vào tin nhắn
    tx_message.data[0] = 0xA6;
    tx_message.data[1] = Direction & 0x01;
    tx_message.data[2] = MaxSpeed & 0xFF;
    tx_message.data[3] = MaxSpeed >> 8;
    tx_message.data[4] = (uint8_t)(Angle & 0x000000FF);
    tx_message.data[5] = (uint8_t)((Angle & 0x0000FF00) >> 8);
    tx_message.data[6] = (uint8_t)((Angle & 0x00FF0000) >> 16);
    tx_message.data[7] = (uint8_t)((Angle & 0xFF000000) >> 24);
    return twai_transmit(&tx_message, pdMS_TO_TICKS(2000));
}
// Hàm gửi lệnh quay theo vị trí góc mong muốn:
/**
 * Lệnh điều khiển động cơ quay theo góc mà mình muốn.
 *  Lưu ý : - MaxSpeed = tốc độ mong muốn thực tế * 1000.
 *          - Angle = (+-)góc mong muốn * 1000.   limit (0 - 359999)*1000
 */
esp_err_t send_increment_angle_control(uint32_t id, uint16_t MaxSpeed, uint32_t Angle) 
{
    twai_message_t tx_message;
    // Nhập tốc độ và góc ở giá trị thật để hàm tự xử lý
    //MaxSpeed = MaxSpeed*100;
    //Angle = Angle*1000;
    tx_message.identifier = id;
    tx_message.data_length_code = 8; 
    // Sao chép dữ liệu vào tin nhắn
    tx_message.data[0] = 0xA8;
    tx_message.data[1] = 0x00;
    tx_message.data[2] = MaxSpeed & 0xFF;
    tx_message.data[3] = MaxSpeed >> 8;
    tx_message.data[4] = Angle & 0xFF;
    tx_message.data[5] = Angle >> 8;
    tx_message.data[6] = Angle >> 16;
    tx_message.data[7] = Angle >> 24;
    // Gửi lệnh
    return twai_transmit(&tx_message, pdMS_TO_TICKS(1000));
}
// Hàm clear_error:
esp_err_t send_Clear_error(uint32_t id)
{
    twai_message_t tx_message;
    tx_message.identifier = id;
    tx_message.data_length_code = 8; 
    tx_message.data[0] = 0x9B;
    tx_message.data[1] = 0x00;
    tx_message.data[2] = 0x00;
    tx_message.data[3] = 0x00;
    tx_message.data[4] = 0x00;
    tx_message.data[5] = 0x00;
    tx_message.data[6] = 0x00;
    tx_message.data[7] = 0x00;
    esp_err_t err = twai_transmit(&tx_message, pdMS_TO_TICKS(1000));  // Timeout sau 1000ms
    twai_message_t rx_message;
    // Chờ để nhận dữ liệu phản hồi từ động cơ  
    err = twai_receive(&rx_message, pdMS_TO_TICKS(1000));  // Timeout sau 1000ms
    if (err == ESP_OK) 
    {
        //printf("Received TWAI message with ID: 0x%lx\n", rx_message.identifier);
        // Kiểm tra ID của gói tin nhận được
        if (rx_message.identifier == 0x141) {  // Nếu ID phản hồi từ động cơ là 0x141
            // Giải mã dữ liệu phản hồi
            int8_t temp = rx_message.data[1];     // Byte 0 có thể là trạng thái
            uint16_t voltage = (rx_message.data[4] << 8) | rx_message.data[3]; // Byte 1-2: Tốc độ
            uint8_t error_motor = rx_message.data[7] ; // Byte 3-4: Vị trí
            // printf("Temperature: %d, Torque Current: %d, Speed: %d, Position: %d\n", temp, voltage);
            uint8_t bit_voltage;
            bit_voltage = error_motor & 0x01;
            uint8_t bit_temp;
            bit_temp = (error_motor) & 0b00001000;
            printf("Temp: %d, Voltage: %d \n", temp, voltage);
            if (bit_temp == 0b00001000)
            {
                printf("Over Temperature!!!\n");
            }
            else if (bit_voltage == 1)
            {
                printf("Low Voltage !!!");
            }
            else
            {
                printf("No Error <3 !\n");
            }
        }
    } else 
    {
        printf("Failed to receive TWAI message\n");
    }
    return err;
}
// Hàm gửi lệnh trả về dữ liệu 1 :đọc dữ liệu nhiệt độ, điện áp và trạng thái lỗi
esp_err_t command_read_state_1(uint32_t id)
{
    twai_message_t tx_message;
    tx_message.identifier = id;
    tx_message.data_length_code = 8; 
    tx_message.data[0] = 0x9A;
    tx_message.data[1] = 0x00;
    tx_message.data[2] = 0x00;
    tx_message.data[3] = 0x00;
    tx_message.data[4] = 0x00;
    tx_message.data[5] = 0x00;
    tx_message.data[6] = 0x00;
    tx_message.data[7] = 0x00;
    twai_transmit(&tx_message, pdMS_TO_TICKS(1000));  // Timeout sau 1000ms
    return twai_transmit(&tx_message, pdMS_TO_TICKS(1000));;
}
// Hàm ra lệnh trả về trạng thái số 2: đọc nhiệt độ, điện áp, tốc độ và giá trị encoder.
esp_err_t command_read_state_2(uint32_t id)
{
    twai_message_t tx_message;
    tx_message.identifier = id;
    tx_message.data_length_code = 8; 
    tx_message.data[0] = 0x9C;
    tx_message.data[1] = 0x00;
    tx_message.data[2] = 0x00;
    tx_message.data[3] = 0x00;
    tx_message.data[4] = 0x00;
    tx_message.data[5] = 0x00;
    tx_message.data[6] = 0x00;
    tx_message.data[7] = 0x00;
    return twai_transmit(&tx_message, pdMS_TO_TICKS(1000));  // Timeout sau 1000ms
}
// Hàm ra lệnh trả về trạng thái số 3: đọc nhiệt độ, dòng điện từng pha.
esp_err_t command_read_state_3(uint32_t id)
{
    twai_message_t tx_message;
    tx_message.identifier = id;
    tx_message.data_length_code = 8; 
    tx_message.data[0] = 0x9D;
    tx_message.data[1] = 0x00;
    tx_message.data[2] = 0x00;
    tx_message.data[3] = 0x00;
    tx_message.data[4] = 0x00;
    tx_message.data[5] = 0x00;
    tx_message.data[6] = 0x00;
    tx_message.data[7] = 0x00;
    return twai_transmit(&tx_message, pdMS_TO_TICKS(1000));  // Timeout sau 1000ms
}

DataClaim receive_data() 
{
    DataClaim Data;
    twai_message_t rx_message;
    Data.err = twai_receive(&rx_message, pdMS_TO_TICKS(1000));
    if (Data.err == ESP_OK) 
    {
        //Kiểm tra byte đầu tiên:
        
        Data.Cmd = rx_message.data[0];
        printf("Received message from motor: ID = 0x%03lX, Data = ", rx_message.identifier);
        if(Data.Cmd == 0x9A) // Command của lệnh gửi nhận data 1
        {
            Data.data1.Temp = rx_message.data[1];     // Byte 1 là nhiệt độ
            Data.data1.Vol = (((uint16_t)(rx_message.data[3] << 8) | (uint16_t)rx_message.data[4])); // Byte 4-3 là điện áp
            Data.data1.Error = rx_message.data[7] ; // Byte 7 và lỗi
            //printf("Temp: %d, Voltage: %.3f \n", Data.data1.Temp, Data.data1.Vol);
            printf("Hex Vol = 0x%X 0x%X\n",rx_message.data[4], rx_message.data[3]);
            uint8_t bit_voltage;
            bit_voltage = Data.data1.Error & 0x01;
            uint8_t bit_temp;
            bit_temp = (Data.data1.Error) & 0b00001000;
            if (bit_temp == 0b00001000)
            {
                printf("Over Temperature!!!\n");
            }
            else if (bit_voltage == 1)
            {
                printf("Low Voltage !!!");
            }
            else
            {
                printf("No Error <3 !\n");
            }
        }
        else if(Data.Cmd == 0x9D)
        {
            Data.data3.Temp = rx_message.data[1];     // Byte 1 là nhiệt độ động cơ
            Data.data3.A_PhaseCur = ((rx_message.data[3] << 8) | rx_message.data[2]); // Byte 2-3: A phase current
            Data.data3.B_PhaseCur = ((rx_message.data[5] << 8) | rx_message.data[4]); // Byte 4-5: B phase current
            Data.data3.C_PhaseCur = ((rx_message.data[7] << 8) | rx_message.data[6]); // Byte 6-7: C phase current
            //printf("Temperature: %d, Phase A: %d, Phase B: %d, Phase B: %d\n", temp, current_A, current_B, current_C);
        }
        else
        {   
            Data.data2.Temp = rx_message.data[1];     // Byte 1 là nhiệt độ động cơ
            Data.data2.TorCurrent = ((rx_message.data[3] << 8) | rx_message.data[2]); // Byte 2-3: torque current (datasheet động cơ)
            Data.data2.Speed = (rx_message.data[5] << 8) | rx_message.data[4]; // Byte 4-5: speed
            Data.data2.Position = ((float)((rx_message.data[7] << 8) | rx_message.data[6]))/182; // Byte 6-7: position/  Chia cho 182
            
                //printf("Temperature: %d, Torque Current: %d, Speed: %d, Position: %f\n", temp, torque_current, speed, position);
        }
        // printf("data2: %d, data3: %d\n", rx_message.data[2],rx_message.data[3]);
    }
    return Data;
}

// Call every 1ms
// void fnc_task_scheduler()
// {
//     uint16_t u16Counter_1ms = Fnc_Get_Time_cnt_1ms();


//     // for 10ms tasks
//     if ((u16Counter_1ms % 10) == 0)  // 0, 10, 20, ...
//     {
//         Fnc_task_1_10ms();
//     }
//     else if ((u16Counter_1ms % 10) == 7 // 5, 15, 25, ...
//     {
//         Fnc_task_2_10ms();
//     }

//     // for 100ms tasks
//     if ((u16Counter_1ms % 100) == 20)  // 20, 120, 227 ...
//     {
//         Fnc_task_1_100ms();
//     }
//     else if ((u16Counter_1ms % 10) == 88) // 88, 188, 288, ...
//     {
//         Fnc_task_2_100ms(); // Motor communicate
//     }

// }