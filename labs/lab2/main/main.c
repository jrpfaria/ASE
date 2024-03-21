#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#define rs(a) {gpio_reset_pin(a);gpio_set_direction(a,GPIO_MODE_OUTPUT);}
#define s(a,b) {gpio_set_level(a,b);}
#define f for(j=0;j<10;j++)
#define x (j[g])-1
#define v void
#define i int
#define c char
#define delay vTaskDelay(10/portTICK_PERIOD_MS);

c* g="	";c* l="?[Ofm}ow|9^yq";i j;
v ciop(v){f rs(x)}
c y=0; v dd(c d){f if(j<8)s(x,(((y?(d&15):(d>>4))[l])&(1<<j)))else s(x,!y) y=!y;}
v app_main(v){ciop();c cnt=0;i t=0;for(;;){if(t++==99){t=0;cnt++;};dd(cnt);delay}}
