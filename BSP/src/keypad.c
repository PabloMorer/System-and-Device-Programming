
#include <s3c44b0x.h>
#include <s3cev40.h>
#include <timers.h>
#include <keypad.h>

extern void isr_KEYPAD_dummy( void );

void keypad_init( void )
{
//    EXTINT = (EXTINT & ~(0xf<<4)) | (2<<4);
//	keypad_open( isr_KEYPAD_dummy );
};

uint8 keypad_scan( void )
{
    uint8 aux;

    aux = *( KEYPAD_ADDR + 0x1c );
    if( (aux & 0x0f) != 0x0f ) //estamos en la fila 1
    {
        if( (aux & 0x8) == 0 ) //columna  1
            return KEYPAD_KEY0;
        else if( (aux & 0x4) == 0 ) //columna 2
            return KEYPAD_KEY1;
        else if( (aux & 0x2) == 0 ) // columna 3
            return KEYPAD_KEY2;
        else if( (aux & 0x1) == 0 ) // columna 4
            return KEYPAD_KEY3;
    }

    aux = *( KEYPAD_ADDR + 0x20);
	   if( (aux & 0x0f) != 0x0f ) //estamos en la fila 2
    {
        if( (aux & 0x8) == 0 ) //columna  1
            return KEYPAD_KEY4;
        else if( (aux & 0x4) == 0 ) //columna 2
            return KEYPAD_KEY5;
        else if( (aux & 0x2) == 0 ) // columna 3
            return KEYPAD_KEY6;
        else if( (aux & 0x1) == 0 ) // columna 4
            return KEYPAD_KEY7;
    }

	aux = *(KEYPAD_ADDR + 0x24);

	   if( (aux & 0x0f) != 0x0f ) //estamos en la fila 3
    {
        if( (aux & 0x8) == 0 ) //columna  1
            return KEYPAD_KEY8;
        else if( (aux & 0x4) == 0 ) //columna 2
            return KEYPAD_KEY9;
        else if( (aux & 0x2) == 0 ) // columna 3
            return KEYPAD_KEYA;
        else if( (aux & 0x1) == 0 ) // columna 4
            return KEYPAD_KEYB;
    }

	aux = *(KEYPAD_ADDR + 0x28);

	   if( (aux & 0x0f) != 0x0f ) //estamos en la fila 4
    {
        if( (aux & 0x8) == 0 ) //columna  1
            return KEYPAD_KEYC;
        else if( (aux & 0x4) == 0 ) //columna 2
            return KEYPAD_KEYD;
        else if( (aux & 0x2) == 0 ) // columna 3
            return KEYPAD_KEYE;
        else if( (aux & 0x1) == 0 ) // columna 4
            return KEYPAD_KEYF;
    }

	

    return KEYPAD_FAILURE;
}

uint8 keypad_status( uint8 scancode )
{

}

void keypad_wait_keydown( uint8 scancode )
{
     while( 1 ) 
     {
        while( PDATG & (1 << 1));
        sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
        if ( scancode == keypad_scan() )
            return;
        while( (PDATG & (1 << 1)) == 0);
        sw_delay_ms( KEYPAD_KEYUP_DELAY );
    }         
}



void keypad_wait_keyup( uint8 scancode )
{
	int  t;
	while(1){
		t = 0;
		while(PDATG & (1 << 1));
		sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
		if ( scancode == keypad_scan() ){
					t = 1;
				}
		while(( PDATG & (1 << 1)) == 0);
		sw_delay_ms( KEYPAD_KEYUP_DELAY );

		if(t){
			return;
		}


	}

}

void keypad_wait_any_keydown( void )
{
	while(PDATG & (1 << 1));
	sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
}

void keypad_wait_any_keyup( void )
{

	while(PDATG & (1 << 1));
	sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
	while( (PDATG & (1 << 1)) == 0);
	sw_delay_ms( KEYPAD_KEYUP_DELAY );
 	
}

uint8 keypad_getchar( void )
{
	while(PDATG & (1 << 1));
	sw_delay_ms( KEYPAD_KEYDOWN_DELAY );
	uint8 res = keypad_scan();
	while( PDATG & (1 << 1) == 0);
	sw_delay_ms( KEYPAD_KEYUP_DELAY );

}

uint8 keypad_getchartime( uint16 *ms )
{
	uint8 scancode;
	while(PDATG & (1 << 1));
	timer3_start();
	sw_delay_ms(KEYPAD_KEYDOWN_DELAY);

	scancode = keypad_scan();

	while(( PDATG & (1 << 1)) == 0);

	*ms = timer3_stop() / 10;
    sw_delay_ms( KEYPAD_KEYUP_DELAY );


	return scancode;
}

uint8 keypad_timeout_getchar( uint16 ms )
{
	timer3_start_timeout (ms);

	if(timer3_timeout ()){

		return KEYPAD_TIMEOUT;
		
	}


	uint8 scancode;
	while(PDATG & ~(1 << 1));

	 timer3_start();
	 sw_delay_ms(KEYPAD_KEYDOWN_DELAY);

	 scancode = keypad_scan();

	while(PDATG & (1 << 1));

	timer3_stop ();

	sw_delay_ms( KEYPAD_KEYUP_DELAY );

	
	return scancode;
	  

}

void keypad_open( void (*isr)(void) )
{
	pISR_KEYPAD = (uint32) isr;
	I_ISPC    = BIT_EINT1 ;
    INTMSK     &= ~(BIT_GLOBAL | BIT_EINT1 );

}

void keypad_close( void )
{
	pISR_KEYPAD = (uint32) isr_KEYPAD_dummy;
	INTMSK  |=  BIT_EINT1;

}

