#include <msp430fr5994.h>

#define LED1   BIT0     // Defines the LED at P1.0
#define BUTTON  BIT6    // Defines the BUTTON at P5.6
/**
 * Nathan Sulzer
 * Into to Embedded Systems
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;           // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;               // Disable GPIO default high-impedance mode
    P1DIR  |= LEDG;                     // Set P1.6 Green LED to output
    P5REN  |= BUTTON;                   // Enables resistor for BUTTON P1.3
    P5OUT  = BUTTON;                    // Makes resistor P1.3 a pull up
    P5IES = 0x0000;                     // Interrupts when BUTTON is pressed or low to high transition
    P5IE |= BUTTON;                     // Interrupt enable set to BUTTON
    P5IFG &= ~BUTTON;                   // Clear interrupt flag

    TB0CCTL0 = 0x0010;                  // Enables Capture/compare register for interrupt of timer A0
    TB0CCR0 = 60000;                    // Set value for Timer A0 to count up to
    TB0CTL = TBSSEL_2 + MC_0;           // Timer_A Control Register - SMCLK high frequency clock, timer stopped

    __enable_interrupt();

    __bis_SR_register(LPM0 + GIE);      // Low Power Mode w/ general interrupts enabled

}

// Interrupt Service Routine 1 which toggles on the LED on the release of the Button and starts timer a to act as a delay
#pragma vector = PORT5_VECTOR
__interrupt void Interrupt_1(void)
{
    P1OUT ^= LEDG;                      // Toggle LED
    P5IE &= ~BUTTON;                    // reset interrupt
    TB0CTL = TBSSEL_2 + MC_1;           // Timer_A Control Register - SMCLK high frequency clock, Up
}

// Interrupt Service Routine 2 which stops Timer A0 and resets the interrupt
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Interrupt_2(void)
{
    TB0CTL = MC_0;                      // Stop timer
    P5IE |= BUTTON;                     // Interrupt enable set to BUTTON
    P5IFG = 0x0000;                     // Clear interrupt flag
}
