#include <FOC/AdcValue.h>

AdcValue::AdcValue() {
	// TODO Auto-generated constructor stub
//	this->Adc1 = &hadc1;
//	this->Adc2 = &hadc2;
//	this->Adc3 = &hadc3;
}

AdcValue::~AdcValue() {
	// TODO Auto-generated destructor stub
}

AdcValue::AdcValue(ADC_HandleTypeDef *adc1, ADC_HandleTypeDef *adc2, ADC_HandleTypeDef *adc3){
	this->Adc1 = adc1;
	this->Adc2 = adc2;
	this->Adc3 = adc3;
}

void AdcValue::AdcInit(ADC_HandleTypeDef *adc1, ADC_HandleTypeDef *adc2, ADC_HandleTypeDef *adc3){
	this->Adc1 = adc1;
	this->Adc2 = adc2;
	this->Adc3 = adc3;
}

void AdcValue::Offset(int cnt){
	unsigned int iu = 0, iv = 0, iw = 0;

	if(this->Adc3){
		HAL_ADC_Start(this->Adc1);
		HAL_ADC_Start(this->Adc2);
		HAL_ADC_Start(this->Adc3);

		for(int i=0; i<cnt; i++){
			iu += HAL_ADCEx_InjectedGetValue(this->Adc1, ADC_INJECTED_RANK_1);
			iv += HAL_ADCEx_InjectedGetValue(this->Adc2, ADC_INJECTED_RANK_1);
			iw += HAL_ADCEx_InjectedGetValue(this->Adc3, ADC_INJECTED_RANK_1);
			HAL_Delay(1);
		}
		this->OffsetValue.iu = (float)iu / cnt * ADC2AMPS;
		this->OffsetValue.iv = (float)iv / cnt * ADC2AMPS;
		this->OffsetValue.iw = (float)iw / cnt * ADC2AMPS;
	}else{
		HAL_ADC_Start(this->Adc1);
		HAL_ADC_Start(this->Adc2);

		for(int i=0; i<cnt; i++){
			iu += HAL_ADCEx_InjectedGetValue(this->Adc1, ADC_INJECTED_RANK_1);
			iv += HAL_ADCEx_InjectedGetValue(this->Adc2, ADC_INJECTED_RANK_1);
			HAL_Delay(1);
		}
		this->OffsetValue.iu = (float)iu / cnt * ADC2AMPS;
		this->OffsetValue.iv = (float)iv / cnt * ADC2AMPS;
		this->OffsetValue.iw = 0;
	}

//	HAL_ADCEx_InjectedStart_IT(&hadc1);
}

void AdcValue::Update(void){
	unsigned int iu = 0, iv = 0, iw = 0;

	if(this->Adc3){
		iu = HAL_ADCEx_InjectedGetValue(this->Adc1, ADC_INJECTED_RANK_1);
		iv = HAL_ADCEx_InjectedGetValue(this->Adc2, ADC_INJECTED_RANK_1);
		iw = HAL_ADCEx_InjectedGetValue(this->Adc3, ADC_INJECTED_RANK_1);

		this->Value.iu = this->OffsetValue.iu - iu * ADC2AMPS;
		this->Value.iv = this->OffsetValue.iv - iv * ADC2AMPS;
		this->Value.iw = this->OffsetValue.iw - iw * ADC2AMPS;
	}else{
		iu = HAL_ADCEx_InjectedGetValue(this->Adc1, ADC_INJECTED_RANK_1);
		iv = HAL_ADCEx_InjectedGetValue(this->Adc2, ADC_INJECTED_RANK_1);

		this->Value.iu = this->OffsetValue.iu - iu * ADC2AMPS;
		this->Value.iv = this->OffsetValue.iv - iv * ADC2AMPS;
		this->Value.iw = 0 - this->Value.iu - this->Value.iv;
	}
}
