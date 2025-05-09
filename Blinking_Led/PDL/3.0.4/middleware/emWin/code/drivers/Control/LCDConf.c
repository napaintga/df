/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.46 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software  has been licensed to  Cypress Semiconductor Corporation,
whose registered  office is situated  at 198 Champion Ct. San Jose, CA 
95134 USA  solely for the  purposes of creating  libraries for Cypress
PSoC3 and  PSoC5 processor-based devices,  sublicensed and distributed
under  the  terms  and  conditions  of  the  Cypress  End User License
Agreement.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Microcontroller Systems LLC
Licensed to:              Cypress Semiconductor Corp, 198 Champion Ct., San Jose, CA 95134, USA
Licensed SEGGER software: emWin
License number:           GUI-00319
License model:            Services and License Agreement, signed June 10th, 2009
Licensed platform:        Any Cypress platform (Initial targets are: PSoC3, PSoC5)
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2009-06-12 - 2022-07-27
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

#include <project.h>
#include "GUI.h"
#include "LCDConf.h"


/*********************************************************************
*       Layer configuration
*********************************************************************/

/* Physical display size */
#define XSIZE_PHYS          240
#define YSIZE_PHYS          320

/* Virtual display size */
#define VXSIZE_PHYS          240
#define VYSIZE_PHYS          320

/* Color conversion */
#define COLOR_CONVERSION    GUICC_M565

/* Display driver */
#define DISPLAY_DRIVER GUIDRV_TEMPLATE


/*********************************************************************
*       Configuration checking
*********************************************************************/

#ifndef   VXSIZE_PHYS
  #define VXSIZE_PHYS XSIZE_PHYS
#endif
#ifndef   VYSIZE_PHYS
  #define VYSIZE_PHYS YSIZE_PHYS
#endif
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   COLOR_CONVERSION
  #error Color conversion not defined!
#endif
#ifndef   DISPLAY_DRIVER
  #error No display driver defined!
#endif
#ifndef   LCD_SWAP_XY
  #define LCD_SWAP_XY 0
#endif
#if LCD_SWAP_XY
  #define LCD_XSIZE YSIZE_PHYS
  #define LCD_YSIZE XSIZE_PHYS
  #define LCD_VXSIZE VYSIZE_PHYS
  #define LCD_VYSIZE VXSIZE_PHYS
#else
  #define LCD_XSIZE XSIZE_PHYS
  #define LCD_YSIZE YSIZE_PHYS
  #define LCD_VXSIZE VXSIZE_PHYS
  #define LCD_VYSIZE VYSIZE_PHYS
#endif


/*********************************************************************
*
*       Static code
*
*********************************************************************/

/*********************************************************************
*
*       _InitController
*
* Purpose:
*  Set up the display controller and put it into operation. If the 
*  display controller is not initialized by any external routine 
*  this needs to be adapted by the customer.
*
*/
static void _InitController(void) {}

/*********************************************************************
*
*       _SetOrg
*
* Purpose:
*   Sett the origin of the display by modifying the frame buffer base 
*   address register
*/
static void _SetOrg(int xPos, int yPos) {
  int x, y;
  #if   (!LCD_MIRROR_X && !LCD_MIRROR_Y && !LCD_SWAP_XY)
    #define LOG2PHYS_X(x, y) x
    #define LOG2PHYS_Y(x, y) y
  #elif (!LCD_MIRROR_X && !LCD_MIRROR_Y &&  LCD_SWAP_XY)
    #define LOG2PHYS_X(x, y) y
    #define LOG2PHYS_Y(x, y) x
  #elif (!LCD_MIRROR_X &&  LCD_MIRROR_Y && !LCD_SWAP_XY)
    #define LOG2PHYS_X(x, y) x
    #define LOG2PHYS_Y(x, y) LCD_VYSIZE - LCD_YSIZE - (y)
  #elif (!LCD_MIRROR_X &&  LCD_MIRROR_Y &&  LCD_SWAP_XY)
    #define LOG2PHYS_X(x, y) y
    #define LOG2PHYS_Y(x, y) LCD_VXSIZE - LCD_XSIZE - (x)
  #elif ( LCD_MIRROR_X && !LCD_MIRROR_Y && !LCD_SWAP_XY)
    #define LOG2PHYS_X(x, y) LCD_VXSIZE - LCD_XSIZE - (x)
    #define LOG2PHYS_Y(x, y) y
  #elif ( LCD_MIRROR_X && !LCD_MIRROR_Y &&  LCD_SWAP_XY)
    #define LOG2PHYS_X(x, y) LCD_VYSIZE - LCD_YSIZE - (y)
    #define LOG2PHYS_Y(x, y) x
  #elif ( LCD_MIRROR_X &&  LCD_MIRROR_Y && !LCD_SWAP_XY)
    #define LOG2PHYS_X(x, y) LCD_VXSIZE - LCD_XSIZE - (x)
    #define LOG2PHYS_Y(x, y) LCD_VYSIZE - LCD_YSIZE - (y)
  #elif ( LCD_MIRROR_X &&  LCD_MIRROR_Y &&  LCD_SWAP_XY)
    #define LOG2PHYS_X(x, y) LCD_VYSIZE - LCD_YSIZE - (y)
    #define LOG2PHYS_Y(x, y) LCD_VXSIZE - LCD_XSIZE - (x)
  #endif

  x = LOG2PHYS_X(xPos, yPos);
  y = LOG2PHYS_Y(xPos, yPos);
  
  while(CYGRAPHICS_IS_VBLANKING){
    /* Wait until an active period before changing the frame buffer
    *  address to be sure that the complete address will be updated
    *  before it is used.
    */    
  }
  CYGRAPHICS_WRITE_FRAME(x + y * VXSIZE_PHYS);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/
void LCD_X_Config(void) {
  /* Set display driver and color conversion */
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
  
  /* Display driver configuration */
  
  LCD_SetSizeEx (0, LCD_XSIZE,   LCD_YSIZE);
  LCD_SetVSizeEx(0, LCD_VXSIZE,  LCD_VYSIZE);
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task, the routine needs to be adapted to
*   the display controller. Note that the commands marked
*   "optional" are not cogently required and should only be adapted if 
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Zero based layer index
*   Cmd        - Command to be executed
*   pData      - Pointer to a data structure.
* 
* Return Value:
*   < -1 - Error
*     -1 - The command is not handled.
*      0 - OK.
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;
  GUI_USE_PARA(LayerIndex);
  switch (Cmd) {
  //
  // Required
  //
  case LCD_X_INITCONTROLLER: {
    //
    // Called during the initialization process in order to set up the
    // display controller and put it into operation. If the display
    // controller is not initialized by any external routine, this needs
    // to be adapted by the customer...
    //
    _InitController();
    r = 0;
    break;
    }

  case LCD_X_SETORG: {
    //
    // Required for setting the display origin which is passed 
    // in the 'xPos' and 'yPos' element of p
    //
    LCD_X_SETORG_INFO * p;
    p = (LCD_X_SETORG_INFO *)pData;
    _SetOrg(p->xPos, p->yPos);
    r = 0;
    break;
    }
  
  default:
    r = -1;
  }
  return r;
}

/*************************** End of file ****************************/
