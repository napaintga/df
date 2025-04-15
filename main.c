#include <main.h>

extern uint8 deviceConnected;

extern uint8 startNotification;	

extern uint8 restartAdvertisement;

uint8 initializeCapSenseBaseline = TRUE;

int main()
{
	InitializeSystem();
	
    for(;;)
    {
		CyBle_ProcessEvents();
		
		HandleStatusLED();
		
		if(TRUE == deviceConnected) 
		{	
			UpdateNotificationCCCD();
			
			if(initializeCapSenseBaseline)
			{
				initializeCapSenseBaseline = FALSE;
				
				CapSense_InitializeAllBaselines();
			}
			
			if(startNotification & CCCD_NTF_BIT_MASK)
			{
				HandleCapSenseProximity();
			}
		}

		if(restartAdvertisement)
		{
			restartAdvertisement = FALSE;
			

			CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);	
		}
    }	
}

void InitializeSystem(void)
{
	CyGlobalIntEnable;

	CyBle_Start(CustomEventHandler);					
	
	Status_LED_SetDriveMode(Status_LED_DM_STRONG);	
	
	#ifdef CAPSENSE_ENABLED
	CapSense_EnableWidget(CapSense_PROXIMITYSENSOR0__PROX);
	CapSense_Start();
	#endif
		
}

void HandleCapSenseProximity(void)
{
	#ifdef CAPSENSE_ENABLED
	static uint16 proxCounter = TRUE;			
				
	uint8 proximityValue;	
		
	if(FALSE == (--proxCounter))			
	{
		proxCounter = PROX_COUNTER_VALUE;
		CapSense_ScanWidget(CapSense_PROXIMITYSENSOR0__PROX);				
		
		while(CapSense_IsBusy())
		{
		}
		
		CapSense_UpdateEnabledBaselines();			

		proximityValue = CapSense_GetDiffCountData(CapSense_PROXIMITYSENSOR0__PROX);
		
		SendDataOverCapSenseNotification(proximityValue);
			
	}
	#endif
}

