//#include "delay.h"
//#include "sys.h"
////////////////////////////////////////////////////////////////////////////////////

//#if SYSTEM_SUPPORT_UCOS
//#include "includes.h"                   //ucos ʹԃ
//#endif

//static u8  fac_us=0;
//static u16 fac_ms=0;
//#ifdef OS_CRITICAL_METHOD

//void SysTick_Handler(void)
//{
//    OSIntEnter();
//    OSTimeTick();
//    OSIntExit();
//}
//#endif

//void delay_init()
//{

//#ifdef OS_CRITICAL_METHOD
//    u32 reload;
//#endif
//    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
//    fac_us = SystemCoreClock / 8000000;

//#ifdef OS_CRITICAL_METHOD
//    reload = SystemCoreClock / 8000000;
//    reload *= 1000000 / OS_TICKS_PER_SEC;
//    fac_ms = 1000 / OS_TICKS_PER_SEC;
//    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
//    SysTick->LOAD = reload;
//    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
//#else
//    fac_ms = (u16)fac_us * 1000;
//#endif
//}

////void delay_us(u32 nus)
////{
////    u32 ticks;
////    u32 told, tnow, tcnt = 0;
////    u32 reload = SysTick->LOAD;
////    ticks=nus * fac_us;
////    tcnt = 0;
////    told = SysTick->VAL;
////    while(1)
////    {
////        tnow = SysTick->VAL;
////        if(tnow != told)
////        {
////            if(tnow<told)tcnt += told - tnow;
////            else tcnt += reload - tnow + told;
////            told = tnow;
////            if(tcnt >= ticks)break;
////        }
////    }
////}

//void delay_ms(uint16_t nms)
//{
//    if(OSRunning == TRUE)
//    {
//        if(nms >= fac_ms)
//        {
//            OSTimeDly(nms / fac_ms);
//        }
//        nms %= fac_ms;
//    }
//    if(nms > 0)
//    {
//        delay_us_ex((u32)(nms * 1000));
//    }
//}



































#include "delay.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
//ɧڻʹԃucos,ղѼ(ЂĦքͷτݾܴࠉ.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"                   //ucos ʹԃ
#endif

static u8  fac_us=0;//usғʱѶԋ˽
static u16 fac_ms=0;//msғʱѶԋ˽
#ifdef OS_CRITICAL_METHOD   //ɧڻOS_CRITICAL_METHOD֨ӥ,˵ķʹԃucosII.
//systickא׏ؾϱگ˽,ʹԃucosʱԃս
void SysTick_Handler(void)
{
    OSIntEnter();       //޸ɫא׏
    OSTimeTick();       //ַԃucosքʱדؾϱԌѲ
    OSIntExit();        //ԥעɎϱȐۻɭא׏
}
#endif

//Եʼۯғԙگ˽
//ձʹԃucosքʱ۲,Ջگ˽ܡԵʼۯucosքʱדޚƄ
//SYSTICKքʱדڌ֨ΪHCLKʱדք1/8
//SYSCLK:ϵͳʱד
void delay_init()
{

#ifdef OS_CRITICAL_METHOD   //ɧڻOS_CRITICAL_METHOD֨ӥ,˵ķʹԃucosII.
    u32 reload;
#endif
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);   //ѡձ΢ҿʱד  HCLK/8
    fac_us = SystemCoreClock / 8000000;     //Ϊϵͳʱדք1/8

#ifdef OS_CRITICAL_METHOD       //ɧڻOS_CRITICAL_METHOD֨ӥ,˵ķʹԃucosII.
    reload = SystemCoreClock / 8000000;     //ÿīדք݆˽Վ˽ եλΪK
    reload *= 1000000 / OS_TICKS_PER_SEC;   //ٹߝOS_TICKS_PER_SECʨ֨ӧԶʱݤ
                                            //reloadΪ24λ݄զǷ,خճֵ:16777216,՚72MЂ,Լۏ1.86sسԒ
    fac_ms = 1000 / OS_TICKS_PER_SEC;       //պҭucosࠉӔғʱքخʙեλ
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  //ߪǴSYSTICKא׏
    SysTick->LOAD = reload;     //ÿ1/OS_TICKS_PER_SECīא׏һՎ
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   //ߪǴSYSTICK
#else
    fac_ms = (u16)fac_us * 1000;//؇ucosЂ,պҭÿٶmsѨҪքsystickʱד˽
#endif
}

#ifdef OS_CRITICAL_METHOD   //ʹԃucos
//ғʱnus
//nusΪҪғʱքus˽.
void delay_us(u32 nus)
{
    u32 ticks;
    u32 told, tnow, tcnt = 0;
    u32 reload = SysTick->LOAD;     //LOADքֵ
    ticks=nus * fac_us;             //ѨҪքޚƄ˽
    tcnt = 0;
    told = SysTick->VAL;            //ٕ޸ɫʱք݆˽Ƿֵ
    while(1)
    {
        tnow = SysTick->VAL;
        if(tnow != told)
        {
            if(tnow<told)tcnt += told - tnow;//֢oעӢһЂSYSTICKˇһٶ֝ݵք݆˽ǷߍࠉӔ.
            else tcnt += reload - tnow + told;
            told = tnow;
            if(tcnt >= ticks)break;//ʱݤӬڽ/ֈԚҪғԙքʱݤ,ղ΋Զ.
        }
    }
}
//ғʱnms
//nms:Ҫғʱքms˽
void delay_ms(u16 nms)
{
    if(OSRunning == TRUE)//ɧڻosӑޭ՚Ɯ
    {
        if(nms >= fac_ms)//ғʱքʱݤճԚucosքخʙʱݤלǚ
        {
            OSTimeDly(nms / fac_ms);//ucosғʱ
        }
        nms %= fac_ms;              //ucosӑޭϞר͡٩֢ôСքғʱ,ӉԃǕͨ׽ʽғʱ
    }
    if(nms > 0)
    {
        delay_us_ex((u32)(nms * 1000));    //Ǖͨ׽ʽғʱ,ՋʱucosϞרǴַ֯׈.
    }
}
#else//һԃucosʱ
//ғʱnus
//nusΪҪғʱքus˽.
void delay_us(u32 nus)
{
    u32 temp = 0;
    SysTick->LOAD = nus * fac_us;   //ʱݤݓ՘
    SysTick->VAL = 0x00;            //ȥࠕ݆˽Ƿ
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;      //ߪʼչ˽
    do
    {
        temp = SysTick->CTRL;
    }
    while(temp & 0x01 && !(temp & (1 << 16)));//ֈսʱݤսկ
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  //ژҕ݆˽Ƿ
    SysTick->VAL = 0X00;    //ȥࠕ݆˽Ƿ
}
//ғʱnms
//עӢnmsք׶Χ
//SysTick->LOADΪ24λ݄զǷ,̹Ӕ,خճғʱΪ:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLKեλΪHz,nmsեλΪms
//ה72M͵ݾЂ,nms<=1864
void delay_ms(u16 nms)
{
    u32 temp;
    SysTick->LOAD = (u32)nms * fac_ms;//ʱݤݓ՘(SysTick->LOADΪ24bit)
    SysTick->VAL = 0x00;        //ȥࠕ݆˽Ƿ
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;  //ߪʼչ˽
    do
    {
        temp = SysTick->CTRL;
    }
    while(temp & 0x01 && !(temp & (1 << 16)));//ֈսʱݤսկ
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  //ژҕ݆˽Ƿ
    SysTick->VAL = 0X00;        //ȥࠕ݆˽Ƿ
}
#endif


