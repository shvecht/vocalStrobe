#include <arduinoFFT.h>

#define SAMPLES 64              //SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 1024  //Ts = Based on Nyquist, must be 2 times the highest expected frequency.

arduinoFFT FFT = arduinoFFT();

unsigned int samplingPeriod;
unsigned long microSeconds;

double vReal[SAMPLES];  //create vector of size SAMPLES to hold real values
double vImag[SAMPLES];  //create vector of size SAMPLES to hold imaginary values

const int ledPin = 5;  //LED pin
const int modeButton = 2; //Button pin
int ledState = LOW;

byte mode = 0; // 0 = Off, 1 = On, 2 = Strobe.
byte buttonState = 0;            
byte lastButtonState = 0; 

void setup() {
  samplingPeriod = round(1000000 * (1.0 / SAMPLING_FREQUENCY));  //Period in microseconds
  pinMode(ledPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(modeButton, INPUT_PULLUP);
  Serial.begin(9600);  //Baud rate for the Serial Monitor
}

void loop() {

  buttonState = digitalRead(modeButton);

    // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      mode++;
      if (mode == 3) { mode = 0; }
    } else {
      // if the current state is LOW then the button went from on to off:
    }
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;


  /*Sample SAMPLES times*/
  for (int i = 0; i < SAMPLES; i++) {
    microSeconds = micros();  //Returns the number of microseconds since the Arduino board began running the current script.

    vReal[i] = analogRead(A1);  //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
    vImag[i] = 0;               //Makes imaginary term 0 always

    /* remaining wait time between samples if necessary */
    while (micros() < (microSeconds + samplingPeriod)) {}
  }

  /*Perform FFT on samples*/
  if (mode == 0) { digitalWrite(ledPin, LOW); }
  else if (mode == 1) { digitalWrite(ledPin, HIGH); }
  else {
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

    /*Find peak frequency */
    double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);

    if (peak < 75) {
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println(peak);
      strobe(peak);
    }
  }
}

void strobe(double freq) {

  float reading = analogRead(A0);
  float adjustment = map(reading, 0, 1024, -25, 26);

  // Set the interval, and 'On' portion of the strobe
  int interval = 1000000 / (freq);
  int pause = interval / 5;
  interval -= pause;

  // Looping 'On' and 'Off', for one second
  for (int i = 0; i < freq; i++) {
    digitalWrite(ledPin, LOW);
    delayMicroseconds(interval);
    digitalWrite(ledPin, HIGH);
    delayMicroseconds(pause);
  }
}
