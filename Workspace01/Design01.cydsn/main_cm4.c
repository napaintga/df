/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
 void ShowCapSense(void);

void ShowCapSense(void)
{    
        while (1)  
  {
        if (!CapSense_IsBusy()) 
        {
            CapSense_ProcessAllWidgets();
            int pos;
          pos=CapSense_GetCentroidPos(CapSense_LINEARSLIDER0_WDGT_ID);
if (pos<101) {PWM_2_SetCompare0(pos);}
                else PWM_2_SetCompare0(0);
            
            if (CapSense_IsWidgetActive(CapSense_BUTTON0_WDGT_ID))
            {
                Cy_GPIO_Write(LED_1_PORT,LED_1_NUM,1u);
                Cy_GPIO_Write(LED_2_PORT,LED_2_NUM,1u);
                PWM_1_SetCompare0(0u);
            } 
       
 else 
         {
            if (CapSense_IsWidgetActive(CapSense_BUTTON1_WDGT_ID)) 
            {
                Cy_GPIO_Write(LED_1_PORT,LED_1_NUM,0u);
                Cy_GPIO_Write(LED_2_PORT,LED_2_NUM,1u);
                PWM_1_SetCompare0(100u);
            }
else 
            {
                Cy_GPIO_Write(LED_1_PORT,LED_1_NUM,0u);
                Cy_GPIO_Write(LED_2_PORT,LED_2_NUM,0u);
                PWM_1_SetCompare0(0u);
            }
         }     
        }
        CapSense_ScanAllWidgets();   
    }
}



int main(void)
{
    __enable_irq(); /* Enable global interrupts. */

    PWM_1_Start();
    PWM_2_Start();
    PWM_1_SetCompare0(0u);
    PWM_2_SetCompare0(0u);
    Cy_GPIO_Write(LED_1_PORT,LED_1_NUM,0u);
    Cy_GPIO_Write(LED_2_PORT,LED_2_NUM,0u);
    Cy_GPIO_Write(GREEN_PORT,GREEN_NUM,1u);
    Cy_GPIO_Write(BLUE_PORT,BLUE_NUM,1u);
     
    CapSense_Start();
      
    for(;;)
    {
     ShowCapSense();    
    }

}

/* [] END OF FILE */
