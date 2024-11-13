/*
 * uart_communication_fsm.c
 *
 *  Created on: 3 Nov 2024
 *      Author: HPVictus
 */

#include "uart_communication_fsm.h"

#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>

// Định nghĩa các trạng thái của FSM
typedef enum {
    IDLE,
    SEND_ADC,
    WAIT_OK
} UartCommState;

UartCommState uart_state = IDLE;      // Trạng thái ban đầu của FSM
//uint8_t command_flag = 0;             // Cờ báo hiệu có lệnh mới từ command_parser_fsm
uint16_t ADC_value = 1234;            // Giá trị ADC giả sử
uint32_t timeout_counter = 0;         // Bộ đếm thời gian chờ
char response_buffer[30];             // Bộ đệm để lưu chuỗi phản hồi

void uart_communication_fsm() {
    switch (uart_state) {
        case IDLE:
            // Kiểm tra nếu có lệnh yêu cầu !RST# từ người dùng
            if (command_flag == 1) {
                if (string_compare((char *)command_data, "!RTS#") == 0) {
                    // Thiết lập lại cờ và chuyển sang trạng thái SEND_ADC
                	memset(command_data, 0, sizeof(command_data));
                    command_flag = 0;
                    uart_state = SEND_ADC;
                }
            }
            break;

        case SEND_ADC:
            // Chuẩn bị chuỗi phản hồi !ADC=1234#
        	HAL_ADC_Start(&hadc1); // Bắt đầu ADC
        	ADC_value = HAL_ADC_GetValue (& hadc1);
        	HAL_ADC_Stop(&hadc1);
            snprintf(response_buffer, sizeof(response_buffer), "!ADC=%d#", ADC_value);

            // Gửi chuỗi phản hồi qua UART
            HAL_UART_Transmit(&huart2, (uint8_t*)response_buffer, strlen(response_buffer), 50);
            HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 50);
            // Khởi động bộ đếm timeout và chuyển sang trạng thái WAIT_OK
            timeout_counter = HAL_GetTick();
            uart_state = WAIT_OK;



            break;

        case WAIT_OK:
            // Kiểm tra nếu nhận được lệnh !OK#
            if (command_flag == 1) {
                if (string_compare((char *)command_data, "!OK#") == 0) {
                    // Thiết lập lại cờ và trở về trạng thái IDLE
                	HAL_UART_Transmit(&huart2, (uint8_t*)"IDLE", 4, 50);
                	HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 50);
                    command_flag = 0;
                    uart_state = IDLE;
                }
            }

            // Kiểm tra timeout 3 giây
            if (HAL_GetTick() - timeout_counter > 3000) {
                // Timeout, gửi lại giá trị ADC
                HAL_UART_Transmit(&huart2, (uint8_t*)response_buffer, strlen(response_buffer), 50);
                HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 50);
                // Reset lại bộ đếm thời gian chờ
                timeout_counter = HAL_GetTick();
            }
            break;

        default:
            uart_state = IDLE;
            break;
    }
}

