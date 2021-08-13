#include <arduinoFFT.h>

#define SAMPLES 128              //SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 1024  //Ts = Based on Nyquist, must be 2 times the highest expected frequency.

arduinoFFT FFT = arduinoFFT();

unsigned int samplingPeriod;
unsigned long microSeconds;

double vReal[SAMPLES];  //create vector of size SAMPLES to hold real values
double vImag[SAMPLES];  //create vector of size SAMPLES to hold imaginary values

void setup() {
  samplingPeriod = round(1000000 * (1.0 / SAMPLING_FREQUENCY));  //Period in microseconds
  DDRD |= _BV(5);                                                // pinMode(5, OUTPUT);
  DDRB |= _BV(5);                                                // pinMode(LED_BUILTIN, OUTPUT);
  // Serial.begin(9600);                                         //Baud rate for the Serial Monitor
}

void loop() {

  /*Sample SAMPLES times*/
  for (int i = 0; i < SAMPLES; i++) {
    microSeconds = micros();  //Returns the number of microseconds since the Arduino board began running the current script.

    vReal[i] = analogRead(A1);  //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
    vImag[i] = 0;               //Makes imaginary term 0 always

    /* remaining wait time between samples if necessary */
    while (micros() < (microSeconds + samplingPeriod)) {}
  }

  /*Perform FFT on samples*/
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

  /*Find peak frequency */
  double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
  // Serial.println(peak);  // Printing the percieved frequency to the serial monitor.

  if (peak < 75) {
    PORTB |= _BV(5);  // digitalWrite(LED_BUILTIN, HIGH);
  } else {
    PORTB &= ~_BV(5);  // digitalWrite(LED_BUILTIN, LOW);
    strobe(peak);
  }
}

void strobe(double freq) {

  float reading = analogRead(A0);
  float adjustment = map(reading, 0, 1024, -19, 20);

  // Set the interval, and 'On' portion of the strobe
  int interval = 1000000 / (freq + adjustment);
  int pause = interval / 5;
  interval -= pause;

  // Looping 'On' and 'Off', for one second
  for (int i = 0; i < freq; i++) {
    PORTD |= _BV(5);  // digitalWrite(ledPin, HIGH);
    delayMicroseconds(pause);
    PORTD &= ~_BV(5);  // digitalWrite(ledPin, LOW);
    delayMicroseconds(interval);
  }
}
