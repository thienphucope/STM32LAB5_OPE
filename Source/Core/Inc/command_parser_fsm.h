/*
 * command_parser_fsm.h
 *
 *  Created on: 3 Nov 2024
 *      Author: HPVictus
 */

#ifndef INC_COMMAND_PARSER_FSM_H_
#define INC_COMMAND_PARSER_FSM_H_

#include <string.h>
#include "stdio.h"
#include "main.h"

#define MAX_BUFFER_SIZE 30

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void command_parser_fsm();
int string_compare(const char *str1, const char *str2);
extern uint8_t buffer_flag;
extern uint8_t buffer[MAX_BUFFER_SIZE];
extern uint8_t temp;
extern uint8_t index_buffer;

extern uint8_t command_flag;
extern uint8_t command_data[30];
#endif /* INC_COMMAND_PARSER_FSM_H_ */
