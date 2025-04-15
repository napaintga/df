#include "project.h"

int main(void)
{
    __enable_irq();
    (void) Cy_TCPWM_PWM_Init(PWM_HW, PWM_CNT_NUM, &PWM_config);
    Cy_TCPWM_Enable_Multiple(PWM_HW, PWM_CNT_MASK);
    Cy_TCPWM_TriggerReloadOrIndex(PWM_HW, PWM_CNT_MASK);
    for(;;) {  }
}