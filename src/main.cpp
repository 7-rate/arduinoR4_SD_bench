#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include "sdfat_wrapper.h"
/******************************************************************/
/* Definitions                                                    */
/******************************************************************/
/***********************************/
/* Local definitions               */
/***********************************/

/***********************************/
/* Local Variables                 */
/***********************************/

/***********************************/
/* Global Variables                */
/***********************************/

/******************************************************************/
/* Implementation                                                 */
/******************************************************************/
/***********************************/
/* Local functions                 */
/***********************************/
void sd_task(void *pvParameters) {
    // int32_t diff;
    Serial.println("sd_task_start");
    while(1){
        sd_write_csv_log("0,1,2\n", sizeof("0,1,2\n") - 1); //dummy data

        if (Serial.available()) {
            sd_end();
            Serial.println(F("Done"));
            while (true) {}
        }
    }
}

/***********************************/
/* Global functions                */
/***********************************/
void setup() {
    Serial.begin(115200);
    while (!Serial) {}

    Serial.println(F("Type any character to start"));
    while (!Serial.available()) {}

    sd_start();
    sd_make_csv_log("test0,test1,test2");

    while (Serial.available() && Serial.read() >= 0) {}

    Serial.println(F("Type any character to stop"));

    xTaskCreate(sd_task, "sd_task", 256, NULL, 2, NULL);

    vTaskStartScheduler();
}


void loop() {

}
