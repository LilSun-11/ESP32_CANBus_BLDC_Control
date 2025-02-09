#include <stdio.h>
#include "driver/gpio.h"
#include "driver/twai.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "MotorCAN.c"
#if(0)
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
    // Hàm điều khiển torque
    esp_err_t send_torque_control(uint32_t id, int16_t Torque) // Torque current range: -2048~2048, nên đặt từ -1000 đến 1000 thôi ! 
    {
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
    // Hàm điều khiển vị trí chính xác theo góc (giới hạn trong 1 vòng):
    esp_err_t send_single_loop_angle_control(uint32_t id, uint8_t Direction , uint16_t MaxSpeed, uint32_t Angle) //Direction: 0x01 or 0x00; Angle nhân với 1000 sẵn mới nhập.
    {
        //Direction: 0x00 quay ngược chiều kim đồng hồ, 0x01 quay thuận.
        twai_message_t tx_message;
        // Nhập tốc độ và góc ở giá trị thật để hàm tự xử lý
        MaxSpeed = MaxSpeed*100;
        
        
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
        // for (int i = 4; i < 8 ; i++)
        // {
        //     printf("%x\n",tx_message.data[i]);
        // }
        
        // Gửi lệnh
        return twai_transmit(&tx_message, pdMS_TO_TICKS(2000));
    }
    // Hàm gửi lệnh quay theo vị trí góc mong muốn:
    esp_err_t send_increment_angle_control(uint32_t id, uint16_t MaxSpeed, uint32_t Angle) 
    {
        twai_message_t tx_message;
        // Nhập tốc độ và góc ở giá trị thật để hàm tự xử lý
        MaxSpeed = MaxSpeed*100;
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
    // Hàm ra lệnh trả về trạng thái số 2:
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
    // Hàm ra lệnh trả về trạng thái số 3:
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
    if (Data.err == ESP_OK) {
        //Kiểm tra byte đầu tiên:
        
        Data.Cmd = rx_message.data[0];
        printf("Received message from motor: ID = 0x%03lX, Data = ", rx_message.identifier);
        if(Data.Cmd == 0x9A) // Command của lệnh gửi nhận data 1
        {
            Data.data1.Temp = rx_message.data[1];     // Byte 1 là nhiệt độ
            Data.data1.Vol = ((float)((rx_message.data[4] << 8) | rx_message.data[3]))/10; // Byte 4-3 là điện áp
            Data.data1.Error = rx_message.data[7] ; // Byte 7 và lỗi
            printf("Temp: %d, Voltage: %.3f \n", Data.data1.Temp, Data.data1.Vol);
            printf("Hex Vol = %X %X\n",rx_message.data[4], rx_message.data[3]);
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
            uint8_t temp = rx_message.data[1];     // Byte 1 là nhiệt độ động cơ
            int16_t current_A = ((rx_message.data[3] << 8) | rx_message.data[2]); // Byte 2-3: A phase current
            int16_t current_B = ((rx_message.data[5] << 8) | rx_message.data[4]); // Byte 4-5: B phase current
            int16_t current_C = ((rx_message.data[7] << 8) | rx_message.data[6]); // Byte 6-7: C phase current
            printf("Temperature: %d, Phase A: %d, Phase B: %d, Phase B: %d\n", temp, current_A, current_B, current_C);
        }
        else
        {   
            uint8_t temp = rx_message.data[1];     // Byte 1 là nhiệt độ động cơ
            int16_t torque_current = ((rx_message.data[3] << 8) | rx_message.data[2]); // Byte 2-3: torque current (datasheet động cơ)
            int16_t speed = (rx_message.data[5] << 8) | rx_message.data[4]; // Byte 4-5: speed
            float position = ((float)((rx_message.data[7] << 8) | rx_message.data[6]))/182; // Byte 6-7: position
            
            printf("Temperature: %d, Torque Current: %d, Speed: %d, Position: %f\n", temp, torque_current, speed, position);
        }
        // printf("data2: %d, data3: %d\n", rx_message.data[2],rx_message.data[3]);
    }
    return Data;
}
#endif
void app_main(void)
{
     // 1. Cấu hình chung cho TWAI, tx chân 21, rx chân 22 (CAN transciever)
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_21, GPIO_NUM_22, TWAI_MODE_NORMAL);

    // 2. Cấu hình thời gian (baud rate) cho TWAI, tùy chỉnh cho phù hợp với động cơ của bạn
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();  // 500 kbps, điều chỉnh nếu cần

    // 3. Cấu hình bộ lọc cho TWAI (có thể lọc tất cả các ID hoặc chỉ định các ID cụ thể)
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    // 4. Khởi động driver TWAI
    esp_err_t err = twai_driver_install(&g_config, &t_config, &f_config);
    if (err == ESP_OK) 
    {
        printf("TWAI driver installed\n");
        err = twai_start();
        if (err == ESP_OK) 
        {
            printf("TWAI driver started\n");
        }
    } else 
    {
        printf("Failed to install TWAI driver\n");
    }
    err = send_speed_control(0x141, 180000);
    DataClaim Data = receive_data();
    //err = send_Clear_error(0x141);
    while(true)
    {
            // err = send_increment_angle_control(0x141, 9 , 90000);
            // // err = command_read_state_1(0x141);
            // DataClaim Data = receive_data();
            // err = Data.err;
            // if (err != ESP_OK) 
            // {
            //     printf("No response from motor: %s\n", esp_err_to_name(err));
            // }
            //printf("Temperature = %d, TorqueCurrent = %d, Speed = %d, Position = %.4f\n", Data.data2.Temp,Data.data2.TorCurrent,Data.data2.Speed,Data.data2.Position);
            vTaskDelay(pdMS_TO_TICKS(20));
            for (int i = 0; i < 50 ; i ++)
            {
                err = command_read_state_3(0x141);
                Data = receive_data();
                err = Data.err;
                if (err != ESP_OK) 
                {
                    printf("No response from motor: %s\n", esp_err_to_name(err));
                }
                printf("Temperature: %d, Phase A: %d, Phase B: %d, Phase C: %d\n", Data.data3.Temp, Data.data3.A_PhaseCur, Data.data3.B_PhaseCur, Data.data3.C_PhaseCur);
                //printf("Temp: %d, Voltage: %d \n", Data.data1.Temp, Data.data1.Vol);
                //printf("Temperature = %d, TorqueCurrent = %d, Speed = %d, Position = %.4f\n", Data.data2.Temp,Data.data2.TorCurrent,Data.data2.Speed,Data.data2.Position);
                vTaskDelay(pdMS_TO_TICKS(20));
            }
            vTaskDelay(pdMS_TO_TICKS(2));
    }

}
