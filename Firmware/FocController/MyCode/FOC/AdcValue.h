#ifndef FOC_ADCVALUE_H_
#define FOC_ADCVALUE_H_

#include "adc.h"

#define  ADC2VOLTAGE	 0.00008056640625  // adcValue/4096*3.3
#define  ADC2AMPS	     0.0008056640625   // adcValue/4096*3.3/0.1

class AdcValue {
public:
	AdcValue();
	virtual ~AdcValue();

	AdcValue(ADC_HandleTypeDef *adc1, ADC_HandleTypeDef *adc2, ADC_HandleTypeDef *adc3 = NULL);
	void AdcInit(ADC_HandleTypeDef *adc1, ADC_HandleTypeDef *adc2, ADC_HandleTypeDef *adc3 = NULL);

	void Start(){HAL_ADCEx_InjectedStart_IT(this->Adc1);};
	void Stop(){HAL_ADCEx_InjectedStop_IT(this->Adc1);};

	ADC_HandleTypeDef *Adc1, *Adc2, *Adc3;

	struct{
		float iu = 0;
		float iv = 0;
		float iw = 0;
	}OffsetValue;

	struct{
		float iu = 0;
		float iv = 0;
		float iw = 0;
	}Value;

	void Offset(int cnt = 1000);
	void Update(void);

};

#endif /* FOC_ADCVALUE_H_ */
