/*
    Accepts inputs in terminal between -100 and 100
    This will be applied to motors with -100 being full power backwards and 100
    full power forwards
*/
#include <stdio.h>
#include <string.h>
#include "usb_serial.h"
#include "pio.h"
#include "delay.h"
#include "panic.h"

#define DELAY_MS 10

int main(void) {

    int i = 0;
    // Redirect stdio to USB serial
    if (usb_serial_stdio_init() < 0)
        panic(LED_ERROR_PIO, 3);

    while (1) 
    {
        delay_ms(DELAY_MS);
        char buf[256];
        if (fgets(buf, sizeof(buf), stdin)) {
            int num;
            if (sscanf(buf, "%d", &num) == 1) {
                if (num >= -100 && num <= 100) {
                    printf("You entered: %d\n", num);
                }
                else {
                    printf("Please enter -100 - 100 No.\n");
                }
            } else {
                printf("Invalid input\n");
                // Clear input buffer
                // while (getchar() != '\n');
            }
        } 
    }
}
