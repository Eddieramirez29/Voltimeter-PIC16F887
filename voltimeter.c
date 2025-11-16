#include <xc.h>
#define _XTAL_FREQ 8000000                     // Set at 8 MHz
#define RESOLUTION 0.0048875855               // 4.88 mV is a constant
#define VOLTAGE_RELATIONSHIP_1 2.4

// CONFIG1
#pragma config FOSC  = INTRC_NOCLKOUT         // High speed internal oscillator
#pragma config WDTE  = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = OFF
#pragma config CP    = OFF
#pragma config CPD   = OFF
#pragma config BOREN = OFF
#pragma config IESO  = ON
#pragma config FCMEN = ON
#pragma config LVP   = OFF

// CONFIG2
#pragma config BOR4V = BOR40V
#pragma config WRT   = OFF

float v_out;
unsigned int result_conversion;

unsigned char display_digits_decimal[10] = 
{
    0x3F,
    0x06,
    0x5B,
    0x4F,
    0x66,
    0x6D,
    0x7D,
    0x07,
    0x7F,
    0x6F
};

unsigned char display_digits_units[10] = 
{
    0x3F,
    0x06,
    0x5B,
    0x4F,
    0x66,
    0x6D,
    0x7D,
    0x07,
    0x7F,
    0x6F
};




void setRegisters()
{
    OSCCON = 0b01110000;        // Set at 8 MHz
    while (OSCCONbits.HTS == 0);
    TRISAbits.TRISA0 = 1;
    ANSELbits.ANS0 = 1;
    ANSELbits.ANS5 = 0;
    ANSELH = 0x00;

    TRISC = 0x00;
    TRISD = 0x00;
    TRISEbits.TRISE0 = 0;
}

void initRegisters()
{
    //Set ADCON0
    //Frequency of conversion set at 250 kHz
    ADCON0bits.ADCS1 = 1;
    ADCON0bits.ADCS0 = 0;
    
    ADCON0bits.CHS0 = 0;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS2 = 0;
    ADCON0bits.CHS3 = 0;
    //Turn ADC on
    ADCON0bits.ADON = 1;
    //Set ADCON1
    ADCON1bits.ADFM = 1;//Right justified
    ADCON1bits.VCFG1 = 0;//0V reference
    ADCON1bits.VCFG0 = 0;//5v reference

    PORTC = 0x00;
    PORTD = 0x00;
    PORTEbits.RE0 = 1;
}



void main(void)
{
    setRegisters();
    initRegisters();
    
    while(1)
    {
        __delay_us(50);//Sample time
        ADCON0bits.GO_DONE = 1;
        while(ADCON0bits.GO_DONE);//It waits until conversion is done
        
        result_conversion = (ADRESH << 8) | ADRESL;
        
        v_out = VOLTAGE_RELATIONSHIP_1*(RESOLUTION*result_conversion);
        
        unsigned int volts_units = (unsigned int)v_out;
        unsigned int volts_decimal = (unsigned int)((v_out - volts_units) * 10);
        

        PORTC = display_digits_units[volts_units];
        PORTD = display_digits_decimal[volts_decimal];
    }
}