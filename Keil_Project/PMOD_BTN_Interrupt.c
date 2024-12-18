 /**
 * @file PMOD_BTN_Interrupt.h
 *
 * @brief Header file for the PMOD_BTN_Interrupt driver.
 *
 * This file contains the function definitions for the PMOD_BTN_Interrupt driver.
 * It interfaces with the PMOD BTN module. The following pins are used:
 * 	- BTN0 (PE1)
 *	- BTN1 (PE2)
 *	- BTN2 (PE3)
 *	- BTN3 (PE4)
 *
 * It configures the pins to trigger interrupts on rising edges. The PMOD BTN
 * push buttons operate in an active high configuration.
 *
 * @author Aaron Nanas
 */
 
#include "PMOD_BTN_Interrupt.h"
 
// Declare pointer to the user-defined task
void (*PMOD_BTN_Task)(uint8_t pmod_btn_state);

void PMOD_BTN_Interrupt_Init(void(*task)(uint8_t))
{
	// Store the user-defined task function for use during interrupt handling
	PMOD_BTN_Task = task;
	
	// Enable the clock to Port E by setting the
	// R0 bit (Bit 0) in the RCGCGPIO register
	SYSCTL->RCGCGPIO |= 0x10;
	
	// Configure the PE1, PE2, PE3, and PE4 pins as input
	// by clearing Bits 4 to 1 in the DIR register
	GPIOE->DIR &= ~0x1E;
	
	// Configure the PE1, PE2, PE3, and PE4 pins to function as
	// GPIO pins by clearing Bits 5 to 2 in the AFSEL register
	GPIOE->AFSEL &= ~0x1E;
	
	// Enable the digital functionality for the PE1, PE2, PE3, and PE4 pins
	// by setting Bits 5 to 2 in the DEN register
	GPIOE->DEN |= 0x1E;
	
	// Enable the weak pull-down resistor for the PE1, PE2, PE3, and PE4 pins
	// by setting Bits 5 to 2 in the PDR register
	GPIOE->PDR |= 0x1E;
	
	// Configure the PE1, PE2, PE3, and PE4 pins to detect edges
	// by clearing Bits 5 to 2 in the IS register
	GPIOE->IS &= ~0x1E;
	
	// Allow the GPIOIEV register to handle interrupt generation
	// and determine which edge to check for the PE1, PE2, PE3, and PE4 pins 
	// by clearing Bits 5 to 2 in the IBE register
	GPIOE->IBE &= ~0x1E;
	
	// Configure the PE1, PE2, PE3, and PE4 pins to detect
	// rising edges by setting Bits 5 to 2 in the IEV register
	// Rising edges on the corresponding pins will trigger interrupts
	GPIOE->IEV |= 0x1E;
	
	// Clear any existing interrupt flags on the PE1, PE2, PE3, and PE4 pins
	// by setting Bits 5 to 2 in the ICR register
	GPIOE->ICR |= 0x1E;
	
	// Allow the interrupts that are generated by the PE1, PE2, PE3, and PE4 pins
	// to be sent to the interrupt controller by setting
	// Bits 5 to 2 in the IM register
	GPIOE->IM |= 0x1E;
	
	// Clear the INTA field (Bits 7 to 5) of the IPR[0] register (PRI0)
	//NVIC->IPR[0] &= ~0x000000E0;
	NVIC->IPR[1] &= ~(0xFF << 0);
	
	// Set the priority level of the interrupts to 3. Port E has an Interrupt Request (IRQ) number of 0
	NVIC->IPR[1] |= (3 << 5);
	
	// Enable IRQ 0 for GPIO Port A by setting Bit 0 in the ISER[0] register
	NVIC->ISER[0] |= (1 << 4);
}

uint8_t PMOD_BTN_Read(void)
{
	// Declare a local variable to store the status of the PMOD BTN
	// Then, read the DATA register for Port E
	// A "0x3C" bit mask is used to capture only the pins used the PMOD BTN
	uint8_t pmod_btn_state = GPIOE->DATA & 0x1E;
	
	// Return the status of the PMOD BTN module
	return pmod_btn_state;
}

void GPIOE_Handler(void)
{
	// Check if an interrupt has been triggered by any of
	// the following pins: PA5, PA4, PA3, and PA2
	if (GPIOE->MIS & 0x1E)
	{
		// Execute the user-defined function
		(*PMOD_BTN_Task)(PMOD_BTN_Read());
		
		// Acknowledge the interrupt from any of the following pins
		// and clear it: PE1, PE2, PE3, and PE4
		GPIOE->ICR |= 0x1E;
	}
}
