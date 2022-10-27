# AAAOS Base STM32
STM32CubeIDE V1.10.1
FreeRTOS
STM32F407

# Fix CubeIDE Generator
file: etherneif.c
line: 764
from: HAL_ETH_Start(&heth);
to: HAL_ETH_Start_IT(&heth);
