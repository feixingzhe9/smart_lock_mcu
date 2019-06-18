#ifndef __SYSTEM_DELAY_H
#define __SYSTEM_DELAY_H
#include "sys.h"
#ifdef __cplusplus
extern "C" {
#endif

void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);


#define asm            __asm

#define delay_300ns()     do {asm("nop");asm("nop");asm("nop");asm("nop");\
                              asm("nop");asm("nop");asm("nop");asm("nop");\
                              asm("nop");asm("nop");asm("nop");asm("nop");\
                              asm("nop");asm("nop");asm("nop");asm("nop");\
                              asm("nop");asm("nop");\
                              asm("nop");asm("nop");} while(1==0)

#define delay_600ns()     do { asm("nop");asm("nop");asm("nop");asm("nop");\
                               asm("nop");asm("nop");asm("nop");asm("nop");\
                               asm("nop");asm("nop");asm("nop");asm("nop");\
                               asm("nop");asm("nop");asm("nop");asm("nop");\
                               asm("nop");asm("nop");asm("nop");asm("nop");\
                               asm("nop");asm("nop");asm("nop");asm("nop");\
                               asm("nop");asm("nop");asm("nop");asm("nop");\
                               asm("nop");asm("nop");asm("nop");asm("nop");\
                               asm("nop");asm("nop");asm("nop");asm("nop");\
                               asm("nop");asm("nop");asm("nop");asm("nop");\
                               asm("nop");asm("nop");asm("nop");asm("nop");\
                               asm("nop");asm("nop");} while(1==0)

#define delay_high_0()      do {\
                                asm("nop");asm("nop");asm("nop");;asm("nop");\
                                asm("nop");;asm("nop");\
                                } while(1==0)

#define delay_low_0()       do {\
                                asm("nop");asm("nop");asm("nop");asm("nop");\
                                asm("nop");asm("nop");asm("nop");asm("nop");\
                                asm("nop");asm("nop");asm("nop");asm("nop");\
                                asm("nop");asm("nop");asm("nop");asm("nop");\
                              } while(1==0)

#define delay_high_1()        do {\
                                asm("nop");asm("nop");asm("nop");asm("nop");\
                                asm("nop");asm("nop");asm("nop");asm("nop");\
                                asm("nop");asm("nop");asm("nop");asm("nop");\
                                asm("nop");asm("nop");asm("nop");asm("nop");\
                                asm("nop");asm("nop");asm("nop");asm("nop");\
                                asm("nop");asm("nop");asm("nop");asm("nop");\
                                asm("nop");asm("nop");asm("nop");asm("nop");\
                                asm("nop");asm("nop");asm("nop");asm("nop");\
                                asm("nop");asm("nop");\
                                } while(1==0)

#define delay_low_1()       do {\
                            asm("nop");asm("nop");asm("nop");asm("nop");\
                            } while(1==0)

#define delay_us_ex(n)       do {\
                                    uint32_t i=0;\
                                    for(i=0; i < n; i++)\
                                    {\
                                        delay_300ns();\
                                        delay_600ns();\
                                    }\
                                }while(0==1);
#ifdef __cplusplus
  }
#endif
#endif


