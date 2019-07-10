/*#include "lpc17xx.h"
#include "i2c.h"
void i2c_master_receiver_init(void);
void send_start(void);
void send_stop(void);
void restart(void);
void byte_recieve(uint8_t slave_addr);


/******************************************************
main function
*******************************************************/
int main(void)
{
	i2c_master_receiver_init();
	while(1)
	{
		byte_recieve(0x5D);
	}
}


/******************************************************
Function send_start()
no parameters
sets start bit i2conset register
******************************************************/
void send_start(void)
{
	LPC_I2C1->I2CONSET = (1 << 5);                    //set start bit
}


/******************************************************
Function send_stop()
No parameters
sets stop bit in i2conset register
******************************************************/
void send_stop(void)
{
	LPC_I2C1->I2CONSET = (1 << 4);                    // set stop bit
}


/******************************************************
Function restart()
No parameters
sets start bit to restart in i2conset register
******************************************************/
void restart(void)
{
	LPC_I2C1->I2CONSET = (1 << 5);                    // send restart bit
}


/******************************************************
Function i2c_master_receiver_init()
No parameters
Basic configuration of i2c communication
******************************************************/
void i2c_master_receiver_init(void)
{
	LPC_SC->PCONP|= (1 << 19); 												// POWER ON I2C1
	LPC_PINCON->PINSEL1 &= ~(3 << 6); 								// PCLK
	LPC_PINCON->PINSEL0 |= (3 << 0); 									//P0.0 SDA1 
	LPC_PINCON->PINSEL0 |= (3 << 2);						 			// SCL1 P0.1
	LPC_PINCON->PINMODE_OD0 |= (1 << 0);							// P0.0 SDA1 OPEN DRAIN MODE
	LPC_PINCON->PINMODE_OD0 |= (1 << 1);							// SCL1 P0.1 OPEN DRAIN MODE
	LPC_PINCON->PINMODE0 |= (2 << 0); 								//P0.0 SDA1 NEITHER PULL UP NOR PULL DOWN
	LPC_PINCON->PINMODE0 |= (2 << 2);									//SCL1 P0.1 NEITHER PULL UP NOR PULL DOWN
	LPC_I2C1->I2CONCLR = 0XFF;												// clear all bits
	LPC_I2C1->I2CONSET |= (1 << 6); 									// ENABLING I2C INTERFACE
	LPC_I2C1->I2SCLH= 0X5A;									 					//18*10^6/180 = 10^5 = 100Khz,CLH+CLL=180
	LPC_I2C1->I2SCLL= 0X5A;
}


/*************************************************************
Function byte_recieve()
Takes 1 parameter as slave address
*************************************************************/
void byte_recieve(uint8_t slave_addr)
{
	uint8_t code,val;
	send_start();
	while(!(LPC_I2C1->I2CONSET & 0X08));              // wait until SI to set
	LPC_I2C1->I2DAT = slave_addr;                     // send address of slave and direction bit
	LPC_I2C1->I2CONCLR = (1 << 3);                    // clear SI bit 
	while(!(LPC_I2C1->I2CONSET & 0X08));              // wait until SI to set
	code =  LPC_I2C1->I2STAT;                         // get status code
	switch(code)
	{
			case 0x40:
			{
				LPC_I2C1->I2CONSET = ( 1<< 2);              //Data byte will be received; ACK bit will be returned.
				break;
			}
			case 0x48:
			{
				restart();
				break;
			}
			case 0x38:
			{
				restart();
				break;
			}
	}
	val=LPC_I2C1->I2DATA_BUFFER;
	LPC_I2C1->I2CONCLR = (1 << 3);                     // clear SI bit 
	while(!(LPC_I2C1->I2CONSET & 0X08));               // wait until SI to set
	restart();
}
*/
