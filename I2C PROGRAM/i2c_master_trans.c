#include "LPC17xx.h"
#include "stdbool.h"
 

 

#define I2EN (1<<6) //Enable/Disable bit
#define STA  (1<<5) //Start Set/Clear bit
#define STO  (1<<4) //Stop bit
#define SI   (1<<3) //Serial Interrupt Flag Clear bit
#define AA   (1<<2) //Assert Acknowledge Set/Clear bit
#define SCL_PIN (LPC_PINCON->PINSEL0 |= ((0x3<<0))
#define SDA_PIN (LPC_PINCON->PINSEL0 |= ((0x3<<2))
#define LOW  0
#define HIGH 1
/**********************************************************/
#define SSD1306_ADDRESS               0x3C


#define SSD1306_COMMAND               0x00
#define SSD1306_DATA                  0xC0
#define SSD1306_DATA_CONTINUE       	0x40
 

#define SSD1306_SET_CONTRAST_CONTROL     0x81
#define SSD1306_DISPLAY_ALL_ON_RESUME    0xA4
#define SSD1306_DISPLAY_ALL_ON           0xA5
#define SSD1306_NORMAL_DISPLAY           0xA6
#define SSD1306_INVERT_DISPLAY           0xA7
#define SSD1306_DISPLAY_OFF              0xAE
#define SSD1306_DISPLAY_ON               0xAF
#define SSD1306_NOP                      0xE3
 

#define SSD1306_HORIZONTAL_SCROLL_RIGHT           0x26
#define SSD1306_HORIZONTAL_SCROLL_LEFT            0x27
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_RIGHT              0x29
#define SSD1306_HORIZONTAL_SCROLL_VERTICAL_AND_LEFT               0x2A
#define SSD1306_DEACTIVATE_SCROLL                                 0x2E
#define SSD1306_ACTIVATE_SCROLL                                   0x2F
#define SSD1306_SET_VERTICAL_SCROLL_AREA                          0xA3
 

#define SSD1306_SET_LOWER_COLUMN                                  0x00
#define SSD1306_SET_HIGHER_COLUMN                                 0x10
#define SSD1306_MEMORY_ADDR_MODE                                  0x20
#define SSD1306_SET_COLUMN_ADDR                                   0x21
#define SSD1306_SET_PAGE_ADDR                                     0x22
 

#define SSD1306_SET_START_LINE                                    0x40
#define SSD1306_SET_SEGMENT_REMAP                                 0xA0
#define SSD1306_SET_MULTIPLEX_RATIO                               0xA8
#define SSD1306_COM_SCAN_DIR_INC                                  0xC0
#define SSD1306_COM_SCAN_DIR_DEC                                  0xC8
#define SSD1306_SET_DISPLAY_OFFSET                                0xD3
#define SSD1306_SET_COM_PINS                                      0xDA
#define SSD1306_CHARGE_PUMP                                       0x8D
 
#define SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO                       0xD5
#define SSD1306_SET_PRECHARGE_PERIOD                              0xD9
#define SSD1306_SET_VCOM_DESELECT                                 0xDB
 
#define C_OledLastLine_U8  0x07u

void codes(uint8_t status);
void delay(void);
void OLED_DisplayString(uint8_t *ptr);
void OLED_Clear();
void I2C0Init(void)
{
                LPC_SC->PCONP |= (1 << 19);
                LPC_PINCON->PINSEL0 |= ((0x3<<0)|(0x3<<2)); //Select SCL1(P0.1) and SDA1(P0.0)
                LPC_I2C1->I2SCLL = 0X5A;
                LPC_I2C1->I2SCLH = 0x5A; //I2C0 @ 100Khz, given PCLK @ 60Mhz
                LPC_I2C1->I2CONCLR = STA | STO | SI | AA; //Clear these bits
                LPC_I2C1->I2CONSET = I2EN; //Enable I2C0
                //After this we are ready to communicate with any other device connected to the same bus.
}
 

void I2C0WaitForSI(void) //Wait till I2C0 block sets SI
{
                uint8_t status = 0;
                while ( !(LPC_I2C1->I2CONSET & SI) ); //Wait till SI bit is set. This is important!
                status=LPC_I2C1->I2STAT;
                codes(status);
}
 

void codes(uint8_t status)
{
    switch(status)
    {
        case 0x08 : LPC_I2C1->I2CONSET &= ~(SI | STO );
                    break;
 

        case 0x10 : LPC_I2C1->I2CONSET  &= ~(SI | STO);
                    break;
 

        case 0x18 : LPC_I2C1->I2CONSET  &=  ~(SI | STO) ;
                    break;
 

        case 0x20 : LPC_I2C1->I2CONSET  &=  ~(SI | STO) ;
                    break;
                                               
        case 0x28 : LPC_I2C1->I2CONSET  &=  ~(SI | STO) ;
                    break;
 

        case 0x30 : LPC_I2C1->I2CONSET  &= ~(SI |STO) ;
                    break;
 

        case 0x38 : LPC_I2C1->I2CONSET  &=  ~(SI | STO ) ;
                    break;
        default : ;
    }
 

 

}
 

void oledSendStart(void)
{
                LPC_I2C1->I2CONCLR = STA | STO | SI | AA; //Clear everything
                LPC_I2C1->I2CONSET = STA; //Set start bit to send a start condition
                I2C0WaitForSI(); //Wait till the SI bit is set
}
 

void I2C0SendStop(void)
{
                int timeout = 0;
                LPC_I2C1->I2CONSET = STO ; //Set stop bit to send a stop condition
                LPC_I2C1->I2CONCLR = SI;
                while (LPC_I2C1->I2CONSET & STO) //Wait till STOP is send. This is important!
                {
                                timeout++;
                                if (timeout > 10) //In case we have some error on bus
                                {
                                //            printf("STOP timeout!\n");
                                                return;
                                }
                }
}
 

void I2C1TX_Byte(uint8_t data)
{
                LPC_I2C1->I2DAT = data;
                LPC_I2C1->I2CONCLR = STA | STO | SI; //Clear These to TX data
                I2C0WaitForSI(); //wait till TX is finished
}
 

 

void OLED_Init(void)
{
    I2C1TX_Byte(SSD1306_DISPLAY_OFF);
    I2C1TX_Byte(SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO);
    I2C1TX_Byte(0x80);
    I2C1TX_Byte(SSD1306_SET_MULTIPLEX_RATIO);
    I2C1TX_Byte(0x3F);
    I2C1TX_Byte(SSD1306_SET_DISPLAY_OFFSET);
    I2C1TX_Byte(0x0);
    I2C1TX_Byte(SSD1306_SET_START_LINE | 0x0);
    I2C1TX_Byte(SSD1306_CHARGE_PUMP);
    I2C1TX_Byte(0x14);
    I2C1TX_Byte(SSD1306_MEMORY_ADDR_MODE);
    I2C1TX_Byte(0x00);
    I2C1TX_Byte(SSD1306_SET_SEGMENT_REMAP | 0x1);
    I2C1TX_Byte(SSD1306_COM_SCAN_DIR_DEC);
    I2C1TX_Byte(SSD1306_SET_COM_PINS);
    I2C1TX_Byte(0x12);
    I2C1TX_Byte(SSD1306_SET_CONTRAST_CONTROL);
    I2C1TX_Byte(0xCF);
    I2C1TX_Byte(SSD1306_SET_PRECHARGE_PERIOD);
    I2C1TX_Byte(0xF1);
    I2C1TX_Byte(SSD1306_SET_VCOM_DESELECT);
    I2C1TX_Byte(0x40);
    I2C1TX_Byte(SSD1306_DISPLAY_ALL_ON_RESUME);
    I2C1TX_Byte(SSD1306_NORMAL_DISPLAY);
    I2C1TX_Byte(SSD1306_DISPLAY_ON);
 

 // OLED_Clear();  /* Clear the complete LCD during init */
}
 

void OLED_Clear()
{
    int i;
 

		I2C1TX_Byte(SSD1306_SET_COLUMN_ADDR);
    I2C1TX_Byte(0);
    I2C1TX_Byte(127);
 

    I2C1TX_Byte(SSD1306_SET_PAGE_ADDR);
    I2C1TX_Byte(0);
    I2C1TX_Byte(7);
		//oledSendStart();
    //I2C1TX_Byte(SSD1306_ADDRESS<<1);
    //I2C1TX_Byte(SSD1306_DATA_CONTINUE);
 

    for (i=0; i<1024; i++)      // Write Zeros to clear the display
    {
        I2C1TX_Byte(0);
    }
 

    I2C1TX_Byte(SSD1306_SET_COLUMN_ADDR);
    I2C1TX_Byte(0);
    I2C1TX_Byte(127);
 

    I2C1TX_Byte(SSD1306_SET_PAGE_ADDR);
    I2C1TX_Byte(0);
    I2C1TX_Byte(7);            
 

    oledSendStart();
    I2C1TX_Byte(SSD1306_ADDRESS<<1);
    I2C1TX_Byte(SSD1306_DATA_CONTINUE);

	}

uint8_t  OledLineNum,OledCursorPos;
 

void OLED_SetCursor(uint8_t lineNumber,uint8_t cursorPosition)
{
    /* Move the Cursor to specified position only if it is in range */
    if((lineNumber <= C_OledLastLine_U8) && (cursorPosition <= 127))
    {
        OledLineNum=lineNumber;   /* Save the specified line number */
        OledCursorPos=cursorPosition; /* Save the specified cursor position */
 

        I2C1TX_Byte(SSD1306_SET_COLUMN_ADDR);
        I2C1TX_Byte(cursorPosition);
        I2C1TX_Byte(127);
 

        I2C1TX_Byte(SSD1306_SET_PAGE_ADDR);
        I2C1TX_Byte(lineNumber);
        I2C1TX_Byte(7);
 

				oledSendStart();
        I2C1TX_Byte(SSD1306_ADDRESS<<1);
				I2C1TX_Byte(SSD1306_DATA_CONTINUE);
    }
}
void delay(void)
{
	uint16_t i,j;
	for(i=0;i<1000;i++)
	{
		for(j=0;j<1000 ;j++);
	}
}
int main()
{
                I2C0Init();
                oledSendStart();
                I2C1TX_Byte((SSD1306_ADDRESS<<1) | 0x00);
                OLED_Init();
                OLED_SetCursor(0,0);
	
	
																//I2C1TX_Byte(0X00);
                                //delay();
																//I2C1TX_Byte(0X80);
																//delay();
																OLED_DisplayString("SHIVAPRASAD");
		//delay();
		while(1);		
              
}
 
#define FONT_SIZE 5
uint8_t const OledFontTable[][FONT_SIZE]=
{
        0x00, 0x00, 0x00, 0x00, 0x00,   // space
        0x00, 0x00, 0x2f, 0x00, 0x00,   // !
        0x00, 0x07, 0x00, 0x07, 0x00,   // "
        0x14, 0x7f, 0x14, 0x7f, 0x14,   // #
        0x24, 0x2a, 0x7f, 0x2a, 0x12,   // $
        0x23, 0x13, 0x08, 0x64, 0x62,   // %
        0x36, 0x49, 0x55, 0x22, 0x50,   // &
        0x00, 0x05, 0x03, 0x00, 0x00,   // '
        0x00, 0x1c, 0x22, 0x41, 0x00,   // (
        0x00, 0x41, 0x22, 0x1c, 0x00,   // )
        0x14, 0x08, 0x3E, 0x08, 0x14,   // *
        0x08, 0x08, 0x3E, 0x08, 0x08,   // +
        0x00, 0x00, 0xA0, 0x60, 0x00,   // ,
        0x08, 0x08, 0x08, 0x08, 0x08,   // -
        0x00, 0x60, 0x60, 0x00, 0x00,   // .
        0x20, 0x10, 0x08, 0x04, 0x02,   // /
 

        0x3E, 0x51, 0x49, 0x45, 0x3E,   // 0
        0x00, 0x42, 0x7F, 0x40, 0x00,   // 1
        0x42, 0x61, 0x51, 0x49, 0x46,   // 2
        0x21, 0x41, 0x45, 0x4B, 0x31,   // 3
        0x18, 0x14, 0x12, 0x7F, 0x10,   // 4
        0x27, 0x45, 0x45, 0x45, 0x39,   // 5
        0x3C, 0x4A, 0x49, 0x49, 0x30,   // 6
        0x01, 0x71, 0x09, 0x05, 0x03,   // 7
        0x36, 0x49, 0x49, 0x49, 0x36,   // 8
        0x06, 0x49, 0x49, 0x29, 0x1E,   // 9
 

        0x00, 0x36, 0x36, 0x00, 0x00,   // :
        0x00, 0x56, 0x36, 0x00, 0x00,   // ;
        0x08, 0x14, 0x22, 0x41, 0x00,   // <
        0x14, 0x14, 0x14, 0x14, 0x14,   // =
        0x00, 0x41, 0x22, 0x14, 0x08,   // >
        0x02, 0x01, 0x51, 0x09, 0x06,   // ?
        0x32, 0x49, 0x59, 0x51, 0x3E,   // @
 

        0x7C, 0x12, 0x11, 0x12, 0x7C,   // A
        0x7F, 0x49, 0x49, 0x49, 0x36,   // B
        0x3E, 0x41, 0x41, 0x41, 0x22,   // C
        0x7F, 0x41, 0x41, 0x22, 0x1C,   // D
        0x7F, 0x49, 0x49, 0x49, 0x41,   // E
        0x7F, 0x09, 0x09, 0x09, 0x01,   // F
        0x3E, 0x41, 0x49, 0x49, 0x7A,   // G
        0x7F, 0x08, 0x08, 0x08, 0x7F,   // H
        0x00, 0x41, 0x7F, 0x41, 0x00,   // I
        0x20, 0x40, 0x41, 0x3F, 0x01,   // J
        0x7F, 0x08, 0x14, 0x22, 0x41,   // K
        0x7F, 0x40, 0x40, 0x40, 0x40,   // L
        0x7F, 0x02, 0x0C, 0x02, 0x7F,   // M
        0x7F, 0x04, 0x08, 0x10, 0x7F,   // N
        0x3E, 0x41, 0x41, 0x41, 0x3E,   // O
        0x7F, 0x09, 0x09, 0x09, 0x06,   // P
        0x3E, 0x41, 0x51, 0x21, 0x5E,   // Q
        0x7F, 0x09, 0x19, 0x29, 0x46,   // R
        0x46, 0x49, 0x49, 0x49, 0x31,   // S
        0x01, 0x01, 0x7F, 0x01, 0x01,   // T
        0x3F, 0x40, 0x40, 0x40, 0x3F,   // U
        0x1F, 0x20, 0x40, 0x20, 0x1F,   // V
        0x3F, 0x40, 0x38, 0x40, 0x3F,   // W
        0x63, 0x14, 0x08, 0x14, 0x63,   // X
        0x07, 0x08, 0x70, 0x08, 0x07,   // Y
        0x61, 0x51, 0x49, 0x45, 0x43,   // Z
 

        0x00, 0x7F, 0x41, 0x41, 0x00,   // [
        0x55, 0xAA, 0x55, 0xAA, 0x55,   // Backslash (Checker pattern)
        0x00, 0x41, 0x41, 0x7F, 0x00,   // ]
        0x04, 0x02, 0x01, 0x02, 0x04,   // ^
        0x40, 0x40, 0x40, 0x40, 0x40,   // _
        0x00, 0x03, 0x05, 0x00, 0x00,   // `
 

        0x20, 0x54, 0x54, 0x54, 0x78,   // a
        0x7F, 0x48, 0x44, 0x44, 0x38,   // b
        0x38, 0x44, 0x44, 0x44, 0x20,   // c
        0x38, 0x44, 0x44, 0x48, 0x7F,   // d
        0x38, 0x54, 0x54, 0x54, 0x18,   // e
        0x08, 0x7E, 0x09, 0x01, 0x02,   // f
        0x18, 0xA4, 0xA4, 0xA4, 0x7C,   // g
        0x7F, 0x08, 0x04, 0x04, 0x78,   // h
        0x00, 0x44, 0x7D, 0x40, 0x00,   // i
        0x40, 0x80, 0x84, 0x7D, 0x00,   // j
        0x7F, 0x10, 0x28, 0x44, 0x00,   // k
        0x00, 0x41, 0x7F, 0x40, 0x00,   // l
        0x7C, 0x04, 0x18, 0x04, 0x78,   // m
        0x7C, 0x08, 0x04, 0x04, 0x78,   // n
        0x38, 0x44, 0x44, 0x44, 0x38,   // o
        0xFC, 0x24, 0x24, 0x24, 0x18,   // p
        0x18, 0x24, 0x24, 0x18, 0xFC,   // q
        0x7C, 0x08, 0x04, 0x04, 0x08,   // r
        0x48, 0x54, 0x54, 0x54, 0x20,   // s
        0x04, 0x3F, 0x44, 0x40, 0x20,   // t
        0x3C, 0x40, 0x40, 0x20, 0x7C,   // u
        0x1C, 0x20, 0x40, 0x20, 0x1C,   // v
        0x3C, 0x40, 0x30, 0x40, 0x3C,   // w
        0x44, 0x28, 0x10, 0x28, 0x44,   // x
        0x1C, 0xA0, 0xA0, 0xA0, 0x7C,   // y
        0x44, 0x64, 0x54, 0x4C, 0x44,   // z
 

        0x00, 0x10, 0x7C, 0x82, 0x00,   // {
        0x00, 0x00, 0xFF, 0x00, 0x00,   // |
        0x00, 0x82, 0x7C, 0x10, 0x00,   // }
        0x00, 0x06, 0x09, 0x09, 0x06    // ~ (Degrees)
};
 

void OLED_DisplayChar(uint8_t ch)
{
    uint8_t dat,i=0;
 

    if(((OledCursorPos+FONT_SIZE)>=128) || (ch=='\n'))
    {
        /* If the cursor has reached to end of line on page1
        OR NewLine command is issued Then Move the cursor to next line */
       // OLED_GoToNextLine();
    }
    if(ch!='\n') /* TODO */
    {
        ch = ch-0x20; // As the lookup table starts from Space(0x20)
 

        while(1)
        {
            dat= OledFontTable[ch][i]; /* Get the data to be displayed for LookUptable*/
						I2C1TX_Byte(dat); /* Display the data and keep track of cursor */
            OledCursorPos++;
						i++;
 

            if(i==FONT_SIZE) /* Exit the loop if End of char is encountered */
            {
                I2C1TX_Byte(0x00); /* Display the data and keep track of cursor */
                OledCursorPos++;
                break;
            }
        }
    }
}
void OLED_DisplayString(uint8_t *ptr)
{
    while(*ptr)
        OLED_DisplayChar(*ptr++);
}
 