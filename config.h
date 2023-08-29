/************************ Adafruit IO Config *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME  "majosuarez"
#define IO_KEY       "aio_jdcc75YEEyzJO4hkWdBcp0G2FJy2"

/******************************* WIFI **************************************/



#define WIFI_SSID "Majo"
#define WIFI_PASS "12345689"


#include "AdafruitIO_WiFi.h"


AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
