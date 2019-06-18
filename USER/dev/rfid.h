#ifndef __RFID_H
#define __RFID_H



#define RFID_WORD_LENTH     4

void rfid_init();
void rfid_proc();
extern char rfid_in_flash[RFID_WORD_LENTH];

#endif

