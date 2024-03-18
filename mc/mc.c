#include "mc.h"
#include <stm32g4xx_ll_gpio.h>
#include <stm32g4xx_ll_tim.h>
#include <stdio.h>
#include <string.h>
#include "tim.h"

#define UH	0
#define UL	1
#define VH	2
#define VL	3
#define WH	4
#define WL	5
// #define BOTTOM_KEYS

uint8_t BLDC_STATE_PREV[6] = {0,0,0,0,0,0};
// BLDC motor steps tables
static const uint8_t BLDC_BRIDGE_STATE_FORWARD[8][6] =   // Motor steps
{
//	UH,UL		VH,VL	WH,WL
   { 0,0	,	0,0	,	0,0 },  // 0 //000

   { 0,1	,	0,0	,	1,0 },
   { 1,0	,	0,1	,	0,0 },
   { 0,0	,	0,1	,	1,0 },
   { 0,0	,	1,0	,	0,1 },
   { 0,1	,	1,0	,	0,0 },
   { 1,0	,	0,0	,	0,1 },

   { 0,0	,	0,0	,	0,0 },  // 0 //111
};

uint8_t BLDC_STATE[6] = {0,0,0,0,0,0};

void BLDC_set_pwm( uint16_t pwm ){
    TIM1->CCR1 = pwm;
    TIM1->CCR2 = pwm;
    TIM1->CCR3 = pwm;
}

void BLDC_MotorStop(void){
    BLDC_set_pwm( 0 );
    LL_TIM_CC_DisableChannel( TIM1, LL_TIM_CHANNEL_CH1 );
	LL_TIM_CC_DisableChannel( TIM1, LL_TIM_CHANNEL_CH2 );
	LL_TIM_CC_DisableChannel( TIM1, LL_TIM_CHANNEL_CH3 );

	LL_TIM_CC_DisableChannel( TIM1, LL_TIM_CHANNEL_CH1N );
	LL_TIM_CC_DisableChannel( TIM1, LL_TIM_CHANNEL_CH2N );
	LL_TIM_CC_DisableChannel( TIM1, LL_TIM_CHANNEL_CH3N );
}

uint16_t read_hall_pos(){
//    pHandle->HallState  = (uint8_t)((LL_GPIO_IsInputPinSet(pHandle->H3Port, pHandle->H3Pin) << 2U)
//                                      | (LL_GPIO_IsInputPinSet(pHandle->H2Port, pHandle->H2Pin) << 1U)
//                                      | LL_GPIO_IsInputPinSet(pHandle->H1Port, pHandle->H1Pin));
//    
    /* Get Current Hall Sensor Position */
    uint8_t hallpos = ((GPIOB->IDR & 0b111000000) >> 6);
    // printf(">> st:%u\r\n", hallpos);
    return hallpos;
}


#ifdef stimpl
#define STEP_1  0U + 1
#define STEP_2  1U + 1
#define STEP_3  2U + 1
#define STEP_4  3U + 1
#define STEP_5  4U + 1
#define STEP_6  5U + 1
#define CntPh 5000

void BLDC_MotorCommutation(uint16_t hallpos){
    
    static uint16_t hp = 0;
    TIM_TypeDef * TIMx = TIM1;

    switch ( hallpos )
      {
        case STEP_1:
        {
          LL_TIM_OC_SetCompareCH1 ( TIMx, (uint32_t)CntPh );
          LL_TIM_OC_SetCompareCH2 ( TIMx, 0 );
          LL_TIM_OC_SetCompareCH3 ( TIMx, 0 );
          LL_TIM_CC_EnableChannel( TIMx, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N | LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH2N);
          LL_TIM_CC_DisableChannel( TIMx, LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH3N );
        }
          break;
        case STEP_2:
        {
          LL_TIM_OC_SetCompareCH1 ( TIMx, (uint32_t)CntPh );
          LL_TIM_OC_SetCompareCH2 ( TIMx, 0 );
          LL_TIM_OC_SetCompareCH3 ( TIMx, 0 );
          LL_TIM_CC_EnableChannel( TIMx, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N | LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH3N );
          LL_TIM_CC_DisableChannel( TIMx, LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH2N );
        }
          break;
        case STEP_3:
        {
          LL_TIM_OC_SetCompareCH1 ( TIMx, 0 );
          LL_TIM_OC_SetCompareCH2 ( TIMx, (uint32_t)CntPh );
          LL_TIM_OC_SetCompareCH3 ( TIMx, 0 );
          LL_TIM_CC_EnableChannel( TIMx, LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH2N | LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH3N );
          LL_TIM_CC_DisableChannel( TIMx, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N );
        }
          break;
        case STEP_4:
        {
          LL_TIM_OC_SetCompareCH1 ( TIMx, 0 );
          LL_TIM_OC_SetCompareCH2 ( TIMx, (uint32_t)CntPh );
          LL_TIM_OC_SetCompareCH3 ( TIMx, 0 );
          LL_TIM_CC_EnableChannel( TIMx, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N | LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH2N );
          LL_TIM_CC_DisableChannel( TIMx, LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH3N );
        }
          break;
        case STEP_5:
        {
          LL_TIM_OC_SetCompareCH1 ( TIMx, 0 );
          LL_TIM_OC_SetCompareCH2 ( TIMx, 0 );
          LL_TIM_OC_SetCompareCH3 ( TIMx, (uint32_t)CntPh );
          LL_TIM_CC_EnableChannel( TIMx, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N | LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH3N );
          LL_TIM_CC_DisableChannel( TIMx, LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH2N );
        }
          break;
        case STEP_6:
        {
          LL_TIM_OC_SetCompareCH1 ( TIMx, 0 );
          LL_TIM_OC_SetCompareCH2 ( TIMx, 0 );
          LL_TIM_OC_SetCompareCH3 ( TIMx, (uint32_t)CntPh );
          LL_TIM_CC_EnableChannel( TIMx, LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH2N | LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH3N );
          LL_TIM_CC_DisableChannel( TIMx, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N );
        }
          break;
      }

}

#elif defined BOTTOM_KEYS
void BLDC_MotorCommutation(uint16_t hallpos)
{
	
	memcpy(BLDC_STATE, BLDC_BRIDGE_STATE_FORWARD[hallpos], sizeof(BLDC_STATE));
	

	// Disable if need
	// if (!BLDC_STATE[UH]) TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable); 
  if (!BLDC_STATE[UH]) LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH1);
	// if (!BLDC_STATE[UL]) TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
  if (!BLDC_STATE[UL]) LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH1N);

	// if (!BLDC_STATE[VH]) TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
	if (!BLDC_STATE[VH]) LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH2);

	// if (!BLDC_STATE[VL]) TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
  if (!BLDC_STATE[VL]) LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH2N);

	// if (!BLDC_STATE[WH]) TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
  if (!BLDC_STATE[WH]) LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH3);

	// if (!BLDC_STATE[WL]) TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);
	if (!BLDC_STATE[WL]) LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH3N);

	// Enable if need. If previous state is Enabled then not enable again. Else output do flip-flop.
	if (BLDC_STATE[UH] & !BLDC_STATE[UL] & !BLDC_STATE_PREV[UH]) LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
	if (BLDC_STATE[UL] & !BLDC_STATE[UH] & !BLDC_STATE_PREV[UL]) LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1N);
	if (BLDC_STATE[VH] & !BLDC_STATE[VL] & !BLDC_STATE_PREV[VH]) LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2);
	if (BLDC_STATE[VL] & !BLDC_STATE[VH] & !BLDC_STATE_PREV[VL]) LL_TIM_CC_EnableChannel(TIM1,  LL_TIM_CHANNEL_CH2N);
	if (BLDC_STATE[WH] & !BLDC_STATE[WL] & !BLDC_STATE_PREV[WH]) LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);
	if (BLDC_STATE[WL] & !BLDC_STATE[WH] & !BLDC_STATE_PREV[WL]) LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3N);

	memcpy(BLDC_STATE_PREV, BLDC_STATE, sizeof(BLDC_STATE));
}

#elif defined BOTTOM_TOP_COMPL
void BLDC_MotorCommutation(uint16_t hallpos){
  
	memcpy(BLDC_STATE, BLDC_BRIDGE_STATE_FORWARD[hallpos], sizeof(BLDC_STATE));
	

	// PWM at low side FET of bridge U
	// active freewheeling at high side FET of bridge U
	// if low side FET is in PWM off mode then the hide side FET
	// is ON for active freewheeling. This mode needs correct definition
	// of dead time otherwise we have shoot-through problems

	if (BLDC_STATE[UH]) {
		// TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_OCMode_PWM1);
    LL_TIM_OC_SetMode(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1 );
		// TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH1N );
		// TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);
    
	} else {
		// Low side FET: OFF
		// TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
    LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH1);    

		if (BLDC_STATE[UL]){
			// High side FET: ON
			// TIM_SelectOCxM(TIM1, TIM_Channel_1, TIM_ForcedAction_Active);
      LL_TIM_OC_SetMode(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_FORCED_ACTIVE );
			// TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);
      LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1N );
		} else {
			// High side FET: OFF
			// TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
      LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH1N);
		}
	}

	if (BLDC_STATE[VH]) {
		// TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_OCMode_PWM1);
    LL_TIM_OC_SetMode(TIM1, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_PWM1 );
		// TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);
		// TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH2N );
	} else {
		// Low side FET: OFF
		// TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
    LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH2);

		if (BLDC_STATE[VL]){
			// High side FET: ON
			// TIM_SelectOCxM(TIM1, TIM_Channel_2, TIM_ForcedAction_Active);
      LL_TIM_OC_SetMode(TIM1, LL_TIM_CHANNEL_CH2N, LL_TIM_OCMODE_FORCED_ACTIVE );
			// TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
      LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2N );
		} else {
			// High side FET: OFF
			LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH2N );
		}
	}

	if (BLDC_STATE[WH]) {
		// TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_OCMode_PWM1);
    LL_TIM_OC_SetMode(TIM1, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_PWM1 );
		// TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);
		// TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH3N );
	} else {
		// Low side FET: OFF
		// TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
    LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH3 );

		if (BLDC_STATE[WL]){
			// High side FET: ON
			// TIM_SelectOCxM(TIM1, TIM_Channel_3, TIM_ForcedAction_Active);
      LL_TIM_OC_SetMode(TIM1, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_FORCED_ACTIVE );
			// TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);
      LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3N );
		} else {
			// High side FET: OFF
			LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH3N );
		}
	}
}
#else
// HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_1);
//HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
void BLDC_MotorCommutation(uint16_t hallpos){
  memcpy(BLDC_STATE, BLDC_BRIDGE_STATE_FORWARD[hallpos], sizeof(BLDC_STATE));
	

	// PWM at low side FET of bridge U
	// active freewheeling at high side FET of bridge U
	// if low side FET is in PWM off mode then the hide side FET
	// is ON for active freewheeling. This mode needs correct definition
	// of dead time otherwise we have shoot-through problems

	if (BLDC_STATE[UH]) {
		HAL_TIM_PWM_Start( &htim1, TIM_CHANNEL_1);
	} else {
		HAL_TIM_PWM_Stop( &htim1, TIM_CHANNEL_1);

		if (BLDC_STATE[UL]){
			HAL_TIMEx_PWMN_Start( &htim1, TIM_CHANNEL_1);
		} else {
			// High side FET: OFF
			// TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
      HAL_TIMEx_PWMN_Stop( &htim1, TIM_CHANNEL_1);
		}
	}

	if (BLDC_STATE[VH]) {
		HAL_TIM_PWM_Start( &htim1, TIM_CHANNEL_2);
	} else {
		// Low side FET: OFF
		// TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
    HAL_TIM_PWM_Stop( &htim1, TIM_CHANNEL_2);

		if (BLDC_STATE[VL]){
			HAL_TIMEx_PWMN_Start( &htim1, TIM_CHANNEL_2);
		} else {
			// High side FET: OFF
			HAL_TIMEx_PWMN_Stop( &htim1, TIM_CHANNEL_2);
		}
	}

	if (BLDC_STATE[WH]) {
		HAL_TIM_PWM_Start( &htim1, TIM_CHANNEL_3);
	} else {
		// Low side FET: OFF
		HAL_TIM_PWM_Stop( &htim1, TIM_CHANNEL_3);

		if (BLDC_STATE[WL]){
			HAL_TIMEx_PWMN_Start( &htim1, TIM_CHANNEL_3);
		} else {
			// High side FET: OFF
			HAL_TIMEx_PWMN_Stop( &htim1, TIM_CHANNEL_3);
		}
	}

//  LL_TIM_GenerateEvent_COM( TIM1 );
//  LL_TIM_GenerateEvent_UPDATE( TIM1 );
}
#endif


