#include "aaa.h"

#include "app.h"
#include "app_log.h"
#include "task_list.h"
#include "task_console.h"
#include "Internet.h"
#include "MqttConnection.h"

#include "flash.h"

#include "dhcp.h"

#include "sys_io.h"

/*****************************************************************************/
/*  local declare
 */
/*****************************************************************************/
#define STR_LIST_MAX_SIZE		10
#define STR_BUFFER_SIZE			128

static char cmd_buffer[STR_BUFFER_SIZE];
static char *str_list[STR_LIST_MAX_SIZE];
static uint8_t str_list_len;

static uint8_t str_parser(char *str);
static char* str_parser_get_attr(uint8_t);

/*****************************************************************************/
/*  command function declare
 */
/*****************************************************************************/
static int32_t console_reset(uint8_t *argv);
static int32_t console_ver(uint8_t *argv);
static int32_t console_help(uint8_t *argv);
static int32_t console_reboot(uint8_t *argv);
static int32_t console_fatal(uint8_t *argv);
static int32_t console_dbg(uint8_t *argv);
static int32_t console_sensor(uint8_t *argv);
static int32_t console_relay(uint8_t *argv);
static int32_t console_net(uint8_t *argv);
/*****************************************************************************/
/*  command table
 */
/*****************************************************************************/
cmd_line_t lgn_cmd_table[] =
	{

	/*************************************************************************/
	/* system command */
	/*************************************************************************/
		{ (const int8_t*) "reset", console_reset, (const int8_t*) "reset terminal" },
		{ (const int8_t*) "ver", console_ver, (const int8_t*) "version info" },
		{ (const int8_t*) "help", console_help, (const int8_t*) "help command info" },
		{ (const int8_t*) "reboot", console_reboot, (const int8_t*) "reboot system" },
		{ (const int8_t*) "fatal", console_fatal, (const int8_t*) "fatal cmd" },

	/*************************************************************************/
	/* debug command */
	/*************************************************************************/
		{ (const int8_t*) "dbg", console_dbg, (const int8_t*) "debug cmd" },
		{ (const int8_t*) "sensor", console_sensor, (const int8_t*) "sensor cmd" },
		{ (const int8_t*) "relay", console_relay, (const int8_t*) "relay cmd" },
		{ (const int8_t*) "net", console_net, (const int8_t*) "network cmd" },

	/* End Of Table */
		{ (const int8_t*) 0, (pf_cmd_func) 0, (const int8_t*) 0 } };

uint8_t str_parser(char *str) {
	UNUSED(str);
	strcpy(cmd_buffer, str);
	str_list_len = 0;

	uint8_t i = 0;
	uint8_t str_list_index = 0;
	uint8_t flag_insert_str = 1;

	while (cmd_buffer[i] != 0 && cmd_buffer[i] != '\n' && cmd_buffer[i] != '\r') {
		if (cmd_buffer[i] == ' ') {
			cmd_buffer[i] = 0;
			flag_insert_str = 1;
		}
		else if (flag_insert_str) {
			str_list[str_list_index++] = &cmd_buffer[i];
			flag_insert_str = 0;
		}
		i++;
	}

	cmd_buffer[i] = 0;

	str_list_len = str_list_index;
	return str_list_len;
}

char* str_parser_get_attr(uint8_t index) {
	UNUSED(index);
	if (index < str_list_len) {
		return str_list[index];
	}
	return NULL;
}

/*****************************************************************************/
/*  command function definaion
 */
/*****************************************************************************/
int32_t console_reset(uint8_t *argv) {
	(void) argv;
	SYS_PRINTF("\033[2J\r");
	return 0;
}

int32_t console_ver(uint8_t *argv) {
	(void) argv;
	SYS_PRINTF("-Manufacturer:     %s\n", APP_MANUFACTURER);
	SYS_PRINTF("-Model:            %s\n", APP_MODEL);
	SYS_PRINTF("-Firmware:         %s\n", APP_FIRMWARE);
	SYS_PRINTF("-Firmware version: %s\n", APP_FW_VERSION);
	SYS_PRINTF("-Uptime:           %lu\n", HAL_GetTick());
	SYS_PRINTF("-Heap free:        %u\n", xPortGetFreeHeapSize());
	return 0;
}

int32_t console_help(uint8_t *argv) {
	uint32_t idx = 0;
	switch (*(argv + 5)) {
	default:
		SYS_PRINTF("\nCOMMANDS INFORMATION:\n\n");
		while (lgn_cmd_table[idx].cmd != (const int8_t*) 0) {
			SYS_PRINTF("%s\n\t%s\n\n", lgn_cmd_table[idx].cmd, lgn_cmd_table[idx].info);
			idx++;
		}
		break;
	}
	return 0;
}

int32_t console_reboot(uint8_t *argv) {
	(void) argv;
	sysReset();
	return 0;
}

int32_t console_fatal(uint8_t *argv) {
	fatalLog_t fatalLog;
	switch (*(argv + 6)) {
	case 'r':
		memset(&fatalLog, 0, sizeof(fatalLog));
		fatalLog.magicNumber = FATAL_LOG_MAGIC_NUMBER;
		flash_erase_sector(FATAL_LOG_FLASH_ADDRESS);
		flash_write(FATAL_LOG_FLASH_ADDRESS, (uint8_t*) &fatalLog, sizeof(fatalLog));
		SYS_PRINTF("fatal cleared\n");
		break;

	case 'l':
		flash_read(FATAL_LOG_FLASH_ADDRESS, (uint8_t*) &fatalLog, sizeof(fatalLog));
		fatalLog.string[sizeof(fatalLog.string) - 1] = 0;
		fatalLog.rstReason[sizeof(fatalLog.rstReason) - 1] = 0;
		SYS_PRINTF("-String:        %s\n", fatalLog.string);
		SYS_PRINTF("-Restart times: %lu\n", fatalLog.restartTimes);
		SYS_PRINTF("-Uptime:        %lu\n", fatalLog.uptime);
		SYS_PRINTF("-RST Reason:    %s\n", fatalLog.rstReason);
		break;

	case 't':
		FATAL("TEST01");
		break;

	default:
		SYS_PRINTF("Usage: fatal [OPTION]\n");
		SYS_PRINTF("\tr clear fatal infos\n");
		SYS_PRINTF("\tl display fatal infos\n");
		SYS_PRINTF("\tt fatal TEST01\n");
		break;
	}
	return 0;
}

int32_t console_dbg(uint8_t *argv) {
	(void) argv;
	switch (*(argv + 4)) {
	case '2':
		AAATaskPostMsg(AAA_TASK_CONSOLE_ID, CONSOLE_READ_NTC, NULL, 0);
		break;
	case '3':
		AAATaskPostMsg(AAA_TASK_CONSOLE_ID, CONSOLE_READ_CT, NULL, 0);
		break;
	case '4':
		AAATaskPostMsg(AAA_TASK_CONSOLE_ID, CONSOLE_SEND_TEST, NULL, 0);
		break;
	case '5': {
		uint32_t wword = 0xA1B2C3D4;
		uint32_t rword = 0;
		uint8_t stt = flash_is_connected();
		SYS_PRINTF("flash_is_connected(): %s\n", stt == FLASH_DRIVER_OK ? "true" : "false");
		flash_read(0x0, (uint8_t*) &rword, 4);
		SYS_PRINTF("rword: x%08x\n", rword);
		flash_erase_sector(0x0);
		flash_write(0x0, (uint8_t*) &wword, 4);
		SYS_PRINTF("wword: x%08x\n", wword);
	}
		break;

	case '8':
		for (int i = 0; i < 16; i++) {
			setLedOutput(i, true);
			shiftLedOutput();
			vTaskDelay(100);
		}
		SYS_PRINTF("set led on\n");
		break;
	case '9':
		for (int i = 0; i < 16; i++) {
			setLedOutput(i, false);
			shiftLedOutput();
			vTaskDelay(100);
		}
		SYS_PRINTF("set led off\n");
		break;

	case 't':
		for (int i = 0; i < 16; i++) {
			toggleLedOutput(i);
			shiftLedOutput();
			vTaskDelay(100);
		}
		SYS_PRINTF("toggle led\n");
		break;

	default: {
		time_t now = time(0);
		struct tm *timeinfo = localtime(&now);
		SYS_PRINTF("time(): %lu, asctime(): %s\n", (uint32_t) now, asctime(timeinfo));
	}
		break;
	}
	return 0;
}

int32_t console_sensor(uint8_t *argv) {
	(void) argv;
	return 0;
}

int32_t console_relay(uint8_t *argv) {
	(void) argv;
	return 0;
}

int32_t console_net(uint8_t *argv) {
	(void) argv;
	extern struct netif gnetif;
	switch (*(argv + 4)) {
	case 'i':
		if (Internet.connected()) {
			SYS_PRINTF("My ip: %s\n", ip4addr_ntoa(netif_ip4_addr(&gnetif)));
		}
		else {
			SYS_PRINTF("Internet is disconnected\n");
			SYS_PRINTF("netif link: %s\n", netif_is_link_up(&gnetif) ? "up" : "down");
			SYS_PRINTF("netif: %d\n", netif_is_up(&gnetif) ? "up" : "down");
		}
		break;

	case 'u':
		netif_set_up(&gnetif);
		SYS_PRINTF("set if up\n");
		break;

	case 'd':
		netif_set_down(&gnetif);
		SYS_PRINTF("set if down\n");
		break;

	case 'f':
		dhcp_stop(&gnetif);
		ip_addr_set_zero_ip4(&gnetif.ip_addr);
		ip_addr_set_zero_ip4(&gnetif.netmask);
		ip_addr_set_zero_ip4(&gnetif.gw);
		SYS_PRINTF("dhcp stopped\n");
		break;

	case 'o':
		if (ERR_OK != dhcp_start(&gnetif)) {
			SYS_PRINTF("dhcp start fail\n");
		}
		else {
			SYS_PRINTF("dhcp start successful\n");
		}
		break;

	case 's': {
		if (str_parser((char*) argv) == 5) {
			char *ips = str_parser_get_attr(2);
			char *masks = str_parser_get_attr(3);
			char *gws = str_parser_get_attr(4);

			if (strlen(ips) >= 7 && strlen(masks) >= 7 && strlen(gws) >= 7) {
				ip4_addr_t ip, mask, gw;
				if (ip4addr_aton(ips, &ip) && ip4addr_aton(masks, &mask) && ip4addr_aton(gws, &gw)) {
					gAppSetting.useStaticIP = true;
					gAppSetting.staticIp = ip;
					gAppSetting.subnetMask = mask;
					gAppSetting.gateway = gw;
					flash_erase_sector(APP_SETTING_FLASH_ADDRESS);
					flash_write(APP_SETTING_FLASH_ADDRESS, (uint8_t*) &gAppSetting, sizeof(gAppSetting));
					SYS_PRINTF("app settings written\n");

					dhcp_stop(&gnetif);
					SYS_PRINTF("dhcp stopped\n");

					Internet.setStaticIP(ip, mask, gw);
					SYS_PRINTF("set static ip: %s %s %s\n", ip4addr_ntoa(&ip), ip4addr_ntoa(&mask), ip4addr_ntoa(&gw));
				}
			}
		}
	}
		break;

	case 'e': {
		gAppSetting.useStaticIP = false;
		flash_erase_sector(APP_SETTING_FLASH_ADDRESS);
		flash_write(APP_SETTING_FLASH_ADDRESS, (uint8_t*) &gAppSetting, sizeof(gAppSetting));
		SYS_PRINTF("app settings written\n");

		if (ERR_OK != dhcp_start(&gnetif)) {
			SYS_PRINTF("dhcp start fail\n");
		}
		else {
			SYS_PRINTF("dhcp start successful\n");
		}
	}
		break;

	default:
		SYS_PRINTF("Usage: net [OPTION] [IP] [MASK] [GW]\n");
		SYS_PRINTF("\ti infos\n");
		SYS_PRINTF("\tu up\n");
		SYS_PRINTF("\td down\n");
		SYS_PRINTF("\tf dhcp stop\n");
		SYS_PRINTF("\to dhcp start\n");
		SYS_PRINTF("\ts disable dhcp, set static ip\n");
		SYS_PRINTF("\te set enable dhcp\n");
		SYS_PRINTF("Exam: net s 192.168.0.9 255.255.255.0 192.168.0.1\n");
		break;
	}
	return 0;
}
