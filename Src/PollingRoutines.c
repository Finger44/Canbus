#include"main.h"
#include"PollingRoutines.h"
#include"string.h"
#include"cmsis_os.h"
#include"stdio.h"
#include"semphr.h"
#include<stdbool.h>

extern UART_HandleTypeDef huart3;
extern osSemaphoreId BinarySemUsartMsgHandle;
extern osMessageQId MainScreenHandle;
extern CAN_TxHeaderTypeDef TxHeader;
extern CAN_HandleTypeDef hcan2;
extern CAN_RxHeaderTypeDef RxHeader;

CAN_TxHeaderTypeDef TxHeader2;
ButtonClicked screenButtonClicked;

extern uint32_t data_to_store[10];

uint8_t uartMsgBuff[UART_BUF_SIZE];
uint8_t RxData[CANDATA_BUF_SIZE];
uint8_t RxData1[CANDATA_BUF_SIZE];
uint8_t song[CAN_BUF_SIZE];
uint8_t song2[CAN_BUF_SIZE];
uint8_t uartMsgData[2];
uint8_t our_message[CANDATA_BUF_SIZE] = {0X40, 0X6, 0XA3, 0X99, 0X03, 0x00, 0X8, 0X1 };
uint8_t buffselect;

uint8_t Info[3]                    = {0x14, 0xF0, 0x01 };
uint8_t info_request[3]          =   {0x16, 0xf0, 0x01 };
uint8_t On[CANDATA_BUF_SIZE]       = {0x20, 0x04, 0xA3,0x99,0x01,0x00,0xff,0xff };
uint8_t OFF[CANDATA_BUF_SIZE]      = {0x80, 0x05, 0xA3,0x99,0x1C,0x00,0x02,0xFF };

uint8_t play[CANDATA_BUF_SIZE]     = {0x80, 0x06, 0xA3,0x99,0x03,0x00,0x08,0x01 };
uint8_t pause[CANDATA_BUF_SIZE]    = {0x40, 0x06, 0xA3,0x99,0x03,0x00,0x08,0x02 };
uint8_t pause2[CANDATA_BUF_SIZE]   = {0x60, 0x06, 0xA3,0x99,0x03,0x00,0x08,0x02 };
uint8_t Next[CANDATA_BUF_SIZE]     = {0x20, 0x06, 0xA3,0x99,0x03,0x00,0x08,0x04 };
uint8_t Previous[CANDATA_BUF_SIZE] = {0x60, 0x06, 0xA3,0x99,0x03,0x00,0x08,0x06 };
//volumeconst int zone1=const screenButtonClicked.Status.Volume;

uint8_t Volume[CANDATA_BUF_SIZE]   = {0xA0, 0x08, 0xA3,0x99,0x19,0x00,0x01, 0x00};
uint8_t Mute[CANDATA_BUF_SIZE]     = {0xA0, 0x08, 0xA3,0x99,0x19,0x00,0x01, 0x01};
uint8_t Zone[CANDATA_BUF_SIZE]     = {0xA1, 0x01, 0x01,0xFF,0xFF,0xFF,0xFF, 0xFF};


uint8_t assigne[CANDATA_BUF_SIZE]   = {0xA0, 0x08, 0x0A, 0x01,0x19,0x00,0x01, 0x00};
uint8_t calSens[CANDATA_BUF_SIZE]   = {0xA0, 0x08, 0xC0, 0x01,0x00,0x00,0x00, 0x00};
uint8_t Name[CANDATA_BUF_SIZE]      = {0x80, 0x86, 0x15, 0x05,0x7E,0x73,0x74, 0x72};
uint8_t Name1[CANDATA_BUF_SIZE]     = {0x81, 0x65, 0x65, 0x74,0x20,0x00,0xff, 0xff};

uint32_t controller_link[3] = {0,0,0};
int next_controller =1;
int finding =0;
int id  = 0x4D6E5347; // main ID
uint32_t RxID = 0x000000001347c103;
int id2 = 0x18EAFF21;
int id1 = 0x1DEF0521;
int id3 = 0x1DEF0469;
uint8_t Post = 0;
uint8_t msgIndex = 0;
uint8_t msgRdyFlag = 0;
uint8_t old_data0 = 0 ;
uint8_t new_data0 = 0;


uint32_t TxMailbox;

const char VhfOn[] =  " VHF is On \n\r";
const char VhfOFF[] =  " VHF is OFF \n\r";

const char SpotOn[] =  " Spot is On \n\r";
const char SpotOFF[] =  " Spot is OFF \n\r";

const char NavOn[] =  " Navigation Light is On \n\r";
const char NavOFF[] =  " Navigation Light is OFF \n\r";

const char CourtesyOn[] =  " Courtesy is On \n\r";
const char CourtesyOFF[] =  " Courtesy is OFF \n\r";

const char DomeOn[] =  " Dome is On \n\r";
const char DomeOFF[] =  " Dome is OFF \n\r";

const char Water1On[] =  " Water is On \n\r";
const char Water1OFF[] =  " Water is OFF \n\r";

const char MusicOn[] =  " Music is On \n\r";
const char MusicOFF[] =  " Music is OFF \n\r";

const char GpsOn[] =  " Gps is On \n\r";
const char GpsOFF[] =  " Gps is OFF \n\r";

const char Water2On[] =  " water 2 is On \n\r";
const char Water2OFF[] =  " water 2 is OFF \n\r";

const char Play[] =  " Play \n\r";
const char Pause[] =  " Pause \n\r";
const char previous[] =  " previous \n\r";
const char next[] =  " next \n\r";

const char CanRec[] =  " Can Message Received \n\r";
 uint8_t detect_message[] =  "STREET GECKOS";
 char detect_message_type[] =  "Controller";
char* CanSend = "Can Data Send!! \n\r";



//40 06 a3 99 03 00 08 01

char myString[100];


void PollingInit()
{
	//enable interrupt
	HAL_UART_Receive_IT(&huart3, uartMsgData, 1);
}


void PollingRoutine()
{
	uint16_t item;

	if(msgRdyFlag ==1)
	{
		xSemaphoreGive(BinarySemUsartMsgHandle);
		msgRdyFlag = 0;

	}

	if(xQueueReceive(MainScreenHandle, &item, (TickType_t)10) == pdPASS)
	{
		memset(&myString, 0, sizeof(myString));
		switch(item)
		{
		case 1://toggle button, led blue
			if(screenButtonClicked.Status.NavLight){
				//HAL_GPIO_WritePin(relay_GPIO_Port, relay_Pin, screenButtonClicked.Status.Vhf);
				strcat(myString, NavOn);
				// messgae to send , id to use , lenght of data, 0 for remote fram 1 for data frame
				CAN2_Tx(assigne,id,8,1);


			}
			else{
				strcat(myString, NavOFF);

			}
			break;

		case 2:
			if(screenButtonClicked.Status.Vhf){
				//HAL_GPIO_WritePin(relay_GPIO_Port, relay_Pin, screenButtonClicked.Status.Vhf);
				strcat(myString, VhfOn);
				CAN2_Tx(info_request,id,3,1);

			}
			else{
				strcat(myString, VhfOFF);

			}
			break;
		case 3:
			if(screenButtonClicked.Status.courtesylight){
				//HAL_GPIO_WritePin(relay_GPIO_Port, relay_Pin, screenButtonClicked.Status.Vhf);
				strcat(myString, CourtesyOn);

			}
			else{
				strcat(myString, CourtesyOFF);

			}
			break;
		case 4:
			if(screenButtonClicked.Status.dome){
				//HAL_GPIO_WritePin(relay_GPIO_Port, relay_Pin, screenButtonClicked.Status.Vhf);
				strcat(myString, DomeOn);
			}
			else{
				strcat(myString, DomeOFF);

			}
			break;

		case 5://toggle button, led blue
			if(screenButtonClicked.Status.spot){
				//HAL_GPIO_WritePin(relay_GPIO_Port, relay_Pin, screenButtonClicked.Status.Vhf);
				strcat(myString, SpotOn);


			}

			else{
				strcat(myString, SpotOFF);


			}
			break;
		case 6:
			if(screenButtonClicked.Status.water){
				//HAL_GPIO_WritePin(relay_GPIO_Port, relay_Pin, screenButtonClicked.Status.Vhf);
				strcat(myString, Water1On);

			}
			else{
				strcat(myString, Water1OFF);

			}
			break;
		case 7:
			if(screenButtonClicked.Status.music){
				//HAL_GPIO_WritePin(relay_GPIO_Port, relay_Pin, screenButtonClicked.Status.Vhf);
				//strcat(myString, MusicOn);
				CAN2_Tx(On,id1,8,1);
			}
			else{
				CAN2_Tx(OFF,id1,8,1);
				//strcat(myString, MusicOFF);

			}
			break;
		case 8:
			if(screenButtonClicked.Status.GPS){
				//HAL_GPIO_WritePin(relay_GPIO_Port, relay_Pin, screenButtonClicked.Status.Vhf);
				strcat(myString, GpsOn);
				CAN2_Tx(info_request,id,3,1);
				finding =1;
			}
			else
			{
				strcat(myString, GpsOFF);

			}
			break;
		case 9:
			if(screenButtonClicked.Status.Water2){
				//HAL_GPIO_WritePin(relay_GPIO_Port, relay_Pin, screenButtonClicked.Status.Vhf);
				strcat(myString, Water2On);

			}
			else{
				strcat(myString, Water2OFF);

			}
			break;
		case 10://play/pause
			if(screenButtonClicked.Status.PlayPause){

				strcat(myString, Play);

				CAN2_Tx(play,id1,8,1);
			}
			else{
				CAN2_Tx(pause,id1,8,1);
				strcat(myString, Pause);

			}
			break;
		case 11://previous song

			strcat(myString, previous);
			CAN2_Tx(Previous,id1,8,1);

			break;
		case 12://next song

			strcat(myString, next);
			CAN2_Tx(Next,id1,8,1);

			break;

		case 13://Slider volume,

			Volume[6]=screenButtonClicked.Status.Volume;
			Volume[7]=screenButtonClicked.Status.Volume2;
			CAN2_Tx(Volume,id1,8,1);
			CAN2_Tx(Zone,id1,8,1);

			break;
		case 14://calSensor

			CAN2_Tx(calSens,id,8,1);

			break;
		case 15://Module detection

			CAN2_Tx(info_request,id,3,1);
			finding =1;

			break;

		}
		SendUartMsg(myString);
	}
}

//send a uart message
void SendUartMsg(char *msg)
{
	if(HAL_UART_Transmit_IT(&huart3, (uint8_t *) myString, strlen(myString)) != HAL_OK)
	{
		Error_Handler();
	}
}

//interrupt for uart receive complete
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if(msgIndex == 0)

	{// clear uartBuff
		memset(&uartMsgBuff, 0, sizeof(uartMsgBuff));
	}

	if(uartMsgData[0] != '\n') // if not "\n" = 13

	{
		uartMsgBuff[msgIndex++] = uartMsgData[0];

	}

	else
	{
		uartMsgBuff[msgIndex] = uartMsgData[0];
		msgIndex = 0;

		msgRdyFlag = 1; //set flag to inform that a message is ready
	}

	//enable interrupt again
	HAL_UART_Receive_IT(&huart3, uartMsgData, 1);

}


//interrupt for message pending
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	//recover the message
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &RxHeader, RxData) != HAL_OK)
	{

		Error_Handler();
	}
	//send data for reconstruction

	dataReconstruction(RxData);


	/*if (RxHeader.DLC == 3)
	{
		if(dataMach(RxData, start_message2) == true)
		{
			CAN2_Tx(Name,id,8,1);
			CAN2_Tx(Name1,id,8,1);
		}

	}*/

	HAL_UART_Transmit(&huart3, (uint8_t*)CanRec, strlen(CanRec), HAL_MAX_DELAY);

	if(RxHeader.ExtId == RxID){

		for(int i=0; i < 8; i++ )
		{
			RxData1[i] = RxData[i];
		}
		RxHeader.ExtId =0;
		msgRdyFlag = 1;
	}
}

void CAN2_Tx(uint8_t* tata, int id, int size, int Type)
{

	TxHeader.DLC = size;
	TxHeader.ExtId = id; //remote id
	TxHeader.IDE   = CAN_ID_EXT;
	if(Type){
		TxHeader.RTR = CAN_RTR_DATA; // 1 for data frame
	}
	else{
		TxHeader.RTR = CAN_RTR_REMOTE; //0 for remote frame
	}
	if( HAL_CAN_AddTxMessage(&hcan2,&TxHeader,tata,&TxMailbox) != HAL_OK)
	{
		Error_Handler();
	}
	//it message has been send, send uart message
	else HAL_UART_Transmit(&huart3, (uint8_t*)CanSend, strlen(CanSend), HAL_MAX_DELAY);

}

//F***ing data reconstruction
void dataReconstruction( uint8_t* Rxdata)
{

	///Song Title + Author Reconstruction
	//            0x01           0x08             0x80
	//if the following data correspond we ha something to display 5 for title
	if(RxData[7]==1 &&RxData[6] == 8 /*&&RxData[5] == 128*/)
	{

		switch(RxData[4]){

		case 5: // 0x05 for title
			old_data0= RxData[0];
			new_data0 = old_data0+1; //store and increment data[0]
			Post =0;
			memset(song, 0, sizeof(song));
			buffselect = 1;
			break;

		case 6://0x07 for Author
			old_data0= RxData[0];
			new_data0 = old_data0+1; //store and increment data[0]
			Post =0;
			memset(song2, 0, sizeof(song2));
			buffselect = 2;
			break;

		}
	}
	if(finding){

		if(dataMach(song, detect_message,14)){
		old_data0 = RxData[0];
		new_data0 = old_data0+1; //store and increment data[0]
		Post =0;
		memset(song, 0, sizeof(song));
		buffselect = 1;
		}

	}
	/*HAL_UART_Transmit(&huart3, (uint8_t*)RxData, 8, HAL_MAX_DELAY);
		sprintf(uart,"\r\n");
		HAL_UART_Transmit(&huart3,(uint8_t*)uart,strlen(uart),HAL_MAX_DELAY);*/


	// check Where are not outside the array
	if(Post <= CAN_BUF_SIZE)
	{

		//it the new incoming data[0] = to new data[0] following and we store the incoming data
		if (RxData[0] == new_data0 )
		{
			// increment new data[0]
			new_data0 = new_data0+1;
			// Can-but data is 8 bite long and we don't need the first one and store the rest
			for(int i =1; i<CANDATA_BUF_SIZE; i++)
			{
				// check if we have null character /0 if not store data and update our position
				if(RxData[i] != 0)
				{
					if(buffselect ==1){

						song[Post++] = RxData[i];
					}
					if(buffselect ==2)
					{
						song2[Post++] = RxData[i];
					}

				}
				else{

					if(buffselect ==1)
					{
						for(int i=0; i<Post; i++)
						{
							song[i] = song[i+1];
						}
					}

					if(buffselect ==2){
						for(int i=0; i<Post; i++){
							song2[i] = song2[i+1];
						}
					}
				}

  /* if(dataMach(song, detect_message,Post)&& finding ==1){
	   assigne[3] = next_controller;
	   next_controller++;
	   CAN2_Tx(assigne,id,8,1);
	   memset(song, 0, sizeof(song));

    }*/
			}

		}

		finding =0;
		//release the Semaphore to update the screen

		msgRdyFlag = 1;
	}


	if(RxData[4] ==0x19  &&RxData[3] == 0x99)
	{
		screenButtonClicked.Status.Volume = Rxdata[6];
		screenButtonClicked.Status.Volume2 = Rxdata[7];
	}

}


bool dataMach( uint8_t* Rxdata, uint8_t* data, int size)
{

	for(int i = 0; i < size; i++)
	{
		if(Rxdata[i] != data[i]){
			return false;
		}

	}
	return true;
}




