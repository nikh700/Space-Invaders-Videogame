# Space Invaders (TM4C123)

A Space Invaders-style game for the Texas Instruments TM4C123 (Tiva C) LaunchPad with an ST7735 LCD, button inputs, and DAC-based audio. The game includes bilingual UI (English/Spanish), three difficulty modes, and classic invader wave gameplay.

## Features
- Space Invaders clone with 18 enemies and score tracking
- English/Spanish language selection
- Difficulty modes: Easy, Medium, Hard
- Analog ship movement via slide potentiometer
- Button-based shooting, pause, and menu navigation
- DAC-based audio effects

## Hardware Requirements
- **MCU**: TM4C123GH6PM (Tiva C LaunchPad)
- **Display**: ST7735 LCD
- **Controls**:
  - Slide potentiometer for horizontal movement (ADC input)
  - Four buttons on Port E (PE0–PE3)
- **Audio**: 6-bit R-2R DAC on Port B (PB0–PB5)
- **LEDs**: Port D (PD0–PD1) for status feedback

### Wiring (from `SpaceInvaders.c`)
- Slide pot pin 1 → GND
- Slide pot pin 2 → PD2 / AIN5
- Slide pot pin 3 → +3.3V
- Buttons → PE0–PE3
- DAC bits 0–5 → PB0–PB5
- LEDs → PD0–PD1

## Controls
- **Language Select**:
  - Up (PE3) → English
  - Down (PE2) → Spanish
- **Instructions Screen**: press any button to continue
- **Mode Select**:
  - Left (PE1) → Easy
  - Up (PE3) → Medium
  - Right (PE0) → Hard
- **In-Game**:
  - Shoot: Up (PE3)
  - Pause: Down (PE2)
  - Resume from Pause: Right (PE0)
- **Movement**: slide potentiometer (ADC)

## Build & Flash
This project is set up for **Keil MDK-ARM (uVision)** with the TM4C123 device pack.

1. Install **Keil MDK-ARM v5**.
2. Install device packs:
   - **Keil::TM4C_DFP@1.1.0**
   - **ARM::CMSIS@6.1.0**
3. Ensure the Valvano/EE319Kware support libraries are available (e.g., `tm4c123gh6pm.h`, `ST7735`, `ADC`, `wave`) and included in the project include paths.
4. Open `SpaceInvaders.uvprojx` in Keil.
5. Build and flash to the TM4C123 board.

## Project Structure
- `SpaceInvaders.c` — main game logic, UI, and control flow
- `Timer0.c`, `Timer1.c` — timers for ADC and enemy movement
- `Random.*` — random number utilities
- `Images.h` — sprite data generated from BMPs
- `*.bmp` / `*.txt` — raw assets and converted image data
- `SpaceInvaders.uvprojx` — Keil project file

## Credits
- Original Space Invaders references and sounds listed in `SpaceInvaders.c`
- Portions of the starter project and libraries are from Jonathan & Daniel Valvano (UT Austin EE319K)
