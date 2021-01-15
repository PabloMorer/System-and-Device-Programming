
#include <s3c44b0x.h>
#include <lcd.h>

extern uint8 font[];
static uint8 lcd_buffer[LCD_BUFFER_SIZE];

static uint8 state;



void lcd_init( void )
{      
    DITHMODE = 0x12210;
    DP1_2    = 0x5A5;      
    DP4_7    = 0xBA5DA65;
    DP3_5    = 0xA5A5F;
    DP2_3    = 0xD6B;
    DP5_7    = 0xEB7B5ED;
    DP3_4    = 0x7DBE;
    DP4_5    = 0x7EBDF;
    DP6_7    = 0x7FDFBFE;
    
    REDLUT   = 0x0;
    GREENLUT = 0x0; 
    BLUELUT  = 0x0;

    LCDCON1  = 0x1C020;
    LCDCON2  = 0x13CEF;
    LCDCON3  = 0x0;    

    LCDSADDR1 = (2 << 27) | ((uint32)lcd_buffer >> 1);
    LCDSADDR2 = (1 << 29) | (((uint32)lcd_buffer + LCD_BUFFER_SIZE) & 0x3FFFFF) >> 1;
    LCDSADDR3 = 0x50;
    
    lcd_off();
}

void lcd_on( void )
{
    LCDCON1 |= (1 << 0);
}

void lcd_off( void )
{
    LCDCON1 &= ~(1 << 0);
}

uint8 lcd_status( void )
{
    if(LCDCON1 & (1 << 1)){
		return 1;
	}else{
		return 0;
	}
}

void lcd_clear( void )
{
    uint16 i ;
    for(i = 0; i < LCD_BUFFER_SIZE; i++){
    	lcd_buffer[i] = WHITE;
    }
}

void lcd_putpixel( uint16 x, uint16 y, uint8 c)
{
    uint8 byte, bit;
    uint16 i;

    i = x/2 + y*(LCD_WIDTH/2);
    bit = (1-x%2)*4;
    
    byte = lcd_buffer[i];
    byte &= ~(0xF << bit);
    byte |= c << bit;
    lcd_buffer[i] = byte;
}

uint8 lcd_getpixel( uint16 x, uint16 y )
{
   uint16 pos = x/2 + y*(320/2); // calculamos la posicion
   uint8 bit = (1 - x % 2) * 4; //calculamos si es de 7 a 4 o de 3 a 0
   uint8 byte = lcd_buffer[pos]; //leemos el byte que contiene el buffer

	return byte &= ~(0xF << bit);
	
}

void lcd_draw_hline( uint16 xleft, uint16 xright, uint16 y, uint8 color, uint16 width )
{	uint16 xact, yact;
    for( xact = xleft ; xact <= xright ; xact++ ){
		for( yact = 0; yact <= width ; yact++){
			lcd_putpixel(xact, y + yact ,color);
		}
	}
}

void lcd_draw_vline( uint16 yup, uint16 ydown, uint16 x, uint8 color, uint16 width )
{

	uint16 xact, yact;


    for( yact = yup ; yact <= ydown ; yact++ ){
		for( xact = 0; xact <= width ; xact++){
			lcd_putpixel(x + xact, yact,color);
		}
	}


}

void lcd_draw_box( uint16 xleft, uint16 yup, uint16 xright, uint16 ydown, uint8 color, uint16 width )
{
    lcd_draw_hline(xleft,xright,yup,color,width); //linea horizontar superior
	lcd_draw_hline(xleft,xright,ydown,color,width); //linea horizontar inferior
	lcd_draw_vline(yup,ydown,xleft,color,width); //linea vertical izquierda
	lcd_draw_vline(yup,ydown+width,xright,color,width); //linea vertical derecha
}

void lcd_putchar( uint16 x, uint16 y, uint8 color, char ch )
{
    uint8 line, row;
    uint8 *bitmap;

    bitmap = font + ch*16;
    for( line=0; line<16; line++ )
        for( row=0; row<8; row++ )                    
            if( bitmap[line] & (0x80 >> row) )
                lcd_putpixel( x+row, y+line, color );
            else
                lcd_putpixel( x+row, y+line, WHITE );
}

void lcd_puts( uint16 x, uint16 y, uint8 color, char *s )
{
    while(*s != '\0'){
		lcd_putchar(x,y,color,*s);
		x+=8;


		*s++;
	}

}

void lcd_putint( uint16 x, uint16 y, uint8 color, int32 i )
{
    char buf[32 + 1];
	char *p = buf + 32;
	char j;
	*p = '\0';
	uint32 c;
	do{
		if(i >= 10){
			c = i % 10;
		}else{
			c = i;
		}


		j = '0' + c;
		*--p = j;


		i = i /10;
	
	}while(i != 0);




	lcd_puts(x, y, color, p);
	
	
}

void lcd_puthex( uint16 x, uint16 y, uint8 color, uint32 i )
{
   	char buf[8 + 1];
    char *p = buf + 8;
	
    uint8 c;
    char j;
    *p = '\0';

    do {
        c = i & 0xf;
        if( c < 10 )
            j = '0' + c;
        else
            j = 'A' + c - 10;


        *--p = j;
        i = i >> 4;
    } while( i != 0);

    lcd_puts(x, y, color, p);
}


void lcd_putchar_x2( uint16 x, uint16 y, uint8 color, char ch )
{

	uint8 line, row;
    uint8 *bitmap;

    bitmap = font + ch*16;
    for( line=0; line<32; line++ )
        for( row=0; row<16; row++ )                    
            if( bitmap[line/2] & (0x80 >> (row/2)) )
                lcd_putpixel( x+row, y+line, color );
            else
                lcd_putpixel( x+row, y+line, WHITE );

	
}

void lcd_puts_x2( uint16 x, uint16 y, uint8 color, char *s )
{
      while(*s != '\0'){
		lcd_putchar_x2(x,y,color,*s);
		x+=16;
		*s++;
	}
}

void lcd_putint_x2( uint16 x, uint16 y, uint8 color, int32 i )
{
	char buf[32 + 1];
		char *p = buf + 32;
		char j;
		*p = '\0';
		uint32 c;
		do{
			if(i >= 10){
				c = i % 10;
			}else{
				c = i;
			}


			j = '0' + c;
			*--p = j;


			i = i /10;

		}while(i != 0);




		lcd_puts_x2(x, y, color, p);
    
}

void lcd_puthex_x2( uint16 x, uint16 y, uint8 color, uint32 i )
{
   	char buf[8 + 1];
    char *p = buf + 8;

    uint8 c;
    char j;
    *p = '\0';

    do {
        c = i & 0xf;
        if( c < 10 )
            j = '0' + c;
        else
            j = 'A' + c - 10;


        *--p = j;
        i = i >> 4;
    } while( i != 0);

    lcd_puts_x2(x, y, color, p);
}

void lcd_putWallpaper( uint8 *bmp )
{
    uint32 headerSize;

    uint16 x, ySrc, yDst;
    uint16 offsetSrc, offsetDst;

    headerSize = bmp[10] + (bmp[11] << 8) + (bmp[12] << 16) + (bmp[13] << 24);

    bmp = bmp + headerSize;
    
    for( ySrc=0, yDst=LCD_HEIGHT-1; ySrc<LCD_HEIGHT; ySrc++, yDst-- )                                                                       
    {
        offsetDst = yDst*LCD_WIDTH/2;
        offsetSrc = ySrc*LCD_WIDTH/2;
        for( x=0; x<LCD_WIDTH/2; x++ )
            lcd_buffer[offsetDst+x] = ~bmp[offsetSrc+x];
    }
}


