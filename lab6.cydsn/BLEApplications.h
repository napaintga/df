#if !defined(BLEAPPLICATIONS_H)
#define BLEAPPLICATIONS_H
#include <project.h>

#define ENABLE_LOW_POWER_MODE
	
#define CAPSENSE_ENABLED

void CustomEventHandler(uint32 event, void * eventParam);
void SendDataOverCapSenseNotification(uint8 proximityValue);
void UpdateNotificationCCCD(void);
void HandleStatusLED(void);

#define CAPSENSE_NOTIFICATION_DATA_LEN		1

#define CCCD_NTF_BIT_MASK					0x01

#define CCC_DATA_LEN						2

#ifdef ENABLE_LOW_POWER_MODE
#define	LED_ADV_BLINK_PERIOD_ON			8000    
#define LED_ADV_BLINK_PERIOD_OFF		8000
#else
#define	LED_ADV_BLINK_PERIOD_ON			10000
#define LED_ADV_BLINK_PERIOD_OFF		15000
#endif

#define LED_ON							0x00
#define LED_OFF							0x01

#endif
