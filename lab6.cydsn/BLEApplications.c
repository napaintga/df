#include "main.h"
uint8 deviceConnected = FALSE;	

uint8 startNotification = FALSE;		

CYBLE_CONN_HANDLE_T  				connectionHandle;	

uint8 restartAdvertisement = FALSE;

uint8 updateNotificationCCCAttribute = FALSE;

uint8 busyStatus = 0;

void CustomEventHandler(uint32 event, void *eventParam)
{
    CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
	
    switch(event)
	{

		case CYBLE_EVT_STACK_ON:
			restartAdvertisement = TRUE;
			
			break;
		
		case CYBLE_EVT_TIMEOUT:
		    restartAdvertisement = TRUE;
			break;
			
		
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
			
			if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
			{
				restartAdvertisement = TRUE;
			}
			break;
			
		case CYBLE_EVT_GAP_DEVICE_CONNECTED:		

			break;
			
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:		
			
			restartAdvertisement = TRUE;

            break;

        case CYBLE_EVT_GATT_CONNECT_IND:
            connectionHandle = *(CYBLE_CONN_HANDLE_T *)eventParam;	

			deviceConnected = TRUE;	
	
            break;
			
        case CYBLE_EVT_GATT_DISCONNECT_IND:	
			deviceConnected = FALSE;
			
			startNotification = FALSE;
			
			updateNotificationCCCAttribute = TRUE;
			UpdateNotificationCCCD();
			
			break;    
            
        case CYBLE_EVT_GATTS_WRITE_REQ:				
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
			
            if(CYBLE_CAPSENSE_CAPSENSE_PROXIMITY_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == wrReqParam->handleValPair.attrHandle)
            {
				startNotification = wrReqParam->handleValPair.value.val[CYBLE_CAPSENSE_CAPSENSE_PROXIMITY_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX];
				
				updateNotificationCCCAttribute = TRUE;
			}
            
			CyBle_GattsWriteRsp(connectionHandle);
			
            break;
			
		case CYBLE_EVT_STACK_BUSY_STATUS:
            busyStatus = * (uint8*)eventParam;
            break;
			
        default:
                        
            break;
	} 
}

void SendDataOverCapSenseNotification(uint8 proximityValue)
{
	CYBLE_GATTS_HANDLE_VALUE_NTF_T		notificationHandle; 

	if(busyStatus == CYBLE_STACK_STATE_FREE)
	{
		notificationHandle.attrHandle = CYBLE_CAPSENSE_CAPSENSE_PROXIMITY_CHAR_HANDLE;				
		notificationHandle.value.val = &proximityValue;
		notificationHandle.value.len = CAPSENSE_NOTIFICATION_DATA_LEN;
		
		CyBle_GattsNotification(connectionHandle,&notificationHandle);
	}
}

void UpdateNotificationCCCD(void)
{
	uint8 CCCDvalue[2];
	
	CYBLE_GATT_HANDLE_VALUE_PAIR_T notificationCCCDhandle;

	if(updateNotificationCCCAttribute)
	{
		updateNotificationCCCAttribute = FALSE;
	
		CCCDvalue[0] = startNotification;
		CCCDvalue[1] = 0x00;
		
		notificationCCCDhandle.attrHandle = CYBLE_CAPSENSE_CAPSENSE_PROXIMITY_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
		notificationCCCDhandle.value.val = CCCDvalue;
		notificationCCCDhandle.value.len = CCC_DATA_LEN;
		
		CyBle_GattsWriteAttributeValue(&notificationCCCDhandle, ZERO, &connectionHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);
	}	
}

void HandleStatusLED(void)
{

	static uint32 led_counter = TRUE;

	static uint8 state = 0xFF;

	uint8 state_changed = FALSE;
	
	if(state != CyBle_GetState())
	{
		state_changed = TRUE;
				
		if(CyBle_GetState() == CYBLE_STATE_ADVERTISING)
		{
			led_counter = TRUE;
		}
	}
	
	state = CyBle_GetState();
		
	switch(state)
	{
		case CYBLE_STATE_CONNECTED:
			if(state_changed)
			{
				state_changed = FALSE;
				
				Status_LED_SetDriveMode(Status_LED_DM_ALG_HIZ);
                Status_LED_Write(LED_ON);
			}
		break;
		
		case CYBLE_STATE_ADVERTISING:
			if((--led_counter) == FALSE)
			{
				Status_LED_SetDriveMode(Status_LED_DM_STRONG);
				
				if(Status_LED_Read() == LED_OFF)
				{
					Status_LED_Write(LED_ON);
					led_counter	= LED_ADV_BLINK_PERIOD_ON;
				}
				else
				{
					Status_LED_Write(LED_OFF);
					led_counter	= LED_ADV_BLINK_PERIOD_OFF;
				}
			}
		break;
		
		case CYBLE_STATE_DISCONNECTED:
			Status_LED_Write(LED_OFF);
			Status_LED_SetDriveMode(Status_LED_DM_ALG_HIZ);
		break;
		
		default:

		break;
	}
	
	state_changed = FALSE;
}
