#include <stdio.h>
#include <string.h>

#include "sys.h"
#include "rfid.h"
#include "usart.h"
#include "delay.h"
#include "can_interface.h"
#include "lock.h"

//MFRC522 mfrc522_B(chipSelectPinRfid2, resetPowerDownPinRfid2, &SPI_2, SPISettings(SPI_CLOCK_DIV4, MSBFIRST, SPI_MODE0, SPI2));
MFRC522 mfrc522_A(chipSelectPinRfid1, resetPowerDownPinRfid1, &SPI_1, SPISettings(SPI_CLOCK_DIV8, MSBFIRST, SPI_MODE0, SPI1));
static MFRC522 *mfrc522 = &mfrc522_A;

static can_message_t upload_msg;

static void upload_rfid_data(can_message_t *rfid_msg, const byte *buffer_type, const byte *buffer_key);
static void uart_print_type_and_key(byte *buffer_type, byte *buffer_key);


char rfid_in_flash[RFID_WORD_LENTH] = {0};

static void build_rfid(uint16_t rfid_int, char *rfid_str)
{
    if(rfid_int >= 1000)
    {
        sprintf(rfid_str,"%d",rfid_int);
    }
    else if(rfid_int >= 100)
    {
        sprintf(rfid_str,"0%d",rfid_int);
    }
    else if(rfid_int >= 10)
    {
        sprintf(rfid_str,"00%d",rfid_int);
    }
    else
    {
        sprintf(rfid_str,"000%d",rfid_int);
    }
}

static bool match_rfid(const char *rfid_in_flash, const char *rfid)
{
    for(u8 i = 0; i < RFID_WORD_LENTH; i++)
    {
        if(rfid_in_flash[i] != rfid[i])
        {
            return false;
        }
    }
    return true;
}

static void super_rfid_unlock_proc(const char *rfid_in_flash, const char *rfid)
{
    if(match_rfid(rfid_in_flash, rfid) == true)
    {
        start_to_unlock_all();
    }
}


static void init_exti(void)
{
    EXTI_InitTypeDef exit_init_structure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);
    exit_init_structure.EXTI_Line = EXTI_Line4;
    exit_init_structure.EXTI_Mode = EXTI_Mode_Interrupt;
    exit_init_structure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    exit_init_structure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exit_init_structure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource6);
    exit_init_structure.EXTI_Line = EXTI_Line6;
    EXTI_Init(&exit_init_structure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void rfid_init()
{
    SPI_1.begin();
    mfrc522 = &mfrc522_A;
    mfrc522->PCD_Init();
    mfrc522->PCD_SetAntennaGain(0x50);
    printf("RFID_A DB:0x%02x\r\n", mfrc522->PCD_GetAntennaGain());
    mfrc522->PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
    init_exti();
    return;
}


u32 rfid_start_tick = 0;
void rfid_task()
{

    mfrc522 = &mfrc522_A;

    // Look for new cards
    if (mfrc522->PICC_IsNewCardPresent())
    {
        // Select one of the cards
        if (mfrc522->PICC_ReadCardSerial())
        {
            // Dump debug info about the card; PICC_HaltA() is automatically called
#if 0 //this is for original test
            mfrc522->PICC_DumpToSerial(&(mfrc522->uid));
#else
            MFRC522::StatusCode status;
            MFRC522::MIFARE_Key key;
            for (byte i = 0; i < 6; i++)
            {
                key.keyByte[i] = 0xFF;
            }
            status = mfrc522->PCD_Authenticate(mfrc522->PICC_CMD_MF_AUTH_KEY_A, 0, &key, &mfrc522->uid);
            if (status != mfrc522->STATUS_OK)
            {
                printf("PCD_Authenticate() failed: %s\r\n", mfrc522->GetStatusCodeName(status));
                goto RFID_OUT;
            }

            byte buffer_type[18];
            byte buffer_key[18];
            byte byteCount;
            byte retryCount = 0;
            // Read block
            byteCount = sizeof(buffer_type);

            do
            {
                status = mfrc522->MIFARE_Read(1, buffer_type, &byteCount);
                if (status != mfrc522->STATUS_OK)
                {
                    printf("MIFARE_Read() failed: %s\r\n", mfrc522->GetStatusCodeName(status));
                    retryCount += 1;
                }
            } while (status != mfrc522->STATUS_OK && retryCount < 5);

            if (retryCount >= 5)
            {
                goto RFID_OUT;
            }
            retryCount = 0;

            byteCount = sizeof(buffer_key);
            do
            {
                status = mfrc522->MIFARE_Read(2, buffer_key, &byteCount);
                if (status != mfrc522->STATUS_OK)
                {
                    printf("MIFARE_Read() failed: %s\r\n", mfrc522->GetStatusCodeName(status));
                    retryCount += 1;
                }
            } while (status != mfrc522->STATUS_OK && retryCount < 5);

            if (retryCount >= 5)
            {
                goto RFID_OUT;
            }

            printf("uid: ");
            for( int i = 0; i < mfrc522->uid.size; i++)
            {
                printf("%02x", mfrc522->uid.uidByte[i]);
            }
            printf("\r\n");
//            uart_print_type_and_key(buffer_type, buffer_key);

            upload_rfid_data(&upload_msg, buffer_type, buffer_key);
            u16 rfid_int = buffer_key[14];
            rfid_int = rfid_int<<8;
            rfid_int += buffer_key[15];
            char rfid_str[10] = {0};
            build_rfid(rfid_int, rfid_str);
            super_rfid_unlock_proc(rfid_in_flash, rfid_str);

            rfid_start_tick = get_tick();

#endif
        }
    }
    RFID_OUT:
    mfrc522->PICC_HaltA(); // Halt the PICC before stopping the encrypted session.
    mfrc522->PCD_StopCrypto1();
    mfrc522->PICC_HaltA();

    return;
}

static void upload_rfid_data(can_message_t *rfid_msg, const byte *buffer_type, const byte *buffer_key)
{
    if (!rfid_msg || !buffer_type || !buffer_key) return;

    can_id_union id;
    id.can_id_struct.src_mac_id = LOCK_CAN_MAC_SRC_ID;
    id.can_id_struct.source_id = CAN_SOURCE_ID_RFID_UPLOAD;
    id.can_id_struct.res = 0;
    id.can_id_struct.ack = 0;
    id.can_id_struct.func_id = 0;

    rfid_msg->id = id.can_id;
    rfid_msg->data[0] = 0x00;
    memcpy( (void *)&rfid_msg->data[1], buffer_key + 12, 4 );
    rfid_msg->data_len = 5;
    can.can_send( rfid_msg );

    return;
}

static void uart_print_type_and_key(byte *buffer_type, byte *buffer_key)
{
    if (!buffer_type || !buffer_key) return;

    printf("buffer_type:");
    for (byte index = 0; index < 16; index++)
    {
        printf(" %02x", *(buffer_type + index));
        if ((index % 4) == 3)
        {
            printf(" ");
        }
    }
    printf("\r\n");

    printf("buffer_key:");
    for (byte index = 0; index < 16; index++)
    {
        printf(" %02x", *(buffer_type + index));
        if ((index % 4) == 3)
        {
            printf(" ");
        }
    }
    printf("\r\n");

    return;
}

