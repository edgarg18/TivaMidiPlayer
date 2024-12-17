#include "TM4C123GH6PM.h"
#include "string.h"
#include "GPIO.h"
#include "PMOD_BTN_Interrupt.h"
#include "SysTick_Delay.h"
#include "PWM.h"
#include "midi.h"
#include "EduBase_LCD.h"

static volatile uint8_t isPlaying_ = 0;
static volatile uint8_t lcd_needs_update = 0;

extern MidiFile_t midi_tune;

void PMOD_BTN_Handler(uint8_t pmod_btn_status);
void UpdateLCDStatus(void);
	
typedef struct 
{
    PWMModule pwmModule; 
    PWMChannel pwmChannel;
    const MidiTrack_t* track;
    uint8_t isPlaying;
    uint32_t lastNoteTime;
    int noteIndex;
} TrackParams_t;

typedef struct 
{
    PWMModule pwmModule; 
    PWMChannel pwmChannel;
    const MidiTrack_t* track;
    uint32_t currentNoteIndex;
    uint32_t lastNoteTime;
    uint8_t isTrackPlaying;
    uint32_t nextNoteTime;
} TrackState_t;

//Mapping the 4 PWM's to the 4 Midi tracks
static TrackState_t trackStates_[4] = 
{
    { PWMModule0, PWM0_, NULL, 0, 0, 0,0 },    // PWM0_0 (PB6)
    { PWMModule1, PWM0_, NULL, 0, 0, 0,0 },    // PWM1_0 (PD0)
		{ PWMModule1, PWM3,  NULL, 0, 0, 0,0 },    // PWM1_3 (PF2)
		{ PWMModule0, PWM3,  NULL, 0, 0, 0,0 }     // PWM0_3 (PC5)

};


static void ProcessTracks(void) 
{
    uint32_t currentTime = GetTick();
    
    for (int trackIdx = 0; trackIdx < midi_tune.numTracks; trackIdx++) 
    {
        TrackState_t* state = &trackStates_[trackIdx];
        
        if (!state->isTrackPlaying || !state->track) 
        {
            continue;
        }
        
        const MidiNoteEvent_t* notes = state->track->notes;
        const MidiNoteEvent_t* currentNote = &notes[state->currentNoteIndex];
			// End of track
        if (currentNote->deltaTime == -1) 
        {
            state->isTrackPlaying = 0;
            PWM_Disable(state->pwmModule, state->pwmChannel);
            continue;
        }
            
        if (currentTime - state->lastNoteTime >= (currentNote->deltaTime * 1000)) 
        {
            uint8_t status = 0xF0 & currentNote->status;
            
            switch(status) 
            {
                case NOTE_ON:
                    if (currentNote->velocity > 0) {
                        uint16_t period = Midi_NotePwmPeriods[currentNote->key];
                        uint16_t duty = period / 2;
                        
                        
                        switch(state->pwmModule) 
                        {
                            case PWMModule0:
                                if(state->pwmChannel == PWM0_) 
                                {
                                    PWM0_0_Init(period, duty);
                                }
                                else if(state->pwmChannel == PWM3) 
                                {
                                    PWM0_3_Init(period, duty);
                                }
                                break;
																
                            case PWMModule1:
                                if(state->pwmChannel == PWM0_) 
                                {
                                    PWM1_0_Init(period, duty);
                                }
																
                                else if(state->pwmChannel == PWM3) 
                                {
                                    PWM1_3_Init(period, duty);
                                }
                                break;
                        }
                    } else {
                        PWM_Disable(state->pwmModule, state->pwmChannel);
                    }
                    break;
                    
                case NOTE_OFF:
                    PWM_Disable(state->pwmModule, state->pwmChannel);
                    break;
            }
            
            // Move to next note and update timing
            state->currentNoteIndex++;
            state->lastNoteTime = currentTime;
        }
    }
}



int main ()
{
	RGB_LED_Init();
	PMOD_BTN_Interrupt_Init(&PMOD_BTN_Handler);
	SysTick_Delay_Init();
	PWM_Clock_Init();
	EduBase_LCD_Init();
	EduBase_LCD_Clear_Display();
  UpdateLCDStatus();
	uint16_t testPeriodC = 4778;  // Middle C (C4) Used for debugging
  uint16_t testDuty = testPeriodC / 2;
	
	// Initialize the PWM channels
	PWM0_0_Init(testPeriodC, 0); //PB6
	PWM1_0_Init(testPeriodC, 0); // PD0
	PWM1_3_Init(testPeriodC, 0); //PF2
	PWM0_3_Init(testPeriodC, 0); // PC5
	
	for (int i = 0; i < midi_tune.numTracks && i < 4; i++) 
    {
        trackStates_[i].track = &midi_tune.tracks[i];
        trackStates_[i].currentNoteIndex = 0;
        trackStates_[i].lastNoteTime = 0;
        trackStates_[i].isTrackPlaying = 0;
    }
	
	
	while(1)
	{
		ProcessTracks();
		if(lcd_needs_update)
		{
			UpdateLCDStatus();
			lcd_needs_update = 0;
		}
	}
}


void PMOD_BTN_Handler(uint8_t pmod_btn_status)
{
		uint32_t currentTime = GetTick();
	
    switch(pmod_btn_status)
    {
        // BTN0 (PE1)
        case 0x04: //Reset button
        {
//					for (int i = 0; i < midi_tune.numTracks; i++) 
//            {
//                // Reset track position but maintain play state
//                trackStates_[i].currentNoteIndex = 0;
//                trackStates_[i].lastNoteTime = currentTime;
//                
//                if (!isPlaying_) 
//                {
//                    PWM_Disable(trackStates_[i].pwmModule, trackStates_[i].pwmChannel);
//                }
//            }
//            RGB_LED_Output(RGB_LED_BLUE);
						break;
        }

        // BTN1 (PE2)
        case 0x08:
        {
					isPlaying_ = !isPlaying_;
					
					for (int i = 0; i < midi_tune.numTracks; i++) 
					{
							trackStates_[i].isTrackPlaying = isPlaying_;
							if (isPlaying_) 
							{             
									trackStates_[i].currentNoteIndex = 0;
									trackStates_[i].lastNoteTime = currentTime;
									RGB_LED_Output(RGB_LED_RED);
							} 
							else 
							{
									PWM_Disable(trackStates_[i].pwmModule, trackStates_[i].pwmChannel);
									RGB_LED_Output(RGB_LED_OFF);
							}
            }
            lcd_needs_update = 1;
            break;
        }

        // BTN2 (PE3)
        case 0x10: 
        {

            break;
        }

        // BTN3 (PE4)
        case 0x20:
        {
					
            break;
        }

        default:
        {
            break;
        }
    }
}


void UpdateLCDStatus(void)
{
     // Play state
    EduBase_LCD_Set_Cursor(0, 0);
    EduBase_LCD_Display_String("Status: ");
    EduBase_LCD_Display_String(isPlaying_ ? "Playing" : "Paused ");
    
    //Time
    EduBase_LCD_Set_Cursor(0, 1);
		
}