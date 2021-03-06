
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <pbs.h>
#include <timers.h>

extern void isr_PB_dummy( void );

void pbs_init( void )
{
    timers_init();
}

uint8 pb_scan( void )
{
    if( PDATG & (1 << 6) )
        return PB_RIGHT;
    else if( PDATG & (1 << 7) )
        return PB_LEFT;
    else
        return PB_FAILURE;
}

uint8 pb_status( uint8 scancode )
{


switch(scancode){
	case PB_LEFT:
			if(PDATG & (1 << 6)){
				return  PB_DOWN ;
			}else{
				return PB_UP;
			}
			 break;
   case PB_RIGHT:
			 if(PDATG & (1 << 7)){
				return  PB_DOWN ;
			 }else{
				return PB_UP;
			 }
			 break;
 }
}

void pb_wait_keydown( uint8 scancode )
{



	while(PDATG & scancode);


	 sw_delay_ms( PB_KEYDOWN_DELAY );



}

void pb_wait_keyup( uint8 scancode )
{


	 while(PDATG & scancode);
	 sw_delay_ms( PB_KEYDOWN_DELAY );
	 while((PDATG & scancode) == 0);
	 sw_delay_ms( PB_KEYUP_DELAY );
}

void pb_wait_any_keydown( void )
{
	uint8 res;


	while(!((PDATG & PB_LEFT) && (PDATG & PB_RIGHT) ));

}

void pb_wait_any_keyup( void )
{


		while( (PDATG & PB_LEFT) && (PDATG & PB_RIGHT) );
	    sw_delay_ms( PB_KEYDOWN_DELAY );


		while( !((PDATG & PB_LEFT) && (PDATG & PB_RIGHT)) );
	    sw_delay_ms( PB_KEYUP_DELAY );


}

uint8 pb_getchar( void )
{
	uint8 res;

	while( (PDATG & PB_LEFT) && (PDATG & PB_RIGHT) );
    sw_delay_ms( PB_KEYDOWN_DELAY );

    res = pb_scan();

	while( !((PDATG & PB_LEFT) && (PDATG & PB_RIGHT)) );
    sw_delay_ms( PB_KEYUP_DELAY );

    return res;
}

uint8 pb_getchartime( uint16 *ms )
{
    uint8 scancode;
    
    while( (PDATG & PB_LEFT) && (PDATG & PB_RIGHT) );
    timer3_start();
    sw_delay_ms( PB_KEYDOWN_DELAY );
    
    scancode = pb_scan();
    
    while( !((PDATG & PB_LEFT) && (PDATG & PB_RIGHT)) );

    *ms = timer3_stop() / 10;
    sw_delay_ms( PB_KEYUP_DELAY );

    return scancode;
}

uint8 pb_timeout_getchar( uint16 ms )
{
	
	uint8 scancode;

	timer3_start_timeout (ms);
	if(!timer3_timeout ()){ //duda

		return PB_TIMEOUT;

	}
	
	while(PDATG & ~(3 << 6));
	sw_delay_ms (PB_KEYDOWN_DELAY);

	scancode = pb_scan();

	while( PDATG & (1 << 6) || PDATG & (1 << 7) );
	sw_delay_ms( PB_KEYUP_DELAY );	

}

void pbs_open( void (*isr)(void) )
{
    pISR_PB   = (uint32) isr;
    EXTINTPND = 0xF;
    I_ISPC    = BIT_EINT4567;
    INTMSK     &= ~(BIT_GLOBAL | BIT_EINT4567);
}

void pbs_close( void )
{
    INTMSK  |= BIT_EINT4567;
    pISR_PB  = (uint32) isr_PB_dummy;
}
