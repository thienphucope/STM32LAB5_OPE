/*
 * command_parser_fsm.c
 *
 *  Created on: 3 Nov 2024
 *      Author: HPVictus
 */


#include "command_parser_fsm.h"

uint8_t temp = 0;
uint8_t buffer[MAX_BUFFER_SIZE];
uint8_t index_buffer = 0;
uint8_t buffer_flag = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        // HAL_UART_Transmit(&huart2, &temp, 1, 50);
    	if (temp == '\b')
    	{
    		index_buffer--;
    	}
    	else
    	{
    		buffer[index_buffer++] = temp;
        	if (index_buffer == MAX_BUFFER_SIZE) index_buffer = 0;
        	buffer_flag = 1;
    	}
        HAL_UART_Receive_IT(&huart2, &temp, 1);
    }
}
int string_compare(const char *str1, const char *str2) {
    // So sánh từng ký tự của 2 chuỗi cho đến khi gặp kết thúc chuỗi hoặc khác biệt
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return -1; // Chuỗi không khớp
        }
        str1++;
        str2++;
    }

    // Kiểm tra xem một trong hai chuỗi có hết trước hay không
    if (*str1 || *str2) {
        return -1; // Chuỗi không khớp vì độ dài khác nhau
    }

    return 0; // Chuỗi khớp hoàn toàn
}


typedef enum {
    WAIT_FOR_START,
    READ_COMMAND,
    PROCESS_COMMAND
} CommandParserState;

CommandParserState command_parser_state = WAIT_FOR_START; // Trạng thái ban đầu
uint8_t command_flag = 0;                                  // Cờ lệnh
uint8_t command_data[30];                                  // Dữ liệu lệnh nhận được
uint8_t command_index = 0; // Chỉ số cho command_data
void command_parser_fsm() {


    switch (command_parser_state) {
        case WAIT_FOR_START:
            // Kiểm tra ký tự đầu tiên
            if (buffer[0] == '!') {
                command_index = 0; // Reset chỉ số command
                command_parser_state = READ_COMMAND; // Chuyển sang trạng thái đọc lệnh
            }



            break;

        case READ_COMMAND:
            // Đọc từng ký tự vào command_data
        	command_index = 0;
            for (uint8_t i = 1; i < index_buffer; i++) {
                // Nếu gặp ký tự '#', kết thúc lệnh
                if (buffer[i] == '#') {
                    command_data[command_index] = '\0'; // Kết thúc chuỗi
                    command_parser_state = PROCESS_COMMAND; // Chuyển sang trạng thái xử lý lệnh
                    break;
                }

                // Lưu ký tự vào command_data
                command_data[command_index++] = buffer[i];
            }
            break;
        case PROCESS_COMMAND:

            // Phân tích lệnh đã nhận
        	if (buffer[index_buffer-1] == '\r') {

                if (string_compare((char *)command_data, "RTS") == 0) {
                	HAL_GPIO_TogglePin(YELLOW_LED_GPIO_Port, YELLOW_LED_Pin);
                    command_flag = 1; // Đặt cờ lệnh yêu cầu dữ liệu ADC
                } else if (string_compare((char *)command_data, "OK") == 0) {

                    command_flag = 1; // Đặt cờ lệnh kết thúc giao tiếp
                } else {
                    command_flag = 0; // Không có lệnh hợp lệ
                }

                // Reset trạng thái và buffer để chuẩn bị cho lệnh tiếp theo
                command_parser_state = WAIT_FOR_START; // Quay lại trạng thái chờ
                memset(buffer, 0, sizeof(buffer)); // Xóa buffer
                index_buffer = 0; // Reset chỉ số buffer
                command_index = 0;
                break;
        	}


        default:
            command_parser_state = WAIT_FOR_START; // Đặt lại trạng thái về mặc định
            break;
    }
}

