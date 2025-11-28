/* Workflow:
1. Timer starts when Sensor 1 (S1) is interrupted
2. Keeps counting
3. Timer stops when Sensor 2 (S2) is interrupted
4. Displays final time for 15 seconds 
5. Display resets to 00.00
*/

#include <TM1650.h>
#include <TM16xxDisplay.h>

// Display Connection
#define DIO 4
#define CLK 5
// Display setup: Name_module(DIO, CLK, number_of_digits, common_anode, brightness)
TM1650 module(DIO, CLK, 4, true, 7);
TM16xxDisplay display(&module, 4);

// 7 Segment bitmaps
const uint8_t digitSegs[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

// IR break-beam sensors Connections
const uint8_t S1 = 2;                  // Sensor 1
const uint8_t S2 = 3;                  // Sensor 2

// LED Connections (just for test)
const uint8_t GreenLED = 8;            // Green LED
const uint8_t YellowLED = 13;          // Yellow LED

// Timer setup
const unsigned long DISPLAY_UPDATE_MS = 10UL;   // update every 10ms to display centiseconds
unsigned long lastDisplayUpdate = 0;
unsigned long startMillis = 0;
unsigned long stopMillis = 0;

// Timer states
bool running = false;
bool stopped = false;

// Main loop
void setup() {
  // Reading sensors, because of INPUT_PULLUP, HIGH means beam is intact (no interrupt), LOW means beam broken (triggered)
  pinMode(S1, INPUT_PULLUP);      // IR break-beam sensor S1 set as Input Pullup
  pinMode(S2, INPUT_PULLUP);      // IR break-beam sensor S1 set as Input Pullup
  pinMode(GreenLED, OUTPUT);      // Set Green LED as Output
  pinMode(YellowLED, OUTPUT);     // Set Yellow LED as Output

  digitalWrite(GreenLED, LOW);    // Set Green LED as low initially
  digitalWrite(YellowLED, LOW);   // Set Yellow LED as low initially

  // Set display initially at 00.00
  module.begin(true, 7);
  module.setSegments(digitSegs[0], 0);
  module.setSegments(digitSegs[0] | 0x80, 1); // decimal point in SS.CC
  module.setSegments(digitSegs[0], 2);
  module.setSegments(digitSegs[0], 3);
}

void loop() {
  bool S1Uninterrupted = (digitalRead(S1) == HIGH);
  bool S2Uninterrupted = (digitalRead(S2) == HIGH);

  // Start trigger: sensor S1 inturrupted (LOW)
  // If not already running or stopped and S1 is interrupted (LOW), start timing.
  if (!running && !stopped && !S1Uninterrupted) {
    running = true;
    stopped = false;
    startMillis = millis();         // Start time
    digitalWrite(YellowLED, HIGH);  // Indicate running
  }

  // Stop trigger: sensor S2 inturrupted (LOW)
  // If currently running and S2 is interrupted (LOW), stop timing.
  if (running && !S2Uninterrupted) {
    running = false;
    stopped = true;
    stopMillis = millis();           // Stop time
    // Do nothing to time so that display remains frozen at last recorded time
    digitalWrite(GreenLED, HIGH);    // Indicate stopped
    updateDisplay(stopMillis - startMillis);
  }

  // Keep incresing time in between 2 inturrupts
  // Continuously update display using elapsed millis
  if (running) {
    unsigned long elapsed = millis() - startMillis;
    updateDisplay(elapsed);
  }

  // Set display to 00.00 if not running
  if (!running && !stopped) {
    module.setSegments(digitSegs[0], 0);
    module.setSegments(digitSegs[0] | 0x80, 1); // decimal point in SS.CC
    module.setSegments(digitSegs[0], 2);
    module.setSegments(digitSegs[0], 3);
  }

  // wait 15 seconds after stop, then auto reset to 00.00
  if (stopped) {
    if ((millis() - stopMillis) >= 15000UL && S1Uninterrupted && S2Uninterrupted) {
    stopped = false;
    // Reset display to 00.00
    module.setSegments(digitSegs[0], 0);
    module.setSegments(digitSegs[0] | 0x80, 1); // decimal point in SS.CC
    module.setSegments(digitSegs[0], 2);
    module.setSegments(digitSegs[0], 3);
  }
}

}

// Convert totalSeconds to SS.CC format and send to display
void updateDisplay(unsigned long elapsedMillis) {
  unsigned long centis_total = elapsedMillis / 10UL;    // total milliseconds
  unsigned int seconds = (centis_total / 100UL) % 100U; // wrap at 99
  unsigned int centis = centis_total % 100U;            // wrap at 99

  // Break into four digits: SS.CC
  uint8_t s_tens  = seconds / 10;
  uint8_t s_units = seconds % 10;
  uint8_t c_tens  = centis / 10;
  uint8_t c_units = centis % 10;

  module.setSegments(digitSegs[s_tens], 0);         // S
  module.setSegments(digitSegs[s_units] | 0x80, 1); // S, decimal point in SS.CC
  module.setSegments(digitSegs[c_tens], 2);         // C
  module.setSegments(digitSegs[c_units], 3);        // C
}

