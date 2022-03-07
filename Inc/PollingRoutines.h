/*
 * PollingRoutines.h
 *
 *  Created on: Apr 29, 2021
 *      Author: Street Geckos
 */

#ifndef INC_POLLINGROUTINES_H_
#define INC_POLLINGROUTINES_H_
#ifdef __cplusplus
extern "C" {
#endif
#include<stdbool.h>
#include "FLASH_SECTOR_F4.h"
#define UART_BUF_SIZE 20
#define CAN_BUF_SIZE  50
#define CANDATA_BUF_SIZE  8
#define CANEXTID_BUF_SIZE 29
#define flash_sector =  0x081E0000

void PollingInit();
void PollingRoutine();
void SendUartMsg(char *msg);
void CAN2_Tx(uint8_t* tata, int id,int size,int Type);
void dataReconstruction( uint8_t* Rxdata);
void infoRequest();
bool dataMach( uint8_t* Rxdata, uint8_t* data, int size);

typedef union
{

	struct{
		uint8_t byte1;
		// uint8_t byte2;
	}Bytes;

	//stucture pour boutton
	struct{
		unsigned NavLight:1;
		unsigned Nav:1;
		unsigned Vhf:1;
		unsigned courtesylight:1;
		unsigned dome:1;
		unsigned spot:1;
		unsigned water:1;
		unsigned music:1;
		unsigned GPS:1;
		unsigned Water2:1;
		unsigned PlayPause:1;
		unsigned Next:1;
		unsigned Previous:1;
		int Volume;
		int Volume2;

		unsigned :9;
	}Status;
}ButtonClicked;




#ifdef __cplusplus
}
#endif

#endif /* INC_POLLINGROUTINES_H_ */
