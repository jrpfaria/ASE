#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"/
#include "sdkconfig.h"
#define z(a) {gpio_reset_pin(a);gpio_set_direction(a,GPIO_MODE_OUTPUT);}
#define s(a,b) {gpio_set_level(a,b);}
#define f for(j=0;j<10;j++)
#define x (j[g])-1
#define v void
#define i int
#define c char
#define w vTaskDelay(10/portTICK_PERIOD_MS);

c* g="	";c* l="?[Ofm}ow|9^yq";i j;v h(v){f z(x)}
c y=0;v p(c d){y=!y;f if(j<8)s(x,(((y?(d>>4):(d&15))[l])&(1<<j)))else s(x,(j>9?y:!y))}
v app_main(v){h();c q=0;i t=0;for(;;){if(t++==99){t=0;q++;};p(q);w}}
