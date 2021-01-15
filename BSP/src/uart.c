
#include <s3c44b0x.h>
#include <uart.h>

void uart0_init( void )
{
    UFCON0 = 0x1;
    UMCON0 = 0x0;
    ULCON0 = 0x3;
    UBRDIV0 = 0x22;
    UCON0 = 0x5;
}

void uart0_putchar( char ch )
{
   while(  UFSTAT0 &(1 << 9) );

   UTXH0 = ch;

}

char uart0_getchar( void )
{
    while( !(UFSTAT0 & 0xF) );
    return URXH0;



}

void uart0_puts( char *s )
{
	while( *s !='\0' ){
		uart0_putchar(*s++);
	}
}

void uart0_putint( int32 i )
{

	char buf[32 + 1];
	char *p = buf + 32;

	uint32 c;

	*p = '\0'; 

	do{
		c = i % 10;
		*--p = '0' + c;
		i = i/10;

	}while(i != 0);

	uart0_puts(p);

}

void uart0_puthex( uint32 i )
{
    char buf[8 + 1];
    char *p = buf + 8;
    uint8 c;

    *p = '\0';

    do {
        c = i & 0xf;
        if( c < 10 )
            *--p = '0' + c;
        else
            *--p = 'A' + c - 10;
        i = i >> 4;
    } while( i );

    uart0_puts( p );
}

void uart0_gets( char *s )
{

	char c;
	do{
		c = uart0_getchar();
		if( c != '\n'){
			*s = c;
			s++;
		}else{
			*s = '\0';
		}
	}while(c != '\n');







}

int32 uart0_getint( void )
{

	char  s[20];
	uart0_gets( s );


	char *p = s;

	int32 k = 0;


	int i = 0;

	int neg = 0;

		if(*p == '-'){
		neg = 1;
		p++;
	}

	
	while( *p != '\0'){

		i*= 10;

		
		k = *p - '0';

		i += k;
		
		p++;
	}
if( neg == 1){
	  i = -i;
	}

	return i;
	}




uint32 uart0_gethex( void )
{

	char c[256];

	uart0_gets(c);
	char *p = c;

	uint32 k = 0;


	uint32 i = 0;
	while( *p != '\0'){
			 i *= 16;
			if(  ('0' <= *p)  && (*p <= '9')){
					k = *p - '0';
			}else if ( ('A' <= *p ) && ( *p <= 'F')){
				k = *p + 10 - 'A';
			}else{
				k = *p + 10 - 'a';
			}
			i += k;
			p++;
}

	return i ;

}

