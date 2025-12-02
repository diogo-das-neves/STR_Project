/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.6
        Device            :  PIC16F18875
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"
#include "I2C/i2c.h"
#include "extra.h"
#include "LCD/lcd.h"
#include "stdio.h"

#define MINLUM  0x00
#define MAXLUM  0x05
#define MINTEMP 0x0A
#define MAXTEMP 0x14

unsigned char readTC74 (void)
{
	unsigned char value;
do{
	IdleI2C();
	StartI2C(); IdleI2C();
    
	WriteI2C(0x9a | 0x00); IdleI2C();
	WriteI2C(0x01); IdleI2C();
	RestartI2C(); IdleI2C();
	WriteI2C(0x9a | 0x01); IdleI2C();
	value = ReadI2C(); IdleI2C();
	NotAckI2C(); IdleI2C();
	StopI2C();
} while (!(value & 0x40));

	IdleI2C();
	StartI2C(); IdleI2C();
	WriteI2C(0x9a | 0x00); IdleI2C();
	WriteI2C(0x00); IdleI2C();
	RestartI2C(); IdleI2C();
	WriteI2C(0x9a | 0x01); IdleI2C();
	value = ReadI2C(); IdleI2C();
	NotAckI2C(); IdleI2C();
	StopI2C();

	return value;
}


uint8_t seconds = 0;
uint8_t minutes = 0;
uint8_t hours   = 0;
void t1_isr()
{
    seconds++;
    if(seconds >= 60){
        seconds = 0;
        minutes++;
    }
    if(minutes >= 60){
        minutes = 0;
        hours++;
    }
    
    if(seconds == 0 || seconds % 5 == 0){
        flag_read_sensors = 1;
    }
}
/*
                         Main application
 */
volatile uint8_t flag_read_sensors = 1;

void main(void)
{
    unsigned char c;
    char buf[17];
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    State state = normal;
    
    
    
    // LCD and I2C start
    OpenI2C();
    LCDinit();

    TMR1_SetInterruptHandler(t1_isr);
    D2_SetHigh();
    
    ADCC_Initialize();
    ADCC_DisableContinuousConversion();
    
    
    int minLum = 1023;
    int maxLum = 0;
    int minTemp = 255;
    int maxTemp = 0;

    uint8_t newLum;
    adc_result_t lum;
    uint8_t temp;

    while (1)
    {
        if(flag_read_sensors == 1){
            flag_read_sensors = 0;
            lum = ADCC_GetSingleConversion(0x0);
            temp = readTC74();

        

            newLum = (lum * 255) / 1023;

            if(lum < minLum){
                minLum = lum;
                DATAEE_WriteByte(MINLUM,   hours);
                DATAEE_WriteByte(MINLUM+1, minutes);
                DATAEE_WriteByte(MINLUM+2, seconds);
                DATAEE_WriteByte(MINLUM+3, newLum);
                DATAEE_WriteByte(MINLUM+4, temp);
            }
            if(lum > maxLum){
                maxLum = lum;
                DATAEE_WriteByte(MAXLUM,   hours);
                DATAEE_WriteByte(MAXLUM+1, minutes);
                DATAEE_WriteByte(MAXLUM+2, seconds);
                DATAEE_WriteByte(MAXLUM+3, newLum);
                DATAEE_WriteByte(MAXLUM+4, temp);
            }

            if(temp < minTemp){
                minTemp = temp;
                DATAEE_WriteByte(MINTEMP,   hours);
                DATAEE_WriteByte(MINTEMP+1, minutes);
                DATAEE_WriteByte(MINTEMP+2, seconds);
                DATAEE_WriteByte(MINTEMP+3, temp);
            }
            if(temp > maxTemp){
                maxTemp = temp;
                DATAEE_WriteByte(MAXTEMP,   hours);
                DATAEE_WriteByte(MAXTEMP+1, minutes);
                DATAEE_WriteByte(MAXTEMP+2, seconds);
                DATAEE_WriteByte(MAXTEMP+3, temp);
            }
        }
    }
        // Add your application code
        NOP();

        // 1. Input handling / non periodic tasks
        
        // 2. Get updated values and sutff / periodic tasks
        
        // 3. Screen info, updated after each cycle
        switch(state){
            case normal:
                // print normal display and values?
                break;
            case config:
                //print the config stuff
                break;
            case records:
                // access the register that store the values?
                break;
        }
        
        c = readTC74();
        LCDcmd(0x80);       //first line, first column
        while (LCDbusy());
        LCDstr("Temp");
        LCDpos(0,8);
        while (LCDbusy());
        LCDstr("STR-RTS");
        LCDcmd(0xc0);       // second line, first column
        sprintf(buf, "%02d C", c);
        while (LCDbusy());
        LCDstr(buf);
        __delay_ms(2000);
    }
}
/**
 End of File
*/

