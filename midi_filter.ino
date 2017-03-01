// Arduino Leonardo MIDI filter

// Filters sequencer related MIDI messages (clock, start, stop, etc.) out from other messages.
// Four LEDs indicate input and output data and filter activity, which can be disabled by button.

byte midi_start = 0xfa;
byte midi_stop = 0xfc;
byte midi_clock = 0xf8;
byte midi_continue = 0xfb;

const int LED_RECEIVED_PIN = 7;
const int LED_FILTERED_PIN = 3;
const int LED_OTHER_PIN = 4;
const int LED_ACTIVE_PIN = 5;
const int BUTTON_PIN = 6;

unsigned long lastReceived = 0;
unsigned long lastFiltered = 0;
unsigned long lastOther = 0;
unsigned long lastDebounceTime = 0;

const unsigned long debounceDelay = 50;
const unsigned long ledTime = 100;

int buttonState;   
int lastButtonState = LOW;
int filterActive = 1;

void setup() {
  pinMode(LED_RECEIVED_PIN, OUTPUT);
  pinMode(LED_FILTERED_PIN, OUTPUT);
  pinMode(LED_OTHER_PIN, OUTPUT);
  pinMode(LED_ACTIVE_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  digitalWrite(LED_RECEIVED_PIN, LOW);
  digitalWrite(LED_FILTERED_PIN, LOW);
  digitalWrite(LED_OTHER_PIN, LOW);
  digitalWrite(LED_ACTIVE_PIN, filterActive);

  digitalWrite(LED_ACTIVE_PIN, filterActive);

  // debugging
  Serial.begin(9600);

  // MIDI
  Serial1.begin(31250);
}

void loop() {
  byte data;

  unsigned long now = millis();

  if(Serial1.available() > 0) {
    data = Serial1.read();

    // we have received some MIDI data - turn on indicating LED
    digitalWrite(LED_RECEIVED_PIN, HIGH);
    lastReceived = now;

    // filter sequencing data
    if (data == midi_start || data == midi_stop || data == midi_continue || data == midi_clock) {
      // send filtered data to MIDI output
      Serial1.write(data);

      // turn on LED indicating filtered data
      digitalWrite(LED_FILTERED_PIN, HIGH);
      lastFiltered = now;
    } else if (!filterActive) {
      // if filter function is disabled, send other MIDI data to output
      Serial1.write(data);

      // turn on LED indicating other MIDI data
      digitalWrite(LED_OTHER_PIN, HIGH);
      lastOther = now;
    }
  }

  // turn off LEDs after some time
  if (now - lastReceived > ledTime) {
    digitalWrite(LED_RECEIVED_PIN, LOW);
  }
  
  if (now - lastFiltered > ledTime) {
    digitalWrite(LED_FILTERED_PIN, LOW);
  }
  
  if (now - lastOther > ledTime) {
    digitalWrite(LED_OTHER_PIN, LOW);
  }

  // debounce button
  int currentButtonState = digitalRead(BUTTON_PIN);
  if (currentButtonState != lastButtonState) {
    lastButtonState = currentButtonState;
    lastDebounceTime = now;
  }
  
  if ((now - lastDebounceTime) > debounceDelay) {
    if (currentButtonState != buttonState) {
      buttonState = currentButtonState;

      // button is pressed -- toggle active flag
      if (buttonState == LOW) {
        filterActive = !filterActive;
        digitalWrite(LED_ACTIVE_PIN, filterActive);
      }
    }
  }
}


