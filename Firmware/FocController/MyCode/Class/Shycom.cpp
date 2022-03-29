#include <FOC/FocController.h>
#include "Shycom.h"

extern FocController focController;

Shycom::Shycom(USART_TypeDef* UartInstance, int32_t baud) : HardwareSerial(UartInstance, baud) {
	// TODO Auto-generated constructor stub
	for(int i=0; i<=15; i++)
		this->Param_Len[i] = 4;
	this->Param_Len[16] = 16;
	this->Param_Len[17] = 8;
	this->Param_Len[32] = 6;
	this->Param_Len[33] = 6;
}
Shycom::~Shycom() {
	// TODO Auto-generated destructor stub
}

void Shycom::DoProtocol(){
	int len = this->GetAvailable();
	/** 针对不同的协议，这里要采取不同的舍弃数据的策略，有可能会出现死锁现象*/
	if((this->Protocol_Buffer_Len + len) >= PROTOCOL_BUFFER_SIZE)
		len = PROTOCOL_BUFFER_SIZE - this->Protocol_Buffer_Len;
	this->Protocol_Buffer_Len += this->Read(&this->Protocol_Buffer[this->Protocol_Buffer_Len], len);

	/** 协议解析*/
	if(this->Protocol_Buffer_Len <= 7) return;
	for(int i=0; i < this->Protocol_Buffer_Len-7; i++){
		if(this->Protocol_Buffer[i] == 0x5A && this->Protocol_Buffer[i+1] == 0xA5){
			uint8_t byte_len = this->Protocol_Buffer[i+2];
			if(byte_len+i <= this->Protocol_Buffer_Len){
				if(this->Protocol_Buffer[i+byte_len-2] == 0x7A && this->Protocol_Buffer[i+byte_len-1] == 0xA7){
					int index = i+3;
					while(index < i+byte_len-3){
						switch(this->Protocol_Buffer[index]){
							case 33:
								uint8_t  mot, par;
								float gain;
								mot = this->Protocol_Buffer[index+1];
								par = this->Protocol_Buffer[index+2];
								memcpy(&gain, &this->Protocol_Buffer[index+3], 4);
								switch(par){
//									case 0: focController.CloseLoopSpeedTarget.Speed = gain; break;
//									case 1: focController.PidSpeed.Kp = gain; break;
//									case 2: focController.PidSpeed.Ki = gain; break;
//									case 3: focController.PidSpeed.Kd = gain; break;
//									case 4: focController.PidSpeed.Limit_Integral = gain;
//											focController.PidSpeed.Limit_Result = gain; break;
								case 0: focController.CloseLoopPositionTarget.Position = gain; break;
								case 1: focController.PidPosition.Kp = gain; break;
								case 2: focController.PidPosition.Ki = gain; break;
								case 3: focController.PidPosition.Kd = gain; break;
								case 4: focController.PidPosition.Limit_Integral = gain;
										focController.PidPosition.Limit_Result = gain; break;
								}
							break;
						}
						index += this->Param_Len[this->Protocol_Buffer[index]] + 1;
					}
					memcpy(this->Protocol_Buffer, &this->Protocol_Buffer[i + byte_len], this->Protocol_Buffer_Len - byte_len - i);
					this->Protocol_Buffer_Len = this->Protocol_Buffer_Len - byte_len - i;
					break;
				}
			}
		}
	}

	/** 缓存区满了就舍弃一半数据，防止因协议无法解析而产生死锁的一种方式*/
	if(this->Protocol_Buffer_Len >= PROTOCOL_BUFFER_SIZE){
		this->Protocol_Buffer_Len = PROTOCOL_BUFFER_SIZE / 2;
		memcpy(this->Protocol_Buffer, &this->Protocol_Buffer[this->Protocol_Buffer_Len], this->Protocol_Buffer_Len);
	}
}

void Shycom::ReportCurveValue(const float *value, uint8_t len)
{
	uint8_t buf[86];
	buf[0] = 0x5A;  buf[1] = 0xA5;
	buf[2] = 5 * len + 6;
	buf[5*len + 4] = 0x7A;
	buf[5*len + 5] = 0xA7;

	for(int i=0; i<len; i++){
		buf[5*i + 3] = i;
		memcpy(&buf[5*i + 4], &value[i], 4);
	}

	buf[5*len + 3] = 0;
	for(int i=3; i <= 5*len + 2; i++)
		buf[5*len + 3] += buf[i];
	for(int i=0; i <= 5*len + 5; i++)
		this->OneWrite(buf[i]);
}
