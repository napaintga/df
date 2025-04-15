#include "project.h"

int main(void)
{
    __enable_irq();
    Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR);
    for(;;) {  }
}
