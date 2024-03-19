#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#define A 2
#define B 3
#define C 8
#define D 5
#define E 4
#define F 7
#define G 6
#define DP 10
#define D0 0
#define D1 1
#define rs(a) {gpio_reset_pin(a);gpio_set_direction(a,GPIO_MODE_OUTPUT);}
#define s(a,b) {gpio_set_level(a,b);}
#define sw(a) switch(a)
#define cr(a,b) case a: return (char)b;
#define n(a) a=!a
#define dr default: return 0;
#define v void
#define i int
#define c char
#define delay vTaskDelay(10/portTICK_PERIOD_MS)
                                                                                                          v ciop(v){rs(A)rs(B)rs(C)rs(D)rs(E)rs(F)rs(G)rs(DP)rs(D0)rs(D1)}
        c d2s(c d){sw(d){cr(0,63)cr(1,6)cr(2,91)cr(3,79)cr(4,102)cr(5,109)cr(6,125)cr(7,7)cr(8,127)cr(9,111)cr(10,119)cr(11,124)cr(12,57)cr(13,94)cr(14,121)cr(15,113)dr}}
                                                                                                                                       c a;v sd(v){n(a);s(D0,!a);s(D1,a);}                                                                                     
                                                    v dd(c d){c seg=d2s(d);s(A,(seg&1));s(B,(seg&2));s(C,(seg&4));s(D,(seg&8));s(E,(seg&16));s(F,(seg&32));s(G,(seg&64));}
                                                                                                                                                    v ddp(c st){s(DP,st);}                                                                      
                                    v app_main(v){ciop();c cnt=0;i t=0;i dp=0;for(;;){if(t++==100){t=0;cnt++;};dd(a?(cnt>>4):(cnt&15));ddp(a?(dp>>1):(dp&1));sd();delay;}}
