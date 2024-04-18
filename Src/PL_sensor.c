
#include  "adc.h"
#include "PL_sensor.h"
#include "stm32l4xx_it.h"


uint8_t AD_step;

uint16_t count =0;

uint16_t g_ADCBuffer[5];
uint16_t g_sensor_on[4];
uint16_t g_sensor_off[4];
uint16_t g_sensor[4];
uint16_t V_battAD;
uint32_t j;
uint16_t sensor_stack[4][4];
void pl_callback_getSensor(void) {
	HAL_ADC_Stop_DMA(&hadc1);
    switch (AD_step) {
        case 0:
            HAL_GPIO_WritePin(SENSORLED_1_GPIO_Port, SENSORLED_1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(SENSORLED_2_GPIO_Port, SENSORLED_2_Pin, GPIO_PIN_RESET);
            for (j = 0; j <= 500; j++) {}
            break;
        case 1:
            g_sensor_off[0] = g_ADCBuffer[1];
            g_sensor_off[1] = g_ADCBuffer[2];
            g_sensor_on[2] = g_ADCBuffer[3];
            g_sensor_on[3] = g_ADCBuffer[4];
            HAL_GPIO_WritePin(SENSORLED_1_GPIO_Port, SENSORLED_1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(SENSORLED_2_GPIO_Port, SENSORLED_2_Pin, GPIO_PIN_SET);
            for (j = 0; j <= 500; j++) {}
            break;
        case 2:
            g_sensor_on[0] = g_ADCBuffer[1];
            g_sensor_on[1] = g_ADCBuffer[2];
            g_sensor_off[2] = g_ADCBuffer[3];
            g_sensor_off[3] = g_ADCBuffer[4];
            HAL_GPIO_WritePin(SENSORLED_1_GPIO_Port, SENSORLED_1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(SENSORLED_2_GPIO_Port, SENSORLED_2_Pin, GPIO_PIN_SET);
            for (j = 0; j <= 10; j++) {}
            break;
	    }

    V_battAD = g_ADCBuffer[0];
    g_V_batt = 3.3 * (float)V_battAD / 1023.0 * (100.0 + 22.0) / 22.0;
    AD_step++;
    if (AD_step != 3) {
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_ADCBuffer, sizeof(g_ADCBuffer) / sizeof(uint16_t));
    } else {
        AD_step = 0;
    }
}

void pl_interupt_getSensor(void) {
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_ADCBuffer, sizeof(g_ADCBuffer) / sizeof(uint16_t));
    for(int i= 0;i<4;i++){
    	if(g_sensor_on[i]>=g_sensor_off[i])
        	g_sensor[i] = g_sensor_on[i]-g_sensor_off[i];
    }
    sensor_update();
    Sensor_Average();
}

void sensor_update() {
    for (int i = 1; i >= 0; i--) { // センサースタック回数を2回に減らす
        for (int j = 0; j < 4; j++) {
            sensor_stack[i][j] = sensor_stack[i - 1][j];
        }
    }

    for (int j = 0; j < 4; j++) {
        sensor_stack[0][j] = g_sensor[j];
    }
}

void Sensor_Average() {
    for (int i = 0; i < 4; i++) {
        average_sensor[i] = (sensor_stack[0][i] + sensor_stack[1][i]) / 2; // センサースタック回数を2回に変更したので、2で割る
    }
}

float pl_getbatt(void){
	float batt;
	uint16_t battAD;

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1,50);
	battAD = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	batt= 3.3 *(float)battAD /1023.0*(100+22.0)/22.0;

	return batt;
}



