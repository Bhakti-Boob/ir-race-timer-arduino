# Automated, IR-based precision timing and race management system with Arduino
Timing system using dual sensors and a 4‑digit 7‑segment display to measure robot lap times with centisecond resolution and reproducible evaluations.

# Overview
This project implements a precise race management timer for robots. An IR break‑beam at the start line begins timing, a second beam at the finish line stops it and the elapsed time is shown as `SS.CC` (seconds.centiseconds) on a 4‑digit display.  
The system was designed for use in RPTU's Assembler programming lab where line folower robots are tested. This project improves measurement reliability during race competitions and makes performance comparisons repeatable between runs.

<div align="center">
  <video width="600" height="300" alt="Block_Diagram" src="https://github.com/user-attachments/assets/63ed819e-2156-455a-87eb-a15926a1db51" />
</div>

# Features
- Two IR break‑beam sensors for start (S1) and finish (S2) triggers
- Elapsed time displayed in SS.CC format (up to 99.99 s) on a 4‑digit 7‑segment display
- Centisecond resolution with continuous display updates while the robot is on track  
- Automatic hold of the final result for 15 seconds, then reset to `00.00` when beams are clear  
- Simple (optional) LED indicators for “timing in progress” and “timing stopped / result ready”

# Hardware and components used
- Arduino Uno R3  
- 2 × Adafruit IR Break Beam Sensor (5 mm)
- Adafruit HT16K33 4-Digit 7-Segment Display with I2C Backpack  
- Status LEDs (optional)
  - Green LED 
  - Red LED
- 5 V adapter for supplying power to Arduino

<div align="left">
  <img width="400" height="350" alt="wiring_diagram" src="https://github.com/user-attachments/assets/209e9ed1-6a97-4d67-854d-bbd5083902f3" />
</div>

# Timing Logic
1. Idle  
   - Display shows `00.00`.  
   - Both IR sensors see intact beams (HIGH).  
2. Start  
   - When S1 goes LOW (beam broken) and the timer is idle, the system:  
     - Records `startMillis = millis()`.  
     - Sets `running = true`, `stopped = false`.  
     - Turns on the Green LED to indicate timing in progress.  
3. Running
   - While `running == true`, the loop computes `elapsed = millis() - startMillis` and converts it to centiseconds.  
   - The `updateDisplay()` function formats the time into digits and updates the display as `SS.CC`.  
4. Stop 
   - When S2 goes LOW while `running == true`, the system:  
     - Records `stopMillis = millis()`.  
     - Sets `running = false`, `stopped = true`.  
     - Freezes the display at the final elapsed time.  
     - Turns on the Red LED to indicate a valid result.  
5. Auto Reset 
   - While `stopped == true`, the code waits until both beams are clear again and at least 15 seconds have passed since `stopMillis`.  
   - After that, it resets `stopped = false`, turns off LEDs, and restores the display to `00.00` ready for the next run.

# How to Use This Repository
- /src/ – Arduino sketch for timing display and IR sensor integration
- /circuit_diagrams/ – Schematics, wiring diagram
- /docs/ – Photo and video of the assembled prototype

# Possible Extensions
- Add a lap counter or "best time" memory over multiple runs.  
- Log times over serial or to an SD card for later analysis.  
