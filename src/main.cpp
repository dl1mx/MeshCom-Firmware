#include <Arduino.h>

#if defined(BOARD_T_ECHO) || defined(BOARD_T5_EPAPER)
#else
  #include <SPI.h>
#endif

#include <configuration.h>

#ifdef ESP32
  #include <esp32/esp32_main.h>
#endif

#if defined(BOARD_RAK4630)
  #include <nrf52/nrf52_main.h>
#endif

#if defined(BOARD_T_ECHO)
  #include <nrf52_techo/nrf52_main.h>
#endif

void setup()
{
  #if defined(BOARD_T5_EPAPER)
    if (psramInit()) {
        Serial.println("\nThe PSRAM is correctly initialized");
    } else {
        Serial.println("\nPSRAM does not work");
    }
  #endif

  #if !defined(BOARD_T_ECHO) && !defined(BOARD_T_DECK) && !defined(BOARD_T_DECK_PLUS) && !defined(BOARD_T5_EPAPER)
    SPI.begin();
  #endif

  #if defined(BOARD_RAK4630)
    nrf52setup();
  #endif

  #if defined(BOARD_T_ECHO)
    nrf52setup();
  #endif

  #ifdef ESP32
    esp32setup();
  #endif
}

void loop()
{

  #if defined(BOARD_T5_EPAPER)
    esp32loop();
  #endif  

  #if defined(BOARD_RAK4630)
    nrf52loop();
  #endif

  #if defined(BOARD_T_ECHO)
    nrf52loop();
  #endif

  #ifdef ESP32
    esp32loop();
  #endif

}