
#if !defined(MAIN_H)
#define MAIN_H

#include <project.h>
#include <BLEApplications.h>
	
void InitializeSystem(void);
void HandleCapSenseProximity(void);
void CustomEventHandler(uint32 event, void * eventParam);

#define TRUE								1
#define FALSE								0
#define ZERO								0

#define MAX_PROX_VALUE						0xFF

#define PROX_COUNTER_VALUE					5

#endif
