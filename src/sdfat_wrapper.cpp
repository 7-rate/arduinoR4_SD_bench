#include <Arduino_FreeRTOS.h>
#include <stdlib.h>
#include <SPI.h>
#include "SdFat.h"
#include "sdfat_wrapper.h"
/******************************************************************/
/* Definitions                                                    */
/******************************************************************/
/***********************************/
/* Local definitions               */
/***********************************/
const uint8_t chipSelect = D4;
const uint32_t CLOCK_MHZ = 16;
const char* file_name_prefix = "log"; //ex:log0000.csv
#define BUFFER_NUM (2)

typedef struct {
    byte buffer[512];
    uint16_t index;
} st_sd_buffer;

/***********************************/
/* Local Variables                 */
/***********************************/
SdFat sd;
SdFile file;
byte writing_index;
byte flushing_index;
st_sd_buffer buffer[BUFFER_NUM];
SemaphoreHandle_t semaphore = NULL;

/***********************************/
/* Global Variables                */
/***********************************/

/******************************************************************/
/* Implementation                                                 */
/******************************************************************/
/***********************************/
/* Local functions                 */
/***********************************/
static void flush_task(void* param) {
    Serial.println("flush_task_start");
    semaphore = xSemaphoreCreateBinary();
    while(1) {
        //semaphore
        xSemaphoreTake(semaphore, portMAX_DELAY);
        Serial.print(micros());
        Serial.print(",");
        Serial.println("semaphore_take:");
        file.write(buffer[flushing_index].buffer, 512);
        Serial.print(micros());
        Serial.print(",");
        Serial.println("write_end:");
        file.sync();
        buffer[flushing_index].index = 0;
        Serial.print(micros());
        Serial.print(",");
        Serial.println("flush_end");
    }
}

/***********************************/
/* Global functions                */
/***********************************/
bool sd_start() {
    writing_index = 0;
    flushing_index = 0;
    memset(buffer, 0, sizeof(buffer));
    xTaskCreate(flush_task, "flush_task", 256, NULL, 3, NULL);
    return sd.begin(chipSelect, SD_SCK_MHZ(CLOCK_MHZ));
}

bool sd_make_csv_log(const char *header) {
    uint8_t file_number = 0;
    char file_name[12] = {0};
    sprintf(file_name, "%s%04d.csv", file_name_prefix, file_number);
    while (sd.exists(file_name)) {
        file_number++;
        sprintf(file_name, "%s%04d.csv", file_name_prefix, file_number);
    }
    if (!file.open(file_name, O_CREAT | O_WRITE | O_TRUNC)) {
        return false;
    }
    if (!file.println(header)) {
        return false;
    }
    return true;
}

bool sd_write_csv_log(const char *data, size_t len) {
    Serial.print(micros());
    Serial.print(",");
    Serial.print("sd_write_csv_log,");
    if (buffer[writing_index].index + len >= 512) {
        flushing_index = writing_index;
        Serial.println();
        Serial.print(micros());
        Serial.print(",");
        Serial.println("semaphore_give");
        xSemaphoreGive(semaphore);
        writing_index = (writing_index + 1) % BUFFER_NUM;
    }
    memcpy(buffer[writing_index].buffer + buffer[writing_index].index, data, len);
    buffer[writing_index].index += len;
    Serial.print(writing_index);
    Serial.print(",");
    Serial.println(buffer[writing_index].index);

    return true;
}

void sd_end() {
    file.close();
}


