/*
 * BQ25731.h
 *
 *  Created on: Nov 13, 2023
 *      Author: Loc
 */

#ifndef INC_BQ25731_H_
#define INC_BQ25731_H_
#include "main.h"
#include <stdint.h>




#define CHARGE_OPTION_0_REG                0x00 /*Charge Option 0*/
#define CHARGE_CURRENT_REG                 0x02 /*7-bit charge current setting LSB 128 mA, Range 0 mA – 16256 mA (RSR=5mΩ)*/
#define CHARGE_VOLTAGE_REG                 0x04 /*12-bit charge voltage setting LSB 8 mV, Default: 1S-4200mV, 2S-8400mV, 3S-12600mV, 4S-16800mV, 5S-21000mV*/
#define OTG_VOLTAGE_REG                    0x06 /*12-bit OTG voltage setting LSB 8 mV, Range: 3000 mV – 24000 mV*/
#define OTG_VOLTAGE_CURRENT_REG            0x08 /*7-bit OTG output current setting LSB 100 mA, Range: 0 A – 12700 mA(RAC=5mΩ)*/
#define INPUT_VOLTAGE_REG                  0x0A /*8-bit input voltage setting LSB 64 mV, Range: 3200 mV – 19520 mV*/
#define INPUT_CURRENT_LIMIT_HOST_REG       0x0E /*6-bit Input current limit set by hostLSB: 100-mA, Range: 100 mA - 10000 mA with 100 mA offset (RAC=5mΩ)*/
#define CHARGE_STATUS_REG                  0x20 /*Charger Status*/
#define PROCHOT_STATUS_REG                 0x22 /*Prochot status*/
#define INPUT_CURRENT_LIMIT_USE_REG        0x24 /*7-bit input current limit in use LSB: 50 mA, Range: 100 mA - 10000 mA (RAC=5mΩ) */
#define ADC_VBUS_PSYS_REG                  0x26 /*8-bit digital output of input voltage, 8-bit digital output of system power PSYS: Full range: 3.06 V, LSB: 12 mV VBUS: Full range: 0 V - 24.48 V, LSB 96 mV*/
#define ADC_BAT_CHARGE_DISCHARGE_CURRENT_REG  0x28 /*7-bit digital output of battery charge current, 7-bit digital output of battery discharge current ICHG: Full range 16.256 A, LSB 128 mA
                                                   IDCHG: Full range: 65.024 A, LSB: 512 mA (RSR=5mΩ)*/
#define ADC_INPUT_CURRENT_CMPIN_VOLTAGE_REG   0x2A	/* 8-bit digital output of input current, 8-bit digital output of CMPIN voltage
                                                    POR State - IIN: Full range: 25.5 A, LSB 100 mA (RAC=5mΩ) CMPIN: Full range 3.06 V, LSB: 12 mV*/
#define ADC_SYS_AND_BAT_VOLTAGE_REG           0x2C /*8-bit digital output of system voltage, 8-bit digital output of battery voltage VSYS: Full range: 2.88 V - 19.2 V, LSB: 64 mV (1S-4S)
                                                   VSYS: Full range: 8.16 V - 24.48 V, LSB: 64 mV (5S) VBAT: Full range : 2.88 V - 19.2 V, LSB 64 mV (1S-4S) VBAT: Full range : 8.16 V - 24.48 V, LSB 64 mV (5S)*/
#define MANUFATURER_ID_REG                    0x2E /*Manufacturer ID - 0x0040H*/
#define DEVICE_ID_REG                         0x2F /*Device ID*/
#define CHARGE_OPTION_1_REG                   0x30 /*Charge Option 1*/
#define CHARGE_OPTION_2_REG                   0x32 /*Charge Option 2*/
#define CHARGE_OPTION_3_REG                   0x34 /*Charge Option 3*/
#define PROCHOT_OPTION_0_REG                  0x36 /*PROCHOT Option 0*/
#define PROCHOT_OPTION_1_REG                  0x38 /*PROCHOT Option 1*/
#define ADC_OPTION_REG                        0x3A /*ADC Option*/
#define ADC_OPTION_4_REG                      0x3C /*ADC Option 4*/
#define VMIN_ACTIVE_PROTECT_REG               0x3E /*Vmin Active Protection */


/*ChargeOption0 Register (I2C address = 01/00h) [reset = E70Eh]*/
/*ChargeOption0 Register (I2C address = 01h) */
#define EN_LPWR_BIT                          0x80 /*defautlt 1b Low Power Mode Enable, under low power mode lowest quiescent current is
achieved when only battery exist. It is not recommended to enable low power
mode when adapter present.
0b: Disable Low Power Mode. Device in performance mode with battery only.
The PROCHOT, current/power monitor buffer and comparator follow register
setting.
1b: Enable Low Power Mode. Device in low power mode with battery only
for lowest quiescent current. The REGN is off. The PROCHOT, discharge
current monitor buffer, power monitor buffer and independent comparator are
disabled. ADC is not available in Low Power Mode. Independent comparator
and its low power mode PROCHOT profile can be enabled by setting
EN_PROCHOT_LPWR bit to 1b. <default at POR> */
#define WDTMR_ADJ_ENABLE_5S_BIT              0x20
#define WDTMR_ADJ_ENABLE_88S_BIT             0x40
#define WDTMR_ADJ_ENABLE_175S_BIT            0x60
/*default: 11b WATCHDOG Timer Adjust
Set maximum delay between consecutive Host write of charge voltage or
charge current command.
If device does not receive a write on the REG0x15() or the REG0x14()
within the watchdog time period, the charger will be suspended by setting
the REG0x14() to 0 mA 256 mA (BQ25731).
After expiration, the timer will resume upon the write of REG0x14(),
REG0x15() or REG0x12[14:13].
00b: Disable Watchdog Timer
01b: Enabled, 5 sec
10b: Enabled, 88 sec
11b: Enable Watchdog Timer, 175 sec <default at POR*/
#define IIN_DPM_AUTO_DISABLE_BIT             0x10 /*default 0b IIN_DPM Auto Disable
When CELL_BATPRESZ pin is LOW, the charger automatically disables the
IIN_DPM function by setting EN_IIN_DPM (REG0x12[1]) to 0. The host can
enable IIN_DPM function later by writing EN_IIN_DPM bit (REG0x12[1]) to 1.
0b: Disable this function. IIN_DPM is not disabled when CELL_BATPRESZ
goes LOW. <default at POR>
1b: Enable this function. IIN_DPM is disabled when CELL_BATPRESZ goes
LOW.*/
#define OTG_ON_CHRGOK_ENABLE_BIT            0x08 /*default: 0b Add OTG to CHRG_OK
Drive CHRG_OK to HIGH when the device is in OTG mode.
0b: Disable <default at POR>
1b: Enable*/
#define EN_OOA_BIT                          0x04 /*default 1b Out-of-Audio Enable
In both forward mode and OTG mode, switching frequency reduces with
diminishing load, under extreme light load condition the switching frequency
could be lower than 25 kHz which is already in audible frequency range. By
configuring EN_OOA=1b, the minimum PFM burst frequency is clamped at
around 25 kHz to avoid any audible noise.
0b: No limit of PFM burst frequency
1b: Set minimum PFM burst frequency to above 25 kHz to avoid audio noise
<default at POR>
*/
#define PWM_FREQ_400KHZ_BIT                  0x02
/*default 1b Switching Frequency Selection: Recommend 800 kHz with 2.2 µH, and 400
kHz with 4.7 µH.
0b: 800kHz
1b: 400 kHz<default at POR>*/
#define LOW_PTM_RIPPLE_BIT                   0x01 /*default 1b PTM mode input voltage and current ripple reduction
0b: Disable
1b: Enable <default at POR>*/

/*. ChargeOption0 Register (I2C address = 00h)*/

#define EN_CMP_LATCH_BIT                    0x80 /*default 0 The EN_CMP_LATCH bit, will latch the independent comparator output after
it is triggered at low state. If enabled in PROCHOT profile REG34H[6]=1 ,
STAT_COMP bit REG0x21[6] keep 1b after triggered until read by host and
clear
0b: Independent comparator output will not latch when it is low<default at
POR>
1b: Independent comparator output will latch when it is low, host can clear
CMPOUT pin by toggling this REG0x12[7] bit*/
#define VSYS_UVP_DISABLE_BIT                0x40 /*default 0b
To disable system under voltage protection.
0b: VSYS under voltage protection is enabled <default at POR>
1b: VSYS under voltage protection is disabled*/
#define EN_LEARN_BIT                        0x20 /*default 0b LEARN mode allows the battery to discharge and converter to shut off while
the adapter is present . It calibrates the battery gas gauge over a complete
discharge/charge cycle. When the host determines the battery voltage is
below battery depletion threshold, the host switch the system back to adapter
input by writing this bit back to 0b.
0b: Disable LEARN Mode <default at POR>
1b: Enable LEARN Mode*/
#define IADPT_GAIN_40X_BIT                 0x10 /*default 0b IADPT Amplifier Ratio
The ratio of voltage on IADPT and voltage across ACP and ACN.
0b: 20× <default at POR>
1b: 40× */
#define IBAT_GAIN_16X_BIT                  0x08 /*default 1b IBAT Amplifier Ratio
The ratio of voltage on IBAT and voltage across SRP and SRN
0b: 8×
1b: 16× <default at POR>*/
#define EN_IIN_DPM_BIT                     0x02 /*default 1b IIN_DPM Enable
Host writes this bit to enable IIN_DPM regulation loop. When the IIN_DPM
is disabled by the charger (refer to IIN_DPM_AUTO_DISABLE), this bit goes
LOW.
0b: IIN_DPM disabled
1b: IIN_DPM enabled <default at POR>*/
#define CHRG_INHIBIT_BIT                   0x01 /*default 0b Charge Inhibit
When this bit is 0, battery charging will start with valid values in the
ChargeVoltage() register and the ChargeCurrent register.
0b: Enable Charge <default at POR>
1b: Inhibit Charge*/

/*ChargeCurrent Register (I2C address = 03/02h) [reset = 0000h] */
/*0 = Adds 0 mA of charger current.
1 = Adds x mA of charger current.*/
/*Charge Current Register with 5-mΩ Sense Resistor (I2C address = 03h)*/
#define CHARGE_CURRENT_8192_MA_BIT             0x10
#define CHARGE_CURRENT_4096_MA_BIT             0x08
#define CHARGE_CURRENT_2048_MA_BIT             0x04
#define CHARGE_CURRENT_1024_MA_BIT             0x02
#define CHARGE_CURRENT_512_MA_BIT              0x01
/*. Charge Current Register with 5-mΩ Sense Resistor (I2C address = 02h)*/
#define CHARGE_CURRENT_256_MA_BIT              0x80
#define CHARGE_CURRENT_128_MA_BIT              0x40

/*ChargeVoltage Register (I2C address = 05/04h) [reset value based on CELL_BATPRESZ pin setting] */
/*ChargeVoltage Register (I2C address = 05h)*/
/*0 = Adds 0 mV of charger voltage.
1 = Adds x mV of charger voltage.*/
#define CHARGE_VOLTAGE_16384_MV_BIT            0x40
#define CHARGE_VOLTAGE_8192_MV_BIT             0x20
#define CHARGE_VOLTAGE_4096_MV_BIT             0x10
#define CHARGE_VOLTAGE_2048_MV_BIT             0x08
#define CHARGE_VOLTAGE_1024_MV_BIT             0x04
#define CHARGE_VOLTAGE_512_MV_BIT              0x02
#define CHARGE_VOLTAGE_256_MV_BIT              0x01
/*ChargeVoltage Register (I2C address = 04h)*/
#define CHARGE_VOLTAGE_128_MV_BIT              0x80
#define CHARGE_VOLTAGE_64_MV_BIT               0x40
#define CHARGE_VOLTAGE_32_MV_BIT               0x20
#define CHARGE_VOLTAGE_16_MV_BIT               0x10
#define CHARGE_VOLTAGE_8_MV_BIT                0x08

/*ChargerStatus Register (I2C address = 21/20h) [reset = 0000h]*/
/*ChargerStatus Register (I2C address = 21h)*/
#define STAT_AC_BIT                            0x80 /*default 0 Input source status. STAT_AC is valid as long as VBUS go within
3.5-V to 26-V range. It is different from CHRG_OK bit, When
CHRG_OK is valid, STAT_AC must be valid, but if STAT_AC is valid,
it is not necessary CHRG_OK is valid. There are Force converter
off, ACOC, TSHUT , SYSOVP, VSYS_UVP, BATOVP can pull low
CHRG_OK.
0b: Input not present
1b: Input is present */
#define ICO_DONE_BIT                          0x40/*default 0 After the ICO routine is successfully executed, the bit goes 1.
0b: ICO is not complete
1b: ICO is complete*/
#define IN_VAP_BIT                            0x20/*default 0b 0b: Charger is not operated in VAP mode
1b: Charger is operated in VAP mode
Digital status bit indicates VAP has enabled(1) or disabled(0). The
enable of VAP mode only follows the host command, which is not
blocked by any status of /PROCHOT. The exit of VAP mode also
follows the host command, except that any faults will exit VAP mode
automatically. STAT_EXIT_VAP (REG0x21[8]) becomes 1 which will
pull low /PROCHOT until host clear.
The host can enable VAP by setting OTG/VAP/FRS pin high and
0x32[5]=0, disable VAP by setting either OTG/VAP/FRS pin low
or 0x32[5]=1. Any faults in VAP When IN_VAP bit goes 0->1,
charger should disable VINDPM, IIN_DPM, ICRIT, ILIM pin, disable
PP_ACOK if it is enabled, enable PP_VSYS if it is disabled. When
IN_VAP bit goes 1->0, charger should enable VINDPM, IIN_DPM,
ICRIT, ILIM pin function.
*/
#define IN_VINDMP_BIT                         0x10 /*default 0b: Charger is not in VINDPM during forward mode, or voltage
regulation during OTG mode
1b: Charger is in VINDPM during forward mode, or voltage
regulation during OTG mode
*/
#define IN_IIN_DPM_BIT                        0x08 /*default 0b: Charger is not in IIN_DPM during forward mode.
1b: Charger is not in IIN_DPM during forward mode.*/
#define IN_FCHRG_BIT                              0x04 /*default 0b: Charger is not in fast charge
1b: Charger is in fast charger*/
#define IN_OTG_BIT                            0x01 /*default 0b: Charger is not in OTG
1b: Charge is in OTG*/

/*ChargerStatus Register (I2C address = 20h)*/
#define FAULT_ACOV_BIT                        0x80 /*default 0b ChargerStatus Register (I
2C address = 20h)*/
#define FAULT_BATOC_BIT                       0x40 /*default 0b The status is latched if triggered until a read from host. Fault
indicator for BATOC only during normal operation. However, in PTM
mode when EN_BATOC=1b, this status bit is fault indicator for both
BATOVP and BATOC; when EN_BATOC=0b, this status bit is not
effective.
0b: No fault
1b: BATOC is triggered*/
#define FAULT_ACOC_BIT                       0x20 /*default 0b The status is latched if triggered until a read from host.
0b: No fault
1b: ACOC*/
#define FAULT_SYSOVP_BIT                     0x10 /*default 0b SYSOVP Status and Clear. SYSOVP fault is latched until a clear
from host by writing this bit to 0.
When the SYSOVP occurs, this bit is HIGH. During the SYSOVP, the
converter is disabled.
After the SYSOVP is removed, the user must write a 0 to this bit
or unplug the adapter to clear the SYSOVP condition to enable the
converter again.
0b: Not in SYSOVP <default at POR>
1b: In SYSOVP. When SYSOVP is removed, write 0 to clear the
SYSOVP latch*/
#define FAULT_VSYS_UVP_BIT                   0x08 /*default 0b VSYS_UVP fault status and clear. VSYS_UVP fault is latched until a
clear from host by writing this bit to 0.
0b: No fault <default at POR>
1b: When system voltage is lower than VSYS_UVP, then 7 times
restart tries are failed.
*/
#define FAULT_FORCE_CONVERTER_OFF_BIT       0x04 /*defaut 0bThe status is latched if triggered until a read from host.
0b: No fault
1b: Force converter off triggered (when FORCE_CONV_OFF
(REG0x30[3])=1b)*/
#define FAULT_OTG_OVP                       0x02 /*default 0b The status is latched if triggered until a read from host.
0b: No fault
1b: OTG OVP fault is triggered*/
#define FAULT_OTG_UVP                       0x01 /*default 0b The status is latched if triggered until a read from host.
0b: No fault
1b: OTG UVP fault is triggered*/
/*ProchotStatus Register (I2C address = 23/22h)*/
/*. ProchotStatus Register (I2C address = 23h)*/
#define EN_PROCHOT_EXT_BIT                 0x40 /*default 0b PROCHOT Pulse Extension Enable. When pulse extension is
enabled, keep the PROCHOT pin voltage LOW until host writes
PROCHOT _CLEAR = 0b.
0b: Disable pulse extension <default at POR>
1b: Enable pulse extension*/
#define PROCHOT_WIDTH_BIT                  0x20 /*default 0b PROCHOT Pulse Width Minimum PROCHOT pulse width when
EN_PROCHOT _EXT = 0b
00b: 100 us
01b: 1 ms
10b: 5 ms
11b: 10 ms <default at POR>*/
#define PROCHOT_CLEAR_BIT                  0x08 /*default 0b PROCHOT Pulse Clear.
Clear PROCHOT pulse when EN_PROCHOT _EXT = 1b.
0b: Clear PROCHOT pulse and drive PROCHOT pin HIGH
1b: Idle <default at POR>*/
#define TSHUT_BIT                          0x04 /*default 0b TSHUT trigger:
0b: TSHUT is not triggered
1b: TSHUT is triggered*/
#define STAT_VAP_FAIL_BIT                  0x02 /*default 0b This status bit reports a failure to load VBUS 7 consecutive times
in VAP mode, which indicates the battery voltage might be not
high enough to enter VAP mode, or the VAP loading current
settings are too high.
0b: Not is VAP failure <default at POR>
1b: In VAP failure, the charger exits VAP mode, and latches off
until the host writes this bit to 0.*/
#define STAT_EXIT_VAP_BIT                  0x01 /*default 0b When the charger is operated in VAP mode, it can exit VAP
by either being disabled through host, or there are ACOV/ACOC/
SYSOVP/BATOVP/VSYS_UVP faults.
0b: PROCHOT_EXIT_VAP is not active <default at POR>
1b: PROCHOT_EXIT_VAP is active, PROCHOT pin is low until
host writes this status bit to 0.*/
/*ProchotStatus Register (I2C address = 22h)*/

#define STAT_VINDPM_BIT                    0x80 /*default 0b PROCHOT Profile VINDPM status bit
0b: Not triggered
1b: Triggered, PROCHOT pin is low until host writes this status bit
to 0 when PP_VINDPM = 1b*/
#define STAT_COMP_BIT                      0x40 /*default 0b PROCHOT Profile CMPOUT status bit. The status is latched until
a read from host.
0b: Not triggered
1b: Triggered*/
#define STAT_ICRIT_BIT                     0x20 /*default 0b PROCHOT Profile ICRIT status bit. The status is latched until a
read from host.
0b: Not triggered
1b: Triggered*/
#define STAT_INOM_BIT                      0x10 /*default 0b PROCHOT Profile INOM status bit. The status is latched until a
read from host.
0b: Not triggered
1b: Triggered*/
#define STAT_IDCHG1_BIT                    0x08 /*default 0b PROCHOT Profile IDCHG1 status bit. The status is latched until a
read from host.
0b: Not triggered
1b: Triggered*/
#define STAT_VSYS_BIT                      0x04 /*default 0b PROCHOT Profile VSYS status bit. The status is latched until a
read from host.
0b: Not triggered
1b: Triggered*/
#define STAT_BATTERY_REMOVAL_BIT           0x02 /*default 0b PROCHOT Profile Battery Removal status bit. The status is
latched until a read from host.
0b: Not triggered
1b: Triggered*/
#define STAT_ADAPTER_REMOVAL_BIT           0x01 /*default 0b PROCHOT Profile Adapter Removal status bit. The status is
latched until a read from host.
0b: Not triggered
1b: Triggered*/

/*ChargeOption1 Register (I2C address = 31/30h) [reset = 3300h]*/

#define EN_IBAT_BIT                       0x80 /*default 0b IBAT Enable
Enable the IBAT output buffer. In low power mode (EN_LWPWR=1b), IBAT
buffer is always disabled regardless of this bit value.
0b Turn off IBAT buffer to minimize Iq <default at POR>
1b: Turn on IBAT buffer*/
#define EN_PROCHOT_LPWR_BIT               0x40 /*default 0b Enable PROCHOT during battery only low power mode
With battery only, enable VSYS in PROCHOT with low power consumption. Do
not enable this function with adapter present. Refer to Section 9.3.20.1 for more
details.
0b: Disable Independent Comparator low power PROCHOT <default at POR>
1b: Enable Independent Comparator low power PROCHOT*/
#define PSYS_CONFIG_BIT                   0x20

/*default 11b PSYS Enable and Definition Register
Enable PSYS sensing circuit and output buffer (whole PSYS circuit). In low
power mode (EN_LWPWR=1b), PSYS sensing and buffer are always disabled
regardless of this bit value.
00b: PSYS=PBUS+PBAT
01b: PSYS=PBUS
10b: Reserved
11b: Turn off PSYS buffer to minimize Iq<default at POR>*/
#define RSNS_RAC_BIT                     0x08 /*default 1b Input sense resistor RAC
0b: 10 mΩ
1b: 5 mΩ <default at POR>*/
#define RSNS_RSR_BIT                     0x04 /*default 1b Charge sense resistor RSR
0b: 10 mΩ
1b: 5 mΩ <default at POR>*/
#define PSYS_RATIO_BIT                   0x02 /*default 1b PSYS Gain
Ratio of PSYS output current vs total system power
0b: 0.25 µA/W
1b: 1 µA/W <default at POR>*/
#define EN_FAST_5MOHM_BIT                0x01 /*default 1b Enable fast compensation to increase bandwidth under 5 mΩ RAC
(RSNS_RAC=1b) for input current up to 6.4-A application (the fast
compensation will only work when IADPT pin is configured less than 160 kΩ)
0b: Turn off bandwidth promotion under RSNS_RAC=1b
(Note when this bit configured as 0b, IIN_HOST DAC can be extended up to
10 A, writing IIN_HOST value higher than 10 A will be neglected, the ICHG
regulation loop will be slower to guarantee stability under 6.4-A to 10-A input
current range)
1b: Turn on bandwidth promotion under RSNS_RAC=1b <default at POR>
(Note when this bit configured as 1b, IIN_HOST DAC is clamped at 6.4 A,
writing IIN_HOST value higher than 6.4 A will be neglected, the ICHG regulation
loop will be faster within 6.4-A input current range)*/


/*CChargeOption1 Register (I2C address = 30h)*/
#define CMP_REF_BIT                      0x80 /*default 0b Independent Comparator internal Reference
0b: 2.3 V <default at POR>
1b: 1.2 V*/
#define CMP_POL_BIT                      0x40 /*default 0b Independent Comparator output Polarity
0b: When CMPIN is above internal threshold, CMPOUT is LOW (internal
hysteresis) <default at POR>
1b: When CMPIN is below internal threshold, CMPOUT is LOW (external
hysteresis)*/
#define CMP_DEG_BIT                      0x02 /*default 00b Independent comparator deglitch time, only applied to the falling edge of
CMPOUT (HIGH → LOW).
00b: Independent comparator is enabled with output deglitch time 5 µs <default
at POR>
01b: Independent comparator is enabled with output deglitch time of 2 ms
10b: Independent comparator is enabled with output deglitch time of 20 ms
11b: Independent comparator is enabled with output deglitch time of 5 sec*/
#define FORCE_CONV_OFF_BIT             0x08 /*default 0b Force Converter Off function
When independent comparator triggers, (CMPOUT pin pulled down) converter
latches off, at the same time, CHRG_OK signal goes LOW to notify the system.
Charge current is also set to zero internally, but charge current register setting
keeps the same. To get out of converter latches off, firstly the CMPOUT should
be released to high and secondly FORCE_CONV_OFF bit should be cleared
(=0b).
0b: Disable this function <default at POR>
1b: Enable this function*/
#define EN_PTM_BIT                     0x04 /*default 0b PTM enable register bit, it will automatically reset to zero
0b: disable PTM. <default at POR>
1b: enable PTM.*/
#define EN_SHIP_DCHG_BIT               0x02 /*default 0b Discharge SRN for Shipping Mode. Used to discharge VBAT pin capacitor
voltage which is necessary for battery gauge device shipping mode.
When this bit is 1, discharge SRN pin down in 140 ms 20 mA. When 140 ms is
over, this bit is reset to 0b automatically. If this bit is written to 0b by host before
140 ms expires, VSYS should stop discharging immediately. Note if after 140-ms
SRN voltage is still not low enough for battery gauge device entering ship mode,
the host may need to start a new 140-ms discharge cycle.
0b: Disable shipping mode <default at POR>
1b: Enable shipping mode
*/
#define AUTO_WAKEUP_EN_BIT             0x01 /*default 0b Auto Wakeup Enable
When this bit is HIGH, if the battery is below VSYS_MIN , the device should
automatically enable 128-mA charging current for 30 mins. When the battery is
charged up above minimum system voltage, charge will terminate and the bit is
reset to LOW. The charger will also exit auto wake up if host write a new charge
current value to charge current register Reg0x14().
0b: Disable <default at POR>
1b: Enable*/

/*ChargeOption2 Register (I2C address = 33h) */

#define PKPWR_TOVLD_DEG_BIT            0x80 /*default 00b Input Overload time in Peak Power Mode
00b: 1 ms <default at POR>
01b: 2 ms
10b: 5 ms
11b: 10 ms*/
#define EN_PKPWR_IIN_DPM_BIT           0x40 /*default 0b Enable Peak Power Mode triggered by input current overshoot
If REG0x33[5:4] are 00b, peak power mode is disabled. Upon adapter
removal, the bits are reset to 00b.
0b: Disable peak power mode triggered by input current overshoot
<default at POR>
1b: Enable peak power mode triggered by input current overshoot.*/
#define EN_PKPWR_VSYS_BIT              0x10 /*default 0b Enable Peak Power Mode triggered by system voltage under-shoot
If REG0x33[5:4] are 00b, peak power mode is disabled. Upon adapter
removal, the bits are reset to 00b.
0b: Disable peak power mode triggered by system voltage under-shoot
<default at POR>
1b: Enable peak power mode triggered by system voltage under-shoot.*/
#define STAT_PKPWR_OVLD_BIT            0x08 /*default 0b Indicator that the device is in overloading cycle. Write 0 to get out of
overloading cycle.
0b: Not in peak power mode. <default at POR>
1b: In peak power mode.*/
#define STAT_PKPWR_RELAX_BIT           0x04 /*default 0b Indicator that the device is in relaxation cycle. Write 0 to get out of
relaxation cycle.
0b: Not in relaxation cycle. <default at POR>
1b: In relaxation mode.*/
#define PKPWR_TMAX_BIT                 0x02 /*default 00b Peak power mode overload and relax cycle time.
00b: 20 ms <default at POR>
01b: 40 ms
10b: 80 ms
11b: 1 sec*/

/*ChargeOption2 Register (I2C address = 32h)*/

#define EN_EXTILIM_BIT                0x80 /*default 1b Enable ILIM_HIZ pin to set input current limit
0b: Input current limit is set by IIN_DPM register..
1b: Input current limit is set by the lower value of ILIM_HIZ pin and
IIN_DPM register.. <default at POR>*/
#define EN_ICHG_IDCHG_BIT             0x40 /*default 0b: IBAT pin as discharge current. <default at POR>
1b: IBAT pin as charge current.*/

#define Q2_OCP_BIT                    0x20 /*default 1b Q2 OCP threshold by sensing Q2 VDS
0b: 210 mV
1b: 150 mV <default at POR>*/
#define ACX_OCP_BIT                   0x10 /*default 1b Fixed Input current OCP threshold by sensing ACP-ACN, converter
is disabled immediately when triggered non latch protection resume
switching automatically after ACX comparator release.
0b: 280 mV(RSNS_RAC=0b)/200 mV(RSNS_RAC=1b)
1b: 150 mV(RSNS_RAC=0b)/100 mV(RSNS_RAC=1b) <default at
POR>
*/
#define EN_ACOC_BIT                   0x08 /*default 0b ACOC Enable
Configurable Input overcurrent (ACOC) protection by sensing the
voltage across ACP and ACN. Upon ACOC (after 250-μs blank-out
time), converter is disabled. Non latch fault, after 250-ms falling edge
de-glitch time converter starts switching automatically.
0b: Disable ACOC <default at POR>
1b: ACOC threshold 133% or 200% ILIM2*/
#define ACOC_VTH_BIT                 0x04 /*default 1b ACOC Limit
Set MOSFET OCP threshold as percentage of IIN_DPM with current
sensed from RAC.
0b: 133% of ILIM2
1b: 200% of ILIM2 <default at POR>*/
#define EN_BATOC_BIT                 0x02 /*default 1b BATOC
Battery discharge overcurrent (BATOC) protection by sensing the
voltage across SRN and SRP. Upon BATOC, converter is disabled.
0b: Disable BATOC
1b: Enable BATOC threshold 133% or 200% PROCHOT IDCHG_TH2
<default at POR>*/
#define BATOC_VTH_BIT                0x01 /*default 1b Set battery discharge overcurrent threshold as percentage of
PROCHOT battery discharge current limit. Note when SRN and SRP
common voltage is low for 1S application, the BATOC threshold could
be derating.
0b: 133% of PROCHOT IDCHG_TH2
1b: 200% of PROCHOT IDCHG _TH2<default at POR>*/
/*ChargeOption3 Register (I2C address = 35/34h) [reset = 0434h]*/

#define EN_HIZ_BIT                   0x80 /*default 0b Device HIZ Mode Enable
When the charger is in HIZ mode, the device draws minimal quiescent
current. With VBUS above UVLO. REGN LDO stays on, and system
powers from battery.
0b: Device not in HIZ mode <default at POR>
1b: Device in HIZ mode*/

#define RESET_REG_BIT               0x40 /*default 0b Reset Registers
All the registers are reset to POR default setting except the VINDPM
register.
0b: Idle <default at POR>
1b: Reset all the registers to default values. After reset, this bit goes back
to 0.*/
#define RESET_VINDPM_BIT            0x20 /*default 0b Reset VINDPM Threshold
0b: Idle
1b: Converter is disabled to measure VINDPM threshold. After VINDPM
measurement is done, this bit goes back to 0 and converter starts*/
#define EN_OTG_BIT                  0x10 /*default 0b OTG Mode Enable
Enable device in OTG mode when OTG/VAP/FRS pin is HIGH.
0b: Disable OTG <default at POR>
1b: Enable OTG mode to supply VBUS from battery*/
#define EN_ICO_MODE_BIT             0x08 /* default 0b Enable ICO Algorithm
0b: Disable ICO algorithm. <default at POR>
1b: Enable ICO algorithm.*/
#define EN_OTG_BIGCAP_BIT           0x01 /*default 0b Enable OTG compensation for VBUS effective capacitance larger than 33
μF
0b: Disable OTG large VBUS capacitance compensation (Recommended
for VBUS effective capacitance smaller than 33 μF) <default at POR>
1b: Enable OTG large VBUS capacitance compensation (Recommended
for VBUS effective capacitance larger than 33 μF)*/

/*ChargeOption3 Register (I2C address = 34h)*/
#define EN_VBUS_VAP_BIT             0x40 /*default 0b Enable the VBUS VAP for VAP operation mode 2&3
0b: Disabled <default at POR>
1b: Enabled
*/
#define OTG_VAP_MODE_BIT            0x20 /*default 1b The selection of the external OTG/VAP/FRS pin control. Don't
recommend to change pin control after OTG/VAP/FRS pin is pulled high.
0b: the external OTG/VAP/FRS pin controls the EN/DIS VAP mode
1b: the external OTG/VAP/FRS pin controls the EN/DIS OTG mode
<default at POR>
*/
#define IL_AVG_BIT                  0x10 /*default 10b Converter inductor average current clamp. It is recommended to choose
the smallest option which is higher than maximum possible converter
average inductor current.
00b: 6A
01b: 10A
10b: 15A <default at POR>
11b: Disabled
*/

#define CMP_EN_BIT                 0x04 /*default 1b Enable Independent Comparator with effective low.
0b: Disabled
1b: Enabled <default at POR>*/
#define PSYS_OTG_IDCHG_BIT         0x01 /*default 0b PSYS function during OTG mode.
0b: PSYS as battery discharge power minus OTG output power <default
at POR>
1b: PSYS as battery discharge power only*/


/*ProchotOption0 Register (I2C address = 37/36h) [reset = 4A81h(2S~5s)*/
/*ProchotOption0 Register (I2C address = 37h) */

#define ILIM2_VTH_BIT              0x80 /*default 01001b ILIM2 Threshold
5 bits, percentage of IIN_DPM in 0x22H. Measure current between ACP and
ACN.
Trigger when the current is above this threshold:
00001b - 11001b: 110% - 230%, step 5%
11010b - 11110b: 250% - 450%, step 50%
11111b: Out of Range (Ignored)
Default 150%, or 01001
*/
#define ICRIT_DEG_BIT             0x04 /*default 01 ICRIT Deglitch time
ICRIT threshold is set to be 110% of ILIM2.
Typical ICRIT deglitch time to trigger PROCHOT.
00b: 15 µs
01b: 100 µs <default at POR>
10b: 400 µs (max 500 μs)
11b: 800 µs (max 1 ms)
*/
#define PROCHOT_VINDPM_80_90      0x01 /*default 0b Lower threshold of the PROCHOT_VINDPM comparator
When REG0x33[0]=1, the threshold of the PROCHOT_VINDPM comparator is
determined by this bit setting.
0b: 83% of VinDPM threshold <default at POR>.
1b: 91% of VinDPM threshold
*/

#define VSYS_TH1_BIT              0x80 /*default 100000b(2S~5s)000010b(1S) VSYS Threshold to trigger discharging VBUS in VAP mode.
Measure on VSYS with fixed 5-µs deglitch time. Trigger when SYS pin voltage is
below the thresholds. There is a fixed DC offset which is 3.2 V.
2S - 5s battery (Default: 6.4 V)
000000b- 111111b: 3.2 V - 9.5 V with 100-mV step size.
1S battery (Default: 3.4 V)
XXX000b - XXX111b: 3.2 V - 3.9 V with 100-mV step size*/
#define INOM_DEG_BIT              0x02 /*default 0b INOM Deglitch Time
INOM is always 10% above IIN_DPM register setting. Measure current between
ACP and ACN.
Trigger when the current is above this threshold.
0b: 1 ms(max) <default at POR>
1b: 60 ms(max)*/
#define LOWER_PROCHOT_VINDPM_BIT     0x01 /*default 1b Enable the lower threshold of the PROCHOT_VINDPM comparator
0b: the threshold of the PROCHOT_VINDPM comparator follows the same
VINDPM REG0x3D() setting.
1b: the threshold of the PROCHOT_VINDPM comparator is lower and determined
by PROCHOT_VINDPM_80_90 bit setting. <default at POR>*/

/*ProchotOption1 Register (I2C address = 39h)*/
#define IDCHG_TH1_BIT            0x80 /*default 010000b IDCHG level 1 Threshold
6 bit, range, range 0 A to 64512 mA, step 1024 mA.
Measure current between SRN and SRP.
Trigger when the discharge current is above the threshold.
If the value is programmed to 000000b PROCHOT is always triggered.
Default: 16256 mA or 010000b*/
#define IDCHG_DEG1_BIT           0x02 /*default 0b IDCHG level 1 Deglitch Time
00b: 78 ms
01b: 1.25s <default at POR>
10b: 5s
11b: 20s*/
#define PP_VINDPM_BIT            0x80 /*default 1b VINDPM PROCHOT Profile
When all the REG0x38[7:0] , REG0x3D[1], REG0x3C[2]bits are 0, PROCHOT
function is disabled.
0b: disable
1b: enable<default at POR>*/
#define PP_COMP_BIT              0x40 /*default 0 Independent comparator PROCHOT Profile
When not in low power mode(Battery only), use this bit to control independent
comparator PROCHOT profiles.
When in low power mode(Battery only), this bit will lose controllability
to independent comparator PROCHOT profiles. Need to use
EN_PROCHOT_LPWR to enable independent comparator and its PROCHOT
profile.
0b: disable <default at POR>
1b: enable*/
#define PP_ICRIT_BIT             0x20 /*default 1b ICRIT PROCHOT Profile
0b: disable
1b: enable <default at POR>*/
#define PP_INOM_BIT              0x10 /*default 0b INOM PROCHOT Profile
0b: disable <default at POR>
1b: enable*/
#define PP_IDCHG1_BIT            0x08 /*default 0b IDCHG1 PROCHOT Profile
0b: disable <default at POR>
1b: enable*/
#define PP_VSYS_BIT              0x04 /*default 0b VSYS PROCHOT Profile
0b: disable <default at POR>
1b: enable*/
#define PP_BATPRES_BIT           0x02 /*default 0b Battery removal PROCHOT Profile
0b: disable <default at POR>
1b: enable (one-shot falling edge triggered)
If BATPRES is enabled in PROCHOT after the battery is removed, it will
immediately send out one-shot PROCHOT pulse*/
#define PP_ACOK_BIT              0x01 /*default 0b Adapter removal PROCHOT Profile
0b: disable <default at POR>
1b: enable
EN_LWPWR= 0b to assert PROCHOT pulse after adapter removal.
If PP_ACOK is enabled in PROCHOT after the adapter is removed, it will be
pulled low.*/

/*ADCOption Register (I2C address = 3B/3Ah) [reset = 2000h]*/
#define ADC_CONV_BIT            0x80 /*default 0b Typical each ADC channel conversion time is 25 ms maximum. Total ADC
conversion time is the product of 25 ms and enabled channel counts.
0b: One-shot update. Do one set of conversion updates to registers
REG0x29/28(), REG0x27/26(), REG0x2B/2A(), and REG0x2D/2C() after
ADC_START = 1.
1b: Continuous update. Do a set of conversion updates to registers
REG0x29/28(), REG0x27/26(), REG0x2B/2A(), and REG0x2D/2C()every 1
sec*/
#define ADC_START_BIT           0x40/*default 0b 0b: No ADC conversion
1b: Start ADC conversion. After the one-shot update is complete, this bit
automatically resets to zero*/
#define ADC_FULLSCALE_BIT      0x20 /*default 1b ADC input voltage range adjustment for PSYS and CMPIN ADC Channels.
2.04-V full scale holds 8 mV/LSB resolution and 3.06-V full scale holds 12
mV/LSB resolution
0b: 2.04 V
1b: 3.06 V <default at POR>(Not accurate for REGN<6-V application (VBUS& VSYS< 6V))*/

/*ADCOption Register (I2C address = 3Ah)*/

#define EN_ADC_CMPIN_BIT        0x80 /*default 0b 0b: Disable <default at POR>
1b: Enable*/
#define EN_ADC_VBUS_BIT         0x40
#define EN_ADC_PSYS_BIT         0x20
#define EN_ADC_IIN_BIT          0x10
#define EN_ADC_IDCHG_BIT        0x08
#define EN_ADC_ICHG_BIT         0x04
#define EN_ADC_VSYS_BIT         0x02
#define EN_ADC_VBAT_BIT         0x01

/*ChargeOption4 Register (I2C address = 3D/3Ch) [reset = 0048h]*/

#define EN_DITHER_BIT           0x10 /*default 00b Frequency Dither configuration
00b: Disable Dithering<default at POR>
01b: Dither 1X (±2% Fs dithering range)
10b: Dither 2X (±4% Fs dithering range)
11b: Dither 3X (±6% Fs dithering range)*/
#define PP_VBUS_VAP_BIT         0x02 /*VBUS_VAP PROCHOT Profile
0b: disable <default at POR>
0b: enable*/
#define STAT_VBUS_VAP_BIT       0x01 /*default 0b PROCHOT profile VBUS_VAP status bit. The status is latched until a read
from host.
0b: Not triggered <default at POR>
1b: Triggered*/

/*ChargeOption4 Register (I2C address = 3Ch)*/
#define IDCHG_DEG2_BIT          0x80 /*default 01 Battery discharge current limit 2 deglitch time(minimum value)
00b: 100 μs
01b: 1.6 ms <default at POR>
10b: 6 ms
11b: 12 ms*/
#define IDCHG_TH2_BIT           0x20 /*default 001b Battery discharge current limit2 based on percentage of IDCHG_TH1.
Note IDCHG_TH2 setting higher than 32256 mA should lose accuracy
de-rating between target value and 32256 mA. (Recommend not to set
higher than 20 A for 1S OTG boost operation)
000b: 125% IDCHG_TH1
001b: 150% IDCHG_TH1 <default at POR>
010b: 175% IDCHG_TH1
011b: 200% IDCHG_TH1
100b: 250% IDCHG_TH1
101b: 300% IDCHG_TH1
110b: 350% IDCHG_TH1
111b: 400% IDCHG_TH1*/

#define PP_IDCHG2_BIT           0x04 /*default 0b IDCHG2 PROCHOT Profile
0b: disable <default at POR>
1b: enable*/
#define STAT_IDCHG2_BIT         0x02 /*default 0b The status is latched until a read from host.
0b: Not triggered <default at POR>
1b: Triggered
*/
#define STAT_PTM_BIT            0x01/*default 0b PTM operation status bit monitor
0b: Not in PTM Operation <default at POR>
1b: In PTM Operation*/

/*Vmin Active Protection Register (I2C address = 3F/3Eh)  [reset = 0070h/0004h]*/
/*0 = Adds 0 mV of VAP Mode VBUS PROCHOT trigger voltage threshold
1 = Adds x mV of VAP Mode VBUS PROCHOT trigger voltage threshold
 * */
#define VBUS_VAP_TH_6400_MV_BIT     0x80
#define VBUS_VAP_TH_3200_MV_BIT     0x40
#define VBUS_VAP_TH_1600_MV_BIT     0x20
#define VBUS_VAP_TH_800_MV_BIT      0x01
#define VBUS_VAP_TH_400_MV_BIT      0x08
#define VBUS_VAP_TH_200_MV_BIT      0x04
#define VBUS_VAP_TH_100_MV_BIT      0x02

/*Vmin Active Protection Register (I2C address = 3Eh) */
/*0 = Adds 0 mV of VAP mode VSYS PROCHOT trigger voltage threshold
1 = Adds x mV of VAP mode VSYS PROCHOT trigger voltage threshold*/
#define VSYS_TH2_3200_MV_BIT        0x80
#define VSYS_TH2_1600_MV_BIT        0x40
#define VSYS_TH2_800_MV_BIT         0x20
#define VSYS_TH2_400_MV_BIT         0x10
#define VSYS_TH2_200_MV_BIT         0x08
#define VSYS_TH2_100_MV_BIT         0x04
#define EN_VSYSTH2_FOLLOW_VSYSTH1_BIT   0x02 /*default 0b Enable internal VSYS_TH2 follow VSYS_TH1 setting neglecting register
REG37[7:2] setting
0b: disable <default at POR>
1b: enable*/
#define EN_FRS_BIT                  0x01   /*default 0b Fast Role Swap feature enable (note not recommend to change EN_FRS
during OTG operation, the FRS bit from 0 to 1 change will disable power
stage for about 200 μs (Fs = 400 kHz). HIZ mode holds higher priority, If
EN_HIZ=1b, this EN_FRS bit should be forced to 0b.
0b: disable <default at POR>
1b: enable*/

/* OTGVoltage Register (I2C address = 07/06h) [reset = 09C4h]*/
/*OTGVoltage Register (I2C address = 07h)*/
/*0 = Adds 0 mV of OTG voltage.
1 = Adds x mV of OTG voltage.*/
#define OTG_VOLTAGE_16384_MV_BIT        0x20
#define OTG_VOLTAGE_8192_MV_BIT         0x10
#define OTG_VOLTAGE_4096_MV_BIT         0x08
#define OTG_VOLTAGE_2048_MV_BIT         0x04
#define OTG_VOLTAGE_1024_MV_BIT         0x02
#define OTG_VOLTAGE_512_MV_BIT          0x01


/* OTGVoltage Register (I2C address = 06h)*/
/*0 = Adds 0 mV of OTG voltage.
1 = Adds x mV of OTG voltage.*/
#define OTG_VOLTAGE_256_MV_BIT          0x80
#define OTG_VOLTAGE_128_MV_BIT          0x40
#define OTG_VOLTAGE_64_MV_BIT           0x20
#define OTG_VOLTAGE_32_MV_BIT           0x10
#define OTG_VOLTAGE_16_MV_BIT           0x08
#define OTG_VOLTAGE_8_MV_BIT            0x04

/*OTGCurrent Register (I2C address = 09/08h) [reset = 3C00h]*/
/*. OTGCurrent Register (I2C address = 09h*/
/*0 = Adds 0 mA of OTG current.
1 = Adds x mA of OTG current.*/
#define OTG_CURRENT_3200_MA_BIT        0x40
#define OTG_CURRENT_1600_MA_BIT        0x20
#define OTG_CURRENT_800_MA_BIT         0x10
#define OTG_CURRENT_400_MA_BIT         0x08
#define OTG_CURRENT_200_MA_BIT         0x04
#define OTG_CURRENT_100_MA_BIT         0x02
#define OTG_CURRENT_50_MA_BIT          0x01

/* InputVoltage(VINDPM) Register (I2C address = 0B/0Ah) [reset =VBUS-1.28V]*/
/*0 = Adds 0 mV of input voltage.
1 = Adds x mV of input voltage.*/
#define INPUT_VOLTAGE_8192_MV_BIT          0x20
#define INPUT_VOLTAGE_4096_MV_BIT          0x10
#define INPUT_VOLTAGE_2048_MV_BIT          0x08
#define INPUT_VOLTAGE_1024_MV_BIT          0x04
#define INPUT_VOLTAGE_512_MV_BIT           0x02
#define INPUT_VOLTAGE_256_MV_BIT           0x01
/* InputVoltage Register (I2C address = 0Ah)*/
#define INPUT_VOLTAGE_128_MV_BIT           0x80
#define INPUT_VOLTAGE_64_MV_BIT            0x40

/*IIN_DPM Register with 10-mΩ Sense Resistor (I2C address = 25/24h) [reset = 4100h]*/
/*0 = Adds 0 mA of input current.
1 = Adds x mA of input current.*/
#define INPUT_CURRENT_6400_MA_BIT         0x40
#define INPUT_CURRENT_3200_MA_BIT         0x20
#define INPUT_CURRENT_1600_MA_BIT         0x10
#define INPUT_CURRENT_800_MA_BIT          0x08
#define INPUT_CURRENT_400_MA_BIT          0x04
#define INPUT_CURRENT_200_MA_BIT          0x02
#define INPUT_CURRENT_100_MA_BIT          0x01


typedef struct //21/20h
{
	uint8_t Fault_OTG_UVP:1;
	uint8_t Fault_OTG_OVP:1;
	uint8_t Fault_Force_Converter_Off:1;
	uint8_t Fault_VSYS_UVP:1;
	uint8_t Fault_SYSOVP:1;
	uint8_t Fault_ACOC:1;
	uint8_t Fault_BATOC:1;
	uint8_t Fault_ACOV:1;
	uint8_t IN_OTG:1;
	uint8_t Reserved:1;
	uint8_t IN_FCHRG:1;
	uint8_t IN_IIN_DPM:1;
	uint8_t IN_VINDPM:1;
	uint8_t IN_VAP:1;
	uint8_t ICO_DONE:1;
	uint8_t STAT_AC:1;
}ChargerStatus_t ;
typedef struct
{
	struct //01/00h
	{
		uint8_t CHRG_INHIBIT:1;
		uint8_t EN_IIN_DPM:1;
		uint8_t Reserved:1;
		uint8_t IBAT_GAIN:1;
		uint8_t IADPT_GAIN:1;
		uint8_t EN_LEARN:1;
		uint8_t VSYS_UVP_ENZ:1;
		uint8_t EN_CMP_LATCH:1;
		uint8_t LOW_PTM_RIPPLE:1;
		uint8_t PWM_FREQ:1;
		uint8_t EN_OOA:1;
		uint8_t OTG_ON_CHRGOK:1;
		uint8_t IIN_DPM_AUTO_DISAB:1;
		uint8_t WDTMR_ADJ:2;
		uint8_t EN_LWPWR:1;
	}ChargeOption0;
	struct //03/02h
	{

		uint8_t BIT0:1;
		uint8_t BIT1:1;
		uint8_t BIT2:1;
		uint8_t BIT3:1;
		uint8_t BIT4:1;
		uint8_t BIT5:1;
		uint8_t BIT6:1;
		uint8_t Reserved_1:2;
	}ChargeCurrent;
	struct //05/04h
	{
		uint8_t Reserved:3;
		uint8_t BIT0:1;
		uint8_t BIT1:1;
		uint8_t BIT2:1;
		uint8_t BIT3:1;
		uint8_t BIT4:1;
		uint8_t BIT5:1;
		uint8_t BIT6:1;
		uint8_t BIT7:1;
		uint8_t BIT8:1;
		uint8_t BIT9:1;
		uint8_t BIT10:1;
		uint8_t BIT11:1;
		uint8_t Reserved_1:1;
	}ChargeVoltage;
	struct//07/06h
	{
		uint8_t Reserved:2;
		uint8_t BIT0:1;
		uint8_t BIT1:1;
		uint8_t BIT2:1;
		uint8_t BIT3:1;
		uint8_t BIT4:1;
		uint8_t BIT5:1;
		uint8_t BIT6:1;
		uint8_t BIT7:1;
		uint8_t BIT8:1;
		uint8_t BIT9:1;
		uint8_t BIT10:1;
		uint8_t BIT11:1;
		uint8_t Reserved_1:2;
	}OTGVoltage;
	struct//09/08h
	{
		uint8_t Reserved:8;
		uint8_t BIT0:1;
		uint8_t BIT1:1;
		uint8_t BIT2:1;
		uint8_t BIT3:1;
		uint8_t BIT4:1;
		uint8_t BIT5:1;
		uint8_t BIT6:1;
		uint8_t Reserved_1:1;
	}OTGCurrent;
	struct//0B/0Ah
	{
		uint8_t Reserved:6;
		uint8_t BIT0:1;
		uint8_t BIT1:1;
		uint8_t BIT2:1;
		uint8_t BIT3:1;
		uint8_t BIT4:1;
		uint8_t BIT5:1;
		uint8_t BIT6:1;
		uint8_t BIT7:1;
		uint8_t Reserved_2:2;
	}VINDPM;
	struct//0F/0Eh
	{
		uint8_t Reserved:8;
		uint8_t BIT0:1;
		uint8_t BIT1:1;
		uint8_t BIT2:1;
		uint8_t BIT3:1;
		uint8_t BIT4:1;
		uint8_t BIT5:1;
		uint8_t BIT6:1;
		uint8_t Reserved_1:1;
	}IIN_HOST;

	ChargerStatus_t ChargerStatus;
	struct //23/22h
	{
		uint8_t STAT_Adapter_Removal:1;
		uint8_t STAT_Battery_Removal:1;
		uint8_t STAT_VSYS:1;
		uint8_t STAT_IDCHG1:1;
		uint8_t STAT_INOM:1;
		uint8_t STAT_ICRIT:1;
		uint8_t STAT_COMP:1;
		uint8_t STAT_VINDPM:1;
		uint8_t STAT_EXIT_VAP:1;
		uint8_t STAT_VAP_FAIL:1;
		uint8_t TSHUT:1;
		uint8_t PROCHOT_CLEAR:1;
		uint8_t PROCHOT_WIDTH:2;
		uint8_t EN_PROCHOT_EXT:1;
		uint8_t Reserved:1;
	}ProchotStatus;
	struct //25/24h
	{
		uint8_t Reserved:8;
		uint8_t BIT0:1;
		uint8_t BIT1:1;
		uint8_t BIT2:1;
		uint8_t BIT3:1;
		uint8_t BIT4:1;
		uint8_t BIT5:1;
		uint8_t BIT6:1;
		uint8_t Reserved_1:1;
	}IIN_DPM;
	struct //27/26h
	{
		uint8_t PSYS:8;
		uint8_t VBUS:8;
	}ADCVBUS_PSYS;
	struct //28/29
	{
		uint8_t Discharge_Current:7;
		uint8_t Reserved:1;
		uint8_t Charge_Current:7;
		uint8_t Reserved_1:1;
	}ADCIBAT;
	struct //2B/2A
	{
		uint8_t CMPIN:8;
		uint8_t ADCIIN:8;
	}ADCIIN_CMPIN;
	struct //2D/2C
	{
		uint8_t VBAT;
		uint8_t VSYS;
	}ADCVSYS_VBAT;
	uint8_t MANUFACTURE_ID:8; //2Eh
	uint8_t DEVICE_ID:8;//2F
	struct //31/30h
	{
		uint8_t AUTO_WAKEUP_EN:1;
		uint8_t EN_SHIP_DCHG:1;
		uint8_t EN_PTM:1;
		uint8_t FORCE_CONV_OFF:1;
		uint8_t CMP_DEG:2;
		uint8_t CMP_POL:1;
		uint8_t CMP_REF:1;
		uint8_t EN_FAST_5MOHM:1;
		uint8_t PSYS_RATIO:1;
		uint8_t RSNS_RSR:1;
		uint8_t RSNS_RAC:1;
		uint8_t PSYS_CONFIG:2;
		uint8_t EN_PROCHOT_LPWR:1;
		uint8_t EN_IBAT:1;
	}ChargeOption1;
	struct //33/32h
	{
		uint8_t BATOC_VTH:1;
		uint8_t EN_BATOC:1;
		uint8_t ACOC_VTH:1;
		uint8_t EN_ACOC:1;
		uint8_t ACX_OCP:1;
		uint8_t Q2_OCP:1;
		uint8_t EN_ICHG_IDCHG:1;
		uint8_t EN_EXTILIM:1;
		uint8_t PKPWR_TMAX:2;
		uint8_t STAT_PKPWR_RELAX:1;
		uint8_t STAT_PKPWR_OVLD:1;
		uint8_t EN_PKPWR_VSYS:1;
		uint8_t EN_PKPWR_IIN_DPM:1;
		uint8_t PKPWR_TOVLD_DEG:2;
	}ChargeOption2;
	struct //35/34h
	{
		uint8_t PSYS_OTG_IDCHG:1;
		uint8_t Reserved:1;
		uint8_t CMP_EN:1;
		uint8_t IL_AVG:2;
		uint8_t OTG_VAP_MODE:1;
		uint8_t EN_VBUS_VAP:1;
		uint8_t Reserved_1:1;
		uint8_t EN_OTG_BIGCAP:1;
		uint8_t Reserved_2:2;
		uint8_t EN_ICO_MODE:1;
		uint8_t EN_OTG:1;
		uint8_t RESET_VINDPM:1;
		uint8_t RESET_REG:1;
		uint8_t EN_HIZ:1;
	}ChargeOption3;
	struct //36/35h
	{
		uint8_t LOWER_PROCHOT_VINDPM:1;
		uint8_t INOM_DEG:1;
		uint8_t VSYS_TH1:6;
		uint8_t PROCHOT_VINDPM_0_90:1;
		uint8_t ICRIT_DEG:2;
		uint8_t ILIM2_VTH:5;
	}ProchotOption0;
	struct //38/39h
	{
		uint8_t PP_ACOK:1;
		uint8_t PP_BATPRES:1;
		uint8_t PP_VSYS:1;
		uint8_t PP_IDCHG1:1;
		uint8_t PP_INOM:1;
		uint8_t PP_ICRIT:1;
		uint8_t PP_COMP:1;
        uint8_t PP_VINDPM:1;
        uint8_t IDCHG_DEG1:2;
        uint8_t IDCHG_TH1:6;
	}ProchotOption1;
	struct //3B/3Ah
	{
		uint8_t EN_ADC_VBAT:1;
		uint8_t EN_ADC_VSYS:1;
		uint8_t EN_ADC_ICHG:1;
		uint8_t EN_ADC_IDCHG:2;
		uint8_t EN_ADC_IIN:1;
		uint8_t EN_ADC_PSYS:1;
		uint8_t EN_ADC_VBUS:1;
		uint8_t EN_ADC_CMPIN:1;
		uint8_t Reserved:5;
		uint8_t ADC_FULLSCALE:1;
		uint8_t ADC_START:1;
		uint8_t ADC_CONV:1;
	}ADCOption;
	struct //3D/3Ch
	{
		uint8_t STAT_PTM:1;
		uint8_t STAT_IDCHG2:1;
		uint8_t PP_IDCHG2:1;
		uint8_t IDCHG_TH2:3;
		uint8_t IDCHG_DEG2:2;
		uint8_t STAT_VBUS_VAP:1;
		uint8_t PP_VBUS_VAP:1;
		uint8_t Reserved_1:1;
		uint8_t EN_DITHER:2;
		uint8_t Reserved:3;
	}ChargeOption4;
	struct //3F/3Eh
	{
		uint8_t EN_FRS:1;
		uint8_t EN_VSYSTH2_FOLLOW_VS_YSTH1:1;
		uint8_t VSYS_TH2_BIT0:1;
		uint8_t VSYS_TH2_BIT1:1;
		uint8_t VSYS_TH2_BIT2:1;
		uint8_t VSYS_TH2_BIT3:1;
		uint8_t VSYS_TH2_BIT4:1;
		uint8_t VSYS_TH2_BIT5:1;
		uint8_t Reserve:1;
		uint8_t VBUS_VAP_TH_BIT0:1;
		uint8_t VBUS_VAP_TH_BIT1:1;
		uint8_t VBUS_VAP_TH_BIT2:1;
		uint8_t VBUS_VAP_TH_BIT3:1;
		uint8_t VBUS_VAP_TH_BIT4:1;
		uint8_t VBUS_VAP_TH_BIT5:1;
		uint8_t VBUS_VAP_TH_BIT6:1;
	}Vmin_Active_Protection;
}bq25731_t;

HAL_StatusTypeDef bq25731_read_reg(uint8_t reg, uint8_t *data, uint16_t len);

HAL_StatusTypeDef bq25731_set_bit_reg(uint8_t reg, uint8_t lsb, uint8_t msb);
HAL_StatusTypeDef bq25731_clear_bit_reg(uint8_t reg, uint8_t lsb, uint8_t msb);
HAL_StatusTypeDef bq2573_set_charge_current(void);
HAL_StatusTypeDef bq25731_get_device_id(uint8_t* dev_id);
HAL_StatusTypeDef bq25731_get_manufacture_id(uint8_t* man_id);
HAL_StatusTypeDef bq25731_set_charge_voltage(uint16_t mV);
HAL_StatusTypeDef bq25731_get_charge_voltage(bq25731_t* bq);
HAL_StatusTypeDef bq25731_get_charge_status(bq25731_t* bq);
HAL_StatusTypeDef bq25731_get_prochot_status(bq25731_t* bq);
HAL_StatusTypeDef bq25731_get_charge_discharge_current(bq25731_t* bq, uint16_t *charge_current, uint16_t *discharge_current);
HAL_StatusTypeDef bq25731_get_sys_and_bat_voltage(bq25731_t* bq, uint16_t *vbat, uint16_t *vsys);
HAL_StatusTypeDef bq25731_get_vbus_psys(bq25731_t* bq, uint16_t *vbus, uint16_t *psys);
HAL_StatusTypeDef bq25731_charge_option_0(uint8_t data1,uint8_t data2);
HAL_StatusTypeDef bq25731_charge_option_1(uint8_t data1,uint8_t data2);
HAL_StatusTypeDef bq25731_charge_option_3(uint8_t data1,uint8_t data2);
HAL_StatusTypeDef bq25731_read_charge_option_0(bq25731_t* bq);
HAL_StatusTypeDef bq25731_read_charge_option_1(bq25731_t* bq);
HAL_StatusTypeDef bq25731_read_charge_option_2(bq25731_t* bq);
HAL_StatusTypeDef bq25731_read_charge_option_3(bq25731_t* bq);
HAL_StatusTypeDef bq25731_set_adc_option(uint8_t lsb, uint8_t msb);
HAL_StatusTypeDef bq25731_charge_option_0_clear_bit(uint8_t lsb, uint8_t msb);
HAL_StatusTypeDef bq25731_set_charge_current(uint16_t mA);
HAL_StatusTypeDef bq25731_disable_charge(bq25731_t* bq);
#endif /* INC_BQ25731_H_ */
