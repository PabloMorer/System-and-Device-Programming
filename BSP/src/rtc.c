
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <rtc.h>

extern void isr_TICK_dummy( void );

void rtc_init( void )
{
    TICNT   = 0;
    RTCALM  = 0;    
    RTCRST  = 0; 
        
    RTCCON  |= (1 << 0);
    
    BCDYEAR = 0x13;
    BCDMON  = 1;
    BCDDAY  = 1;
    BCDDATE = 1;
    BCDHOUR = 00;
    BCDMIN  = 00;
    BCDSEC  = 00;

    ALMYEAR = 0;
    ALMMON  = 0;
    ALMDAY  = 0;
    ALMHOUR = 0;
    ALMMIN  = 0;
    ALMSEC  = 0;

    RTCCON &= ~(1 << 0);
}

void rtc_puttime( rtc_time_t *rtc_time )
{
    RTCCON |= (1 << 0) ;
    
    BCDYEAR = ((rtc_time->year/10) << 4 )| (rtc_time->year%10);
    BCDMON  = ((rtc_time->mon/10) << 4 )| (rtc_time->mon%10);
    BCDDAY  = ((rtc_time->mday/10) << 4 )| (rtc_time->mday%10);
    BCDDATE = ((rtc_time->wday/10) << 4 )| (rtc_time->wday%10);
    BCDHOUR = ((rtc_time->hour/10) << 4 )| (rtc_time->hour%10);
    BCDMIN  = ((rtc_time->min/10) << 4 )| (rtc_time->min%10);
    BCDSEC  = ((rtc_time->sec/10) << 4 )| (rtc_time->sec%10);
        
    RTCCON &= ~(1 << 0);
}

void rtc_gettime( rtc_time_t *rtc_time )
{
    RTCCON |= (1 << 0);
    
    rtc_time->year = ((BCDYEAR >> 4 ) * 10 ) + (BCDYEAR & (0xF) );
    rtc_time->mon  = ((BCDMON >> 4 ) * 10 ) + (BCDMON & (0xF) );
    rtc_time->mday = ((BCDDAY >> 4 ) * 10 ) + (BCDDAY & (0xF) );
    rtc_time->wday = ((BCDDATE >> 4 ) * 10 ) + (BCDDATE & (0xF) );
    rtc_time->hour = ((BCDHOUR >> 4 ) * 10 ) + (BCDHOUR & (0xF) );
    rtc_time->min  = ((BCDMIN >> 4 ) * 10 ) + (BCDMIN & (0xF) );
    rtc_time->sec  = ((BCDSEC >> 4 ) * 10 ) + (BCDSEC & (0xF) );
    if( ! rtc_time->sec ){
        rtc_time->year = BCDYEAR;
        rtc_time->mon  = BCDMON;
        rtc_time->mday = BCDDAY;
        rtc_time->wday = BCDDATE;
        rtc_time->hour = BCDHOUR;
        rtc_time->min  = BCDMIN;
        rtc_time->sec  = BCDSEC;
    };

    RTCCON &= ~(1 << 0);
}


void rtc_open( void (*isr)(void), uint8 tick_count )
{
    pISR_TICK = (uint32) isr;
    I_ISPC    = BIT_TICK;
    INTMSK   &= ~(BIT_GLOBAL | BIT_TICK);
    TICNT     = (1 << 7) | tick_count;
}

void rtc_close( void )
{
    TICNT     = 0;  
    INTMSK   |= BIT_TICK ;    
    pISR_TICK = (uint32) isr_TICK_dummy ;
}

