# Tiva Midi Player - ECE 425 Final Project
![Board](Images/Device)
## Introduction
When first introduced to the Tiva and presented with its capabilities for PWM programming, I was allured by the idea of designing a music box. Early labs that showcased musical notes being played through a tiny buzzer reminded me of early 16-bit gaming OSTs and how we could potentially design something to playback some of these nostalgic tracks. The project was created with the intent of designing a small player that would take in a MIDI file and output the entire track.

## Background and Methodology
This project utilizes many embedded system concepts learned throughout the semester. We initialized some pins as GPIOs for button inputs, LCD display output, as well as adding interrupts and delays with SysTick. Since many MIDI tracks have layered instruments, we used 4 PWMs to emulate multiple instrument sounds. The General Purpose Timer Module is also utilized throughout for configuring the PWM periods and duty cycles, as well as the SysTick Timer. To convert the MIDI files into something readable by the Tiva board, we used the [MidiFile](https://github.com/craigsapp/midifile) project to change the .mid file into a .c output for it to be parsed by the microcontroller.

Our contraption involved using PMOD buttons for controls and outputting the 4 PWM tracks to route through a Quad Tri-State Buffer, which were then routed to a potentiometer (volume knob) and sent to an Op-Amp, and then to a speaker as the final output. An LCD display embedded within the EduBase board was used to display the status of playing tracks.

## Block Diagram
![Block_Diagram](Images/Block_Diagram.jpg)
## Components Used

| Devices | Amount Used |
| --- | ---: |
| [Tiva TM4C123GH6PM](https://www.ti.com/lit/ds/spms376e/spms376e.pdf) | x1 |
| [SN74LS125 - Quad Tri-State Buffer](https://www.ti.com/lit/gpn/SN54LS125A) | x1 |
| [NJM386 - Audio Amp](https://docs.rs-online.com/d4d0/0900766b8002dd96.pdf) | x1 |
| 10K Potentiometer | x1 |
| 4 Ohm Speaker | x1 |
| [EduBase Board](https://trainer4edu.com/edubase_v2/index.html) | x1 |
| [PMOD Buttons](https://digilent.com/reference/pmod/pmodbtn/reference-manual) | x1 |


## Pinout Used

| Pin | Utilization | Input/Output |
| --- | :---: | ---: |
| PE 1-4 | PMOD BTN 0-3 | Inputs |
| PA 2-5 | LCD D4-7 | Outputs |
| PC6 | LCD Enable | Output |
| PE0 | LCD Register Select | Output |
| PB6 | PWM0_0 | Output |
| PD0 | PWM1_0 | Output |
| PF2 | PWM1_3 | Output |
| PC5 | PWM0_3 | Output |

## Analysis and Results

The MIDI player was capable of track playback on its own once configured. It included volume control, a Play/Stop button, an LCD status display, and onboard LED output during playback. MIDI files were parsed using the MidiFile code, then split into a maximum of four tracks, which were played across the four initialized PWM modules.

### Examples
[Super Mario Intro](https://drive.google.com/file/d/1Ec3wyDyLBsa6OF05u46dPFqC-WILQXnV/view?usp=drive_link)

[So Fresh, So Clean - OutKast](https://youtube.com/shorts/FWsr0I1MdNg?si=zJ8ugQ_CHx7RENt3)

[Yoshi's Island Intro](https://drive.google.com/file/d/1EeKvgMTFukUXmkjqyIFN_wXHTDJ6R_ms/view?usp=drive_link)

There were limitations in terms of configuration. At its conclusion, it was set to run tracks at a consistent tempo of 120 BPM. As a result, faster tracks (e.g., Flight of the Bumblebee) did not play correctly. Implementing variable tempo playback would have been a significant undertaking, requiring a longer timeline. Certain tones, such as tremolo noises and short percussion sounds within specific tracks, did not emulate correctly, likely due to the varying pitch and short duration of these sounds. Incorporating these would have also extended the project timeline. Additionally, the breadboard used to house some of the hardware introduced noise into the circuit, causing inconsistent pitch. Different capacitor values were tested on the amplifier to achieve the desired gain and filter out the noise.

## Conclusion

The desired design goal was achieved, albeit with some limitations. MIDI tracks with a maximum of four instruments, running at the standard tempo of 120 BPM or below, played as expected. Tracks with more than four instruments, varying pitches, or short percussive sounds were not emulated correctly.

Throughout the project, multiple embedded systems concepts were used to achieve the desired effects in the final design. From simple GPIO drivers, to dense looping of PWM channels, my understanding of the C language grew immensely. However, the hardware concepts were the main takeaway, as registers, timers, input/output ports, etc., are seemingly universal across all microcontrollers.

This project has become the catalyst for many upcoming microcontroller projects. Stay tuned!

## Works Cited
MidiFile: https://github.com/craigsapp/midifile

Project design inspired by: https://github.com/jspicer-code/Tiva-C-MusicPlayer