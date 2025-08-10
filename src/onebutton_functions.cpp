#include <Arduino.h>

#include "configuration.h"
#include "loop_functions_extern.h"
#include "loop_functions.h"
#include "command_functions.h"

#include "onebutton_functions.h"

#if defined(BOARD_T_DECK) || defined(BOARD_T_DECK_PLUS)
#include <lvgl.h>
#include <t-deck/tdeck_main.h>
#include <t-deck/lv_obj_functions.h>
#endif 

void singleClick()
{
  if(bDisplayCont)
    Serial.println("SingleClick");

    #ifdef BOARD_T5_EPAPER
        return;
    #endif

  // no oneclick on TRACK=on
  if(!bDisplayTrack)
  {
    if(bDisplayCont)
        Serial.printf("BUTTON singel press last:%i pointer:%i lines:%i\n", pageLastPointer, pagePointer, pageLastLineAnz[pagePointer]);

    if(pagePointer == 5)
      return;
      
    if(pageLastLineAnz[pagePointer] == 0)
      pagePointer = pageLastPointer-1;

    bDisplayIsOff=false;

    pageLineAnz = pageLastLineAnz[pagePointer];
    for(int its=0;its<pageLineAnz;its++)
    {
        // Save last Text (init)
        pageLine[its][0] = pageLastLine[pagePointer][its][0];
        pageLine[its][1] = pageLastLine[pagePointer][its][1];
        pageLine[its][2] = pageLastLine[pagePointer][its][2];
        memcpy(pageText[its], pageLastText[pagePointer][its], 25);
        if(its == 0)
        {
            for(int iss=0; iss < 20; iss++)
            {
                if(pageText[its][iss] == 0x00)
                    pageText[its][iss] = 0x20;
            }
            pageText[its][19] = pagePointer | 0x30;
            pageText[its][20] = 0x00;
        }
    }

    #ifdef BOARD_E290
        iDisplayType=9;
    #else
        iDisplayType=0;
    #endif

    strcpy(pageTextLong1, pageLastTextLong1[pagePointer]);
    strcpy(pageTextLong2, pageLastTextLong2[pagePointer]);

    sendDisplay1306(false, true, 0, 0, (char*)"#N");

    pagePointer--;
    if(pagePointer < 0)
        pagePointer=PAGE_MAX-1;

    pageHold=5;
  }
}

void doubleClick()
{

  if(bDisplayCont)
    Serial.println("DoubleClick");

  #ifdef BOARD_T5_EPAPER
      return;
  #endif

  if(bDisplayTrack)
      commandAction((char*)"--sendtrack", false);
  else
      commandAction((char*)"--sendpos", false);
}

void tripleClick()
{
  if(bDisplayCont)
    Serial.println("TripleClick");

  #ifdef BOARD_T5_EPAPER
    return;
  #endif

  bDisplayTrack=!bDisplayTrack;

  bDisplayIsOff=false;

  if(bDisplayTrack)
      commandAction((char*)"--track on", false);
  else
      commandAction((char*)"--track off", false);

  sendDisplayHead(false);
}

void PressLong()
{
  if(bDisplayCont)
    Serial.println("LongPress Stop");

  bShowHead=false;

  #ifdef BOARD_E290
      sendDisplayMainline();
      E290DisplayUpdate();
  #else
      pageHold=0;

      if(!bDisplayOff)
      {
          commandAction((char*)"--display off", isPhoneReady, false);
      }
      else
      {
          commandAction((char*)"--display on", isPhoneReady, false);
      }

  #endif
}

// setup code here, to run once:
void init_onebutton()
{
  if(bButtonCheck)
  {
    Serial.printf("[OBUT]...One Button GPIO(%i) started\n", iButtonPin);

/* Initialize a new OneButton instance for a button connected to digital
*  pin and GND, which is active low [2] and uses the internal pull-up resistor.
*/
    #if defined (BOARD_E290)
      btn.setup( /*GPIO=*/ iButtonPin, /*active LOW=*/ true, /*pull-up=*/ false);
    #else
      btn.setup( /*GPIO=*/ iButtonPin, /*active LOW=*/ true, /*pull-up=*/ true);
    #endif
    btn.attachClick(singleClick);         // Single Click event attachment
    btn.attachDoubleClick(doubleClick);   // Double Click event attachment
    btn.attachMultiClick(tripleClick);    // Multi Click event attachement
    btn.attachLongPressStart(PressLong);  // LongPress Click event attachement
  }
} // setup

// End
