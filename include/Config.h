#pragma once

#ifndef MDNS_NAME
    #error MDNS_NAME need to setting to operate
#endif

#if ( USE_DISPLAY == 0 || USE_DISPLAY == 1 ) && !defined(I2C_OLED_ADDRESS) 
    #error I2C_OLED_ADDRESS need to be set for I2C diplay oled
#endif

#ifndef MESSAGE
    #define MESSAGE Ready for OTA:
#endif
#define MESSAGE_STR STR(MESSAGE)

//***************************//
#define STR2(x) #x
#define STR(x) STR2(x)
#if defined(USE_DISPLAY) && USE_DISPLAY == 0
    #define USE_DISPLAY_STR "SSD1306Wire" 
#elif defined(USE_DISPLAY) && USE_DISPLAY == 1
    #define USE_DISPLAY_STR "SH1106Wire" 
#elif defined(USE_DISPLAY) && USE_DISPLAY == 2
    #define USE_DISPLAY_STR "SSD1306Spi" 
#elif defined(USE_DISPLAY) && USE_DISPLAY == 3
    #define USE_DISPLAY_STR "SH1106SPi"
#elif defined(USE_DISPLAY)
    #error USE_DISPLAY invalid value!    
#endif
#define MDNS_NAME_STR STR(MDNS_NAME)

//***************************//
#ifdef ESP01_1M
    #define SDA         0
    #define SCL         2
#endif
