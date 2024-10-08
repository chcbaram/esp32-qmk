#ifndef BATTERY_H_
#define BATTERY_H_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_BATTERY




bool batteryInit(void);
bool batteryIsInit(void);
bool batteryIsCharging(void);
bool batteryIsConnected(void);

int32_t batteryGetPercent(void);
float   batteryGetVoltage(void);


#endif


#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_BATTERY_H_ */