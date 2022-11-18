/*
 * sys_io.c
 *
 *  Created on: Sep 26, 2022
 *      Author: binh
 */
#include "main.h"
#include "adc.h"
#include "rtc.h"
#include "spi.h"

#include "aaa.h"
#include "sys_io.h"

#define GET_BIT(byte, index) ((byte >> index) & 0x01)

static uint16_t readADCChannel(ADC_HandleTypeDef *hadc, uint32_t channel) {
	uint16_t val;
	ADC_ChannelConfTypeDef sConfig =
		{ 0 };
	sConfig.Channel = channel;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
	HAL_ADC_ConfigChannel(hadc, &sConfig);
	HAL_ADC_Start(hadc);
	HAL_ADC_PollForConversion(hadc, 5);
	val = (uint16_t) HAL_ADC_GetValue(hadc);
	HAL_ADC_Stop(hadc);
	return val;
}

uint16_t readADC1Channel(uint32_t channel) {
	return readADCChannel(&hadc1, channel);
}

static uint16_t readADC3Channel(uint32_t channel) {
	return readADCChannel(&hadc3, channel);
}

static void muxINA(uint8_t port) {
	HAL_GPIO_WritePin(INA_S0_GPIO_Port, INA_S0_Pin, GET_BIT(port, 0) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(INA_S1_GPIO_Port, INA_S1_Pin, GET_BIT(port, 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(INA_S2_GPIO_Port, INA_S2_Pin, GET_BIT(port, 2) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void muxINB(uint8_t port) {
	HAL_GPIO_WritePin(INB_S0_GPIO_Port, INB_S0_Pin, GET_BIT(port, 0) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(INB_S1_GPIO_Port, INB_S1_Pin, GET_BIT(port, 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(INB_S2_GPIO_Port, INB_S2_Pin, GET_BIT(port, 2) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static DoorSwitchState_t adc2DoorSwitchState(uint16_t adcVal) {
	float vol = (float) adcVal * 3.3 / 4096.0;
	if (vol > 2.51) {
		return DOOR_SWITCH_DISCONN;
	}
	else if (vol > 0.86) {
		return DOOR_SWITCH_OPEN;
	}
	return DOOR_SWITCH_CLOSE;
}

DoorSwitchState_t getFrontDoor() {
	muxINA(0);
	vTaskDelay(1);
	uint16_t adcVal = readADC3Channel(ADC_CHANNEL_4);
	return adc2DoorSwitchState(adcVal);
}

DoorSwitchState_t getBehindDoor() {
	muxINA(1);
	vTaskDelay(1);
	uint16_t adcVal = readADC3Channel(ADC_CHANNEL_4);
	return adc2DoorSwitchState(adcVal);
}

DoorSwitchState_t getSafeBoxDoor() {
	muxINA(2);
	vTaskDelay(1);
	uint16_t adcVal = readADC3Channel(ADC_CHANNEL_4);
	return adc2DoorSwitchState(adcVal);
}

DoorSwitchState_t getBehindLightSwitch() {
	muxINA(4);

	vTaskDelay(1);
	uint16_t adcVal = readADC3Channel(ADC_CHANNEL_4);
	return adc2DoorSwitchState(adcVal);
}

DoorSwitchState_t getBehindFanSwitch() {
	muxINA(5);
	vTaskDelay(1);
	uint16_t adcVal = readADC3Channel(ADC_CHANNEL_4);
	return adc2DoorSwitchState(adcVal);
}

DoorSwitchState_t getAlarmSwitch() {
	muxINA(6);
	vTaskDelay(1);
	uint16_t adcVal = readADC3Channel(ADC_CHANNEL_4);
	return adc2DoorSwitchState(adcVal);
}

DoorSwitchState_t getFrontLightSwitch() {
	muxINA(7);
	vTaskDelay(1);
	uint16_t adcVal = readADC3Channel(ADC_CHANNEL_4);
	return adc2DoorSwitchState(adcVal);
}

static SirenState_t adc2SirenState(uint16_t adcVal) {
	float vol = (float) adcVal * 3.3 / 4096.0;
	if (vol > 3.05) {
		return SIREN_DISCONN;
	}
	else if (vol > 2.46) {
		return SIREN_CONN;
	}
	return SIREN_CLOSE;
}

static void ctrlDriver1(bool on) {
	HAL_GPIO_WritePin(SIN_OUT1_GPIO_Port, SIN_OUT1_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void ctrlDriver2(bool on) {
	HAL_GPIO_WritePin(SIN_OUT3_GPIO_Port, SIN_OUT3_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

SirenState_t getSiren1() {
	ctrlDriver1(false);
	muxINB(0);
	vTaskDelay(1);
	uint16_t adcVal = readADC3Channel(ADC_CHANNEL_5);
	return adc2SirenState(adcVal);
}

SirenState_t getSiren2() {
	ctrlDriver2(false);
	muxINB(2);
	vTaskDelay(1);
	uint16_t adcVal = readADC3Channel(ADC_CHANNEL_5);
	return adc2SirenState(adcVal);
}

void setSiren1(bool on) {
	ctrlDriver1(on);
}

void setSiren2(bool on) {
	ctrlDriver2(on);
}

static SmkState_t adc2SmkState(uint16_t adcVal) {
	float vol = (float) adcVal * 3.3 / 4096.0;
	if (vol > 2.51) {
		return SMK_DISCONN;
	}
	else if (vol > 0.86) {
		return SMK_NORMAL;
	}
	return SMK_WARNING;
}

static VibState_t adc2VibState(uint16_t adcVal) {
	float vol = (float) adcVal * 3.3 / 4096.0;
	if (vol > 2.51) {
		return VIB_DISCONN;
	}
	else if (vol > 0.86) {
		return VIB_WARNING;
	}
	return VIB_NORMAL;
}

void rebootSmokePwr() {
	HAL_GPIO_WritePin(SMK_OUT6_GPIO_Port, SMK_OUT6_Pin, GPIO_PIN_RESET);
	vTaskDelay(100);
	HAL_GPIO_WritePin(SMK_OUT6_GPIO_Port, SMK_OUT6_Pin, GPIO_PIN_SET);
}

void rebootVibratePwr() {
	HAL_GPIO_WritePin(VBR_OUT5_GPIO_Port, VBR_OUT5_Pin, GPIO_PIN_RESET);
	vTaskDelay(100);
	HAL_GPIO_WritePin(VBR_OUT5_GPIO_Port, VBR_OUT5_Pin, GPIO_PIN_SET);
}

SmkState_t getSmokeState() {
	muxINB(4);
	vTaskDelay(1);
	uint16_t adcVal = readADC3Channel(ADC_CHANNEL_5);
	return adc2SmkState(adcVal);
}

VibState_t getVibrateState() {
	muxINB(6);
	vTaskDelay(1);
	uint16_t adcVal = readADC3Channel(ADC_CHANNEL_5);
	return adc2VibState(adcVal);
}

uint16_t readADC_NTC1() {
	return readADC1Channel(ADC_CHANNEL_8);
}

uint16_t readADC_NTC2() {
	return readADC1Channel(ADC_CHANNEL_9);
}

float readVol_OVD() {
	uint16_t adcVal = readADC3Channel(ADC_CHANNEL_6);
	float vol = (float) adcVal * 3.3 / 4096.0;
	return vol * 57.0 / 10.0;
}

float readVol_Vin() {
	uint16_t adcVal = readADC3Channel(ADC_CHANNEL_7);
	float vol = (float) adcVal * 3.3 / 4096.0;
	return vol * 57.0 / 10.0;
}

uint16_t readADC_CT1() {
	return readADC1Channel(ADC_CHANNEL_3);
}

uint16_t readADC_CT2() {
	return readADC1Channel(ADC_CHANNEL_4);
}

uint16_t readADC_CT3() {
	return readADC1Channel(ADC_CHANNEL_5);
}

uint16_t readADC_CT4() {
	return readADC1Channel(ADC_CHANNEL_6);
}

void setRelayCMR1(bool on) {
	HAL_GPIO_WritePin(RL_CMR1_GPIO_Port, RL_CMR1_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void setRelayCMR2(bool on) {
	HAL_GPIO_WritePin(RL_CMR2_GPIO_Port, RL_CMR2_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void triggerLatch() {
	HAL_GPIO_WritePin(RL_LATCH_GPIO_Port, RL_LATCH_Pin, GPIO_PIN_SET);
	vTaskDelay(5);
	HAL_GPIO_WritePin(RL_LATCH_GPIO_Port, RL_LATCH_Pin, GPIO_PIN_RESET);
}

static bool relayOutputStorage[8] =
	{ false, false, false, false, false, false, false, false };
void setRelayOutput(uint8_t idx, bool on) {
	relayOutputStorage[idx] = on;
	switch (idx) {
	case 0:
		HAL_GPIO_WritePin(RL_OUT1_GPIO_Port, RL_OUT1_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
		break;
	case 1:
		HAL_GPIO_WritePin(RL_OUT2_GPIO_Port, RL_OUT2_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
		break;
	case 2:
		HAL_GPIO_WritePin(RL_OUT3_GPIO_Port, RL_OUT3_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
		break;
	case 3:
		HAL_GPIO_WritePin(RL_OUT4_GPIO_Port, RL_OUT4_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
		break;
	case 4:
		HAL_GPIO_WritePin(RL_OUT5_GPIO_Port, RL_OUT5_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
		break;
	case 5:
		HAL_GPIO_WritePin(RL_OUT6_GPIO_Port, RL_OUT6_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
		break;
	case 6:
		HAL_GPIO_WritePin(RL_OUT7_GPIO_Port, RL_OUT7_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
		break;
	case 7:
		HAL_GPIO_WritePin(RL_OUT8_GPIO_Port, RL_OUT8_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
		break;
	default:
		break;
	}
	triggerLatch();
}

bool getRelayOutput(uint8_t idx) {
	return relayOutputStorage[idx];
}

void setBuzzer(bool on) {
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void setLedLife(bool on) {
	HAL_GPIO_WritePin(LED_LIFE_GPIO_Port, LED_LIFE_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void toggleLedLife() {
	HAL_GPIO_TogglePin(LED_LIFE_GPIO_Port, LED_LIFE_Pin);
}

void setLedStatus(bool on) {
	HAL_GPIO_WritePin(LED_SST_GPIO_Port, LED_SST_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static bool ledOutputStorage[16] =
	{ false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false };
static bool ledOutputChangeFlag = false;
void setLedOutput(uint8_t idx, bool on) {
	ledOutputStorage[idx] = on;
	ledOutputChangeFlag = true;
}

bool getLedOutput(uint8_t idx) {
	return ledOutputStorage[idx];
}

void toggleLedOutput(uint8_t idx) {
	setLedOutput(idx, !getLedOutput(idx));
}

void shiftLedOutput() {
	if (ledOutputChangeFlag) {
		for (int i = 0; i < sizeof(ledOutputStorage) / sizeof(ledOutputStorage[0]); i++) {
			HAL_GPIO_WritePin(SER_IN_GPIO_Port, SER_IN_Pin, ledOutputStorage[i] ? GPIO_PIN_SET : GPIO_PIN_RESET);
			HAL_GPIO_WritePin(SRCLK_GPIO_Port, SRCLK_Pin, GPIO_PIN_SET);
			delayBlockingUs(1);
			HAL_GPIO_WritePin(SRCLK_GPIO_Port, SRCLK_Pin, GPIO_PIN_RESET);
		}
		HAL_GPIO_WritePin(RCLK_GPIO_Port, RCLK_Pin, GPIO_PIN_SET);
		delayBlockingUs(1);
		HAL_GPIO_WritePin(RCLK_GPIO_Port, RCLK_Pin, GPIO_PIN_RESET);
		ledOutputChangeFlag = false;
	}
}

static bool inputStorage[16] =
	{ false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false };
static bool inputICIsConnected() {
	bool firstStt = inputStorage[0];
	// Search any value difference with first value
	for (int i = 1; i < sizeof(inputStorage) / sizeof(inputStorage[0]); i++) {
		if (inputStorage[i] != firstStt) {
			return true;
		}
	}
	return false;
}
bool shiftInput() {
	HAL_GPIO_WritePin(SH_LD_GPIO_Port, SH_LD_Pin, GPIO_PIN_RESET);
	delayBlockingUs(2);
	HAL_GPIO_WritePin(SH_LD_GPIO_Port, SH_LD_Pin, GPIO_PIN_SET);
	for (int i = 0; i < sizeof(inputStorage) / sizeof(inputStorage[0]); i++) {
		HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_RESET);
		delayBlockingUs(2);
		inputStorage[i] = HAL_GPIO_ReadPin(SER_OUT_GPIO_Port, SER_OUT_Pin) == GPIO_PIN_SET;
		HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, GPIO_PIN_SET);
		delayBlockingUs(2);
	}
	return inputICIsConnected();
}

bool getInput(uint8_t idx) {
	return inputStorage[idx];
}

void setFlashSpiCS(bool on) {
	HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

uint8_t tranferFlashSpi(uint8_t send) {
	uint8_t recv = 0xFF;
	HAL_SPI_TransmitReceive(&hspi2, &send, &recv, 1, 100);
	return recv;
}

const char* getRstReason(bool clear) {
	char *res;
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)) {
		res = "POR/PDR";
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) {
		res = "SOFT";
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) {
		res = "IWDG";
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)) {
		res = "WWDG";
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)) {
		res = "LPWR";
	}
	else {
		res = "PIN";
	}

	if (clear) {
		__HAL_RCC_CLEAR_RESET_FLAGS();
	}

	return res;
}

static void hwEthernetPower(int stt) {
	if (stt) {
		HAL_GPIO_WritePin(ETH_PWR_GPIO_Port, ETH_PWR_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(ETH_NRST_GPIO_Port, ETH_NRST_Pin, GPIO_PIN_SET);
	}
	else {
		HAL_GPIO_WritePin(ETH_PWR_GPIO_Port, ETH_PWR_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(ETH_NRST_GPIO_Port, ETH_NRST_Pin, GPIO_PIN_RESET);
	}
}
void hwEthernetReboot() {
	hwEthernetPower(0);
	delayBlocking(1000);
	hwEthernetPower(1);
}

void setLcdRst(bool on) {
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void setLcdCs(bool on) {
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void setLcdBl(bool on) {
	HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void tranferLcdData(uint8_t data) {
	for (uint8_t i = 0; i < 8; i++) {
		HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_SET);
		delayBlockingUs(1);
		HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_RESET);
		delayBlockingUs(1);
		data = data << 1;
	}
}

bool meBoxIsOpen() {
	return HAL_GPIO_ReadPin(BOX_PG7_GPIO_Port, BOX_PG7_Pin) == GPIO_PIN_SET;
}

/*
 * System core function
 */

void sysReset() {
	NVIC_SystemReset();
}

void setTimestamp(uint32_t sec) {
	RTC_TimeTypeDef sTime =
		{ 0 };
	RTC_DateTypeDef sDate =
		{ 0 };
	time_t ts = (time_t) sec;
	struct tm *ti = gmtime(&ts);

	sTime.Hours = (uint8_t) ti->tm_hour;
	sTime.Minutes = (uint8_t) ti->tm_min;
	sTime.Seconds = (uint8_t) ti->tm_sec;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	sDate.WeekDay = RTC_WEEKDAY_MONDAY;
	sDate.Month = (uint8_t) ti->tm_mon + 1;
	sDate.Date = (uint8_t) ti->tm_mday;
	sDate.Year = (uint8_t) ti->tm_year - 100;
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	printf("system time updated: %s\n", ctime(&ts));
}

void getTimestamp(uint32_t *sec) {
	RTC_TimeTypeDef sTime =
		{ 0 };
	RTC_DateTypeDef sDate =
		{ 0 };
	time_t rS;
	struct tm ti =
		{ 0 };

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	ti.tm_year = (int) sDate.Year + 100;
	ti.tm_mon = (int) sDate.Month - 1;
	ti.tm_mday = (int) sDate.Date;
	ti.tm_hour = (int) sTime.Hours;
	ti.tm_min = (int) sTime.Minutes;
	ti.tm_sec = (int) sTime.Seconds;
	rS = mktime(&ti);
	*sec = (uint32_t) rS;
}

void delayBlockingUs(uint32_t us) {
	volatile uint32_t countUs = us;
	while (countUs--) {
		volatile uint64_t wait1us = 8;
		while (wait1us--) {
			;
		}
	}
}

void delayBlocking(uint32_t ms) {
	volatile uint32_t countMs = ms;
	while (countMs--) {
		volatile uint64_t wait1ms = 16000;
		while (wait1ms--) {
			;
		}
	}
}

void blinkLedLife(uint32_t ms) {
	toggleLedLife();
	delayBlocking(ms);
}

void blinkLedLife12(uint32_t ms1, uint32_t ms2) {
	setLedLife(false);
	delayBlocking(ms1);
	setLedLife(true);
	delayBlocking(ms2);
}

void blinkLedLifeNonBlock(uint32_t ms) {
	toggleLedLife();
	vTaskDelay(ms);
}
