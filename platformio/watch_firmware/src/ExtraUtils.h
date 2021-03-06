/*
  Description: Some extra helper functions are placed here
  Author: Saurabh datta.
  Time: Aug, 2021.
  Loc: Beijing, China.
  License: MIT
*/

void disableUnusedPins()
{
    //  Set them to input pull and do not keep them floating
    PORTC.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN1CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTC.PIN4CTRL = PORT_PULLUPEN_bm;
}

void disableSerialHWPins()
{
    PORTB.DIRSET = PIN2_bm; // TX
    PORTB.DIRSET = PIN3_bm; // RX
    cli();
    PORTB.OUT &= ~PIN2_bm;
    PORTB.OUT &= ~PIN3_bm;
    sei();
}

void disableTWI()
{
    //  TWI0.MCTRLA &= ~(TWI_ENABLE_bm);
    PORTB.DIRSET = PIN0_bm;
    PORTB.DIRSET = PIN1_bm;
    cli();
    PORTB.OUT &= ~PIN0_bm;
    PORTB.OUT &= ~PIN1_bm;
    sei();
}

//-----------------------------------//
//----- LOW VOLTAGE CALCULATION -----//
//-----------------------------------//
// Courtesy: http://www.technoblogy.com/show?3K82 & http://www.technoblogy.com/list?3KKM

void ADCVoltRefSetup()
{
    VREF.CTRLA = VREF_ADC0REFSEL_1V1_gc;
    ADC0.CTRLC = ADC_REFSEL_VDDREF_gc | ADC_PRESC_DIV256_gc; // 78kHz clock
    ADC0.MUXPOS = ADC_MUXPOS_INTREF_gc;                      // Measure INTREF
    ADC0.CTRLA = ADC_ENABLE_bm;                              // Single, 10-bit
}

uint16_t measuredVoltage()
{
    uint16_t v;
    ADC0.COMMAND = ADC_STCONV_bm; // Start conversion
    while (ADC0.COMMAND & ADC_STCONV_bm)
        ;                            // Wait for completion
    uint16_t adc_reading = ADC0.RES; // ADC conversion result
    v = 11264 / adc_reading;
    //  Buffer[0] = voltage / 10; Buffer[1] = voltage % 10;
    return v;
}
