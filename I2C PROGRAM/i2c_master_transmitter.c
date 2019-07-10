/*#include "lpc17xx.h"
#include "i2c.h"
void i2c_init(void);
void send_start(void);
void send_stop(void);
uint8_t byte_write(uint8_t,uint8_t);
uint8_t i,slave_addr;
void set_enable_interface(void);

int main(void)
{
	i2c_init();
	
	uint8_t commands[]={0x00,0x40,0xAE,0XAF,0X20,0X21,0X81,0XA6,0XA7};
	for(i=0;i<10;i++)
		{
			byte_write(slave_addr,commands[i]);
		}
}

void i2c_init(void)
{
	LPC_SC->PCONP|= (1 << 19); 												// POWER ON I2C1
	LPC_PINCON->PINSEL1 &= ~(3 << 6); 								// PCLK
	LPC_PINCON->PINSEL0 |= (3 << 0); 								//P0.0 SDA1 
	LPC_PINCON->PINSEL0 |= (3 << 2);						 		// SCL1 P0.1
	LPC_PINCON->PINMODE_OD0 |= (1 << 0);							// P0.0 SDA1 OPEN DRAIN MODE
	LPC_PINCON->PINMODE_OD0 |= (1 << 1);							// SCL1 P0.1 OPEN DRAIN MODE
	LPC_PINCON->PINMODE0 |= (2 << 0); 								//P0.0 SDA1 NEITHER PULL UP NOR PULL DOWN
	LPC_PINCON->PINMODE0 |= (2 << 2);									//SCL1 P0.1 NEITHER PULL UP NOR PULL DOWN
	LPC_I2C1->I2CONCLR = 0XFF;
	LPC_I2C1->I2CONSET |= (1 << 6); 									// ENABLING I2C INTERFACE
	LPC_I2C1->I2SCLH= 0X5A;									 					//18*10^6/180 = 10^5 = 100Khz,CLH+CLL=180
	LPC_I2C1->I2SCLL= 0X5A;
	//jav, 	18MHz/180=100kHz
}
void set_enable_interface(void)
{
	LPC_I2C1->I2CONCLR = 0XFF; 												// clear all bits
	LPC_I2C1->I2CONSET = (1 << 6); 										// ENABLING I2C INTERFACE
}
void send_start(void)
{
	LPC_I2C1->I2CONSET = (1 << 5); 										// setting start bit
}

void send_stop(void)
{
	LPC_I2C1->I2CONSET = 0X10; 												// setting stop bit
}

uint8_t byte_write(uint8_t slave_addr,uint8_t data)
{
	uint8_t val;
	set_enable_interface();
	send_start();
	while(!(LPC_I2C1->I2CONSET & 0X08)); 						// check SI flag i.e start condition has been transmitted.
	LPC_I2C1->I2DAT = slave_addr<< 1 & 0xFE; 				// writing slave addr with direction bit (0=write)
	LPC_I2C1->I2CONCLR = (1 << 3) | (1 << 5); 									// CLEAR SI BIT
	while(!(LPC_I2C1->I2CONSET & 0X08)); 						// WAIT UNTIL SI TO SET
	val=LPC_I2C1->I2STAT;
	switch(val)
	{
		case 0x18:
		{
			LPC_I2C1->I2CONCLR = (1<<5) | (1 << 3);
			break;
		}
		case 0x20:
		{
			LPC_I2C1->I2CONSET = (1<<5);
			break;
		}
		case 0x28:
		{
			LPC_I2C1->I2CONCLR = (1<< 3) | (1 << 5);
			break;
		}
		case 0x38:
		{
			LPC_I2C1->I2CONCLR = (1<<5)|(1 << 3);
			break;
		}
		case 0x30:
		{
			LPC_I2C1->I2CONSET =(1<<5);
			break;
			
		}
	}
	
	LPC_I2C1->I2DAT = data;
	while(!(LPC_I2C1->I2CONSET & 0X08));
	LPC_I2C1->I2CONCLR = 0X28;												 //  clear SI bit And start bit.
	
	while(!(LPC_I2C1->I2CONSET & 0X08)); // check SI flag i.e data has been txed.
	send_stop();
	
}
*/
