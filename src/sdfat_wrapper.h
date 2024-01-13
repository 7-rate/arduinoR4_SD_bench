#pragma once
/******************************************************************/
/* Definitions                                                    */
/******************************************************************/
/***********************************/
/* Global definitions              */
/***********************************/
const uint32_t SAMPLE_INTERVAL_MS = 5;

extern SemaphoreHandle_t semaphore;


/***********************************/
/* Class                           */
/***********************************/

/***********************************/
/* Global functions                */
/***********************************/

/***********************************/
/* Global Variables                */
/***********************************/
bool sd_start();
bool sd_make_csv_log(const char *header);
bool sd_write_csv_log(const char *data, size_t len);
void sd_end();


