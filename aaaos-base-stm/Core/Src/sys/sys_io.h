/*
 * sys_io.h
 *
 *  Created on: Sep 26, 2022
 *      Author: binh
 */

#ifndef SRC_SYS_SYS_IO_H_
#define SRC_SYS_SYS_IO_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ADC1_INA3  -> CT1
 * ADC1_INA4  -> CT2
 * ADC1_INA5  -> CT3
 * ADC1_INA6  -> CT4
 * ADC1_INA8  -> NTC1
 * ADC1_INA9  -> NTC2
 *
 * ADC3_INA4  -> MUX_INA
 * ADC3_INA5  -> MUX_INB
 * ADC3_INA6  -> OVD (13.5V)
 * ADC3_INA7  -> VIN (12V)
 * ADC3_INA14 -> INA9
 * ADC3_INA15 -> INA10
 * */

/*
Door1		Close/Open/Disconnected		Trạng thái cảm biến cửa trước
Door2		Close/Open/Disconnected		Trạng thái cảm biến cửa sau
Door3		Close/Open/Disconnected		Trạng thái cảm biến cửa két
Door4		Close/Open/Disconnected		Dự phòng
Switch1		Off/On/Disconnected			Bybass đèn phòng sau
Switch2		Off/On/Disconnected			Bybass quạt phòng sau
Switch3		Off/On/Disconnected			Bật/Tắt báo động trước khi mở két
Switch4		Off/On/Disconnected			Bybass đèn phía trước
Switch5		Off/On/Disconnected			Dự phòng
Switch6		Off/On/Disconnected			Dự phòng
Siren1		Connected/Disconnected		Trạng thái còi báo động
Siren2		Connected/Disconnected		Dự phòng
Vibration	Normal/Warning/Disconnected	Trạng thái cảm biến rung chuyển
Smoke		Normal/Warning/Disconnected	Trạng thái cảm biến khói
Thermistor1	28.5						Giá trị cảm biến nhiệt độ 1
Thermistor2	28.5						Giá trị cảm biến nhiệt độ 2
MainVoltage	12.5						Giá trị điện áp từ bộ chuyển điện lưới AC (V)
AccuVoltage	12.5						Giá trị điện áp từ Accu (V)
CT1			120.5						Giá trị dòng tải 1 (mA)
CT2			120.5						Giá trị dòng tải 2
CT3			120.5						Giá trị dòng tải 3
CT4			120.5						Giá trị dòng tải 4
*/

#define millis()		HAL_GetTick()

enum eLedIdx {
	RL4_LED_IDX,
	RL3_LED_IDX,
	RL2_LED_IDX,
	RL1_LED_IDX,
	AUTO_LED_IDX,
	ALARM_LED_IDX,
	MANUAL_LED_IDX,
	OFF_LED_IDX,
	SWITCH_LED_IDX,
	AC_LED_IDX,
	BAT_LED_IDX,
	TEMP_LED_IDX,
	DOOR_LED_IDX,
	SMK_LED_IDX,
	VIB_LED_IDX,
	SIREN_LED_IDX,
};

enum eBtnIdx {
	UP_BTN_IDX,
	RIGHT_BTN_IDX,
	SET_BTN_IDX,
	LEFT_BTN_IDX,
	AUTO_BTN_IDX,
	ALARM_BTN_IDX,
	MANUAL_BTN_IDX,
	OFF_BTN_IDX,
	NOD_USED0,
	NOD_USED1,
	NOD_USED2,
	RL4_BTN_IDX,
	RL3_BTN_IDX,
	RL2_BTN_IDX,
	RL1_BTN_IDX,
	DOWN_BTN_IDX,
};

typedef enum eDoorState {
	DOOR_SWITCH_CLOSE,
	DOOR_SWITCH_OPEN,
	DOOR_SWITCH_DISCONN,
	DOOR_SWITCH_INIT = 255
} DoorSwitchState_t;

typedef enum eSirenState {
	SIREN_CONN,
	SIREN_DISCONN,
	SIREN_CLOSE,
	SIREN_ACTIVE,
	SIREN_INIT = 255
} SirenState_t;

typedef enum eVibState {
	VIB_NORMAL,
	VIB_WARNING,
	VIB_DISCONN,
	VIB_INIT = 255
} VibState_t;

typedef enum eSmkState {
	SMK_NORMAL,
	SMK_WARNING,
	SMK_DISCONN,
	SMK_INIT = 255
} SmkState_t;

extern DoorSwitchState_t getFrontDoor();
extern DoorSwitchState_t getBehindDoor();
extern DoorSwitchState_t getSafeBoxDoor();
extern DoorSwitchState_t getBehindLightSwitch();
extern DoorSwitchState_t getBehindFanSwitch();
extern DoorSwitchState_t getAlarmSwitch();
extern DoorSwitchState_t getFrontLightSwitch();
extern SirenState_t getSiren1();
extern SirenState_t getSiren2();
extern void setSiren1(bool on);
extern void setSiren2(bool on);
extern void rebootSmokePwr();
extern void rebootVibratePwr();
extern SmkState_t getSmokeState();
extern VibState_t getVibrateState();
extern uint16_t readADC1Channel(uint32_t channel);
extern uint16_t readADC_NTC1();
extern uint16_t readADC_NTC2();
extern float readVol_OVD();
extern float readVol_Vin();
extern uint16_t readADC_CT1();
extern uint16_t readADC_CT2();
extern uint16_t readADC_CT3();
extern uint16_t readADC_CT4();
extern void setRelayCMR1(bool on);
extern void setRelayCMR2(bool on);
extern void setRelayOutput(uint8_t idx, bool on);
extern bool getRelayOutput(uint8_t dix);
#define setBehindLightRelay(o) setRelayOutput(3,o)
#define setBehindFanRelay(o) setRelayOutput(5,o)
#define setFrontLightRelay(o) setRelayOutput(1,o)
#define getBehindLightRelay() getRelayOutput(3)
#define getBehindFanRelay() getRelayOutput(5)
#define getFrontLightRelay() getRelayOutput(1)
extern void setBuzzer(bool on);
extern void setLedLife(bool on);
extern void toggleLedLife();
extern void setLedStatus(bool on);
extern void setLedOutput(uint8_t idx, bool on);
extern bool getLedOutput(uint8_t idx);
extern void toggleLedOutput(uint8_t idx);
extern void shiftLedOutput();
extern void shiftInput();
extern bool getInput(uint8_t idx);
extern void setFlashSpiCS(bool on);
extern uint8_t tranferFlashSpi(uint8_t send);
extern const char* getRstReason(bool clear);
extern void hwEthernetReboot();
extern void setLcdRst(bool on) ;
extern void setLcdCs(bool on);
extern void setLcdBl(bool on);
extern void tranferLcdData(uint8_t data);
extern bool meBoxIsOpen();
/*
 * System core function
 */
extern void sysReset();
extern void setTimestamp(uint32_t sec);
extern void getTimestamp(uint32_t *sec);
extern void delayBlockingUs(uint32_t us);
extern void delayBlocking(uint32_t ms);
extern void blinkLedLife(uint32_t ms);
extern void blinkLedLife12(uint32_t ms1, uint32_t ms2);
extern void blinkLedLifeNonBlock(uint32_t ms);
extern void setTimestamp(uint32_t sec);
extern void getTimestamp(uint32_t *sec);
extern void setLedLife(bool on);
extern void toggleLedLife();

#ifdef __cplusplus
}
#endif

#endif /* SRC_SYS_SYS_IO_H_ */
