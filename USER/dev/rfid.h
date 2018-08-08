#ifndef __RFID_H
#define __RFID_H

#include "MFRC522.h"

#define RFID_WORD_LENTH     4

void rfid_init();
void rfid_task();
extern char rfid_in_flash[RFID_WORD_LENTH];

#endif

