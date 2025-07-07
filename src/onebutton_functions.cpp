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

/*

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
)
*/

volatile int iButtonState=0;

// save the millis when a press has started.
volatile unsigned long pressStartTime=0;

// This function is called from the interrupt when the signal on the PIN_INPUT has changed.
// do not use Serial in here.
#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif
void checkTicks(void)
{
  // include all buttons here to be checked
  bool bButtonState = (digitalRead(iButtonPin) == LOW);
  
  //Serial.print("button int ");
  //Serial.println(bButtonState);

  pressStartTime = millis();

  if(bButtonState)
    iButtonState++;
}

// setup code here, to run once:
void init_onebutton()
{
  if(bButtonCheck)
  {
    Serial.println("[OBUT]...One Button with interrupts started");

    #if defined (BOARD_E290)
      pinMode(iButtonPin, INPUT);
    #else
      pinMode(iButtonPin, INPUT_PULLUP);
    #endif

    attachInterrupt(digitalPinToInterrupt(iButtonPin), checkTicks, CHANGE);
  }
} // setup

// main code here, to run repeatedly:
void loop_onebutton()
{
  // keep watching the push button, even when no interrupt happens:
  if(bButtonCheck)
  {
    if((pressStartTime + 300) < millis())
    {
      switch (iButtonState)
      {
          case 1: singleClick(); break;
          case 2: doubleClick(); break;
          case 3: tripleClick(); break;
          default: break;
      }

      iButtonState=0;
    }
  }
} // loop


// End
