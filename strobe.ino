/*
 Sketch Name: vocalStrobe
 
 partly based on sketch by Clyde A. Lettsome, PhD, PE, MEM. (https://clydelettsome.com/blog/2019/12/18/my-weekend-project-audio-frequency-detector-using-an-arduino/).
 
 Description: This sketch recognises the approximate frequency of the loudest sound detected by a sound detection module, and strobe light accordingly. For this project, the analog output from the sound module detector sends the analog audio signal detected to A1 of the Arduino Nano. The analog signal is sampled and quantized (digitized). A Fast Fourier Transform (FFT) is then performed on the digitized data. The FFT converts the digital data from the approximate discrete-time domain result. The maximum frequency of the approximate discrete-time domain result is then determined and transferred into a approximately one second loop of HIGH and LOW LED pin, which is than can be manually adusted using potentiometer connected to A0 pin, allowing for tracking vocal folds movement when connected to an endoscope.

 Note: The arduinoFFT.h library needs to be added to the Arduino IDE before compiling and uploading this script/sketch to an Arduino.
 
 Copyright (C) 2021 Isaac Shochat, MD.

 License: This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License (GPL) version 3, or any later version of your choice, as published by the Free Software Foundation. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <arduinoFFT.h>
#include <avr/interrupt.h>

#define SAMPLES 128              //SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 1024  //Ts = Based on Nyquist, must be 2 times the highest expected frequency.

arduinoFFT FFT = arduinoFFT();

unsigned int samplingPeriod;
unsigned long microSeconds;

volatile double vReal[SAMPLES];  // buffer for real values
volatile double vImag[SAMPLES];  // buffer for imaginary values
volatile byte sampleIndex = 0;   // current position in the buffer
volatile bool bufferReady = false;  // set once SAMPLES are captured

double prevFreq = 0;  // last detected frequency used for strobing

void setupTimer1() {
  cli();                 // disable interrupts while configuring
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // compare match every ~977us using prescaler 8
  OCR1A = 1953;          // (977us / 0.5us) - 1
  TCCR1B |= _BV(WGM12);  // CTC mode
  TCCR1B |= _BV(CS11);   // prescaler 8
  TIMSK1 |= _BV(OCIE1A); // enable compare match interrupt
  sei();                 // enable interrupts
}

ISR(TIMER1_COMPA_vect) {
  if (bufferReady) return;  // ignore samples until processed

  vReal[sampleIndex] = analogRead(A1);
  vImag[sampleIndex] = 0;
  sampleIndex++;

  if (sampleIndex >= SAMPLES) {
    sampleIndex = 0;
    bufferReady = true;
  }
}

void setup() {
  samplingPeriod = 1000000UL / SAMPLING_FREQUENCY;  //Period in microseconds
  DDRD |= _BV(5);                                                // pinMode(5, OUTPUT);
  DDRB |= _BV(5);                                                // pinMode(LED_BUILTIN, OUTPUT);
  setupTimer1();
  // Serial.begin(9600);                                         //Baud rate for the Serial Monitor
}

void loop() {

  if (bufferReady) {
    FFT.Windowing((double*)vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute((double*)vReal, (double*)vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude((double*)vReal, (double*)vImag, SAMPLES);

    double peak = FFT.MajorPeak((double*)vReal, SAMPLES, SAMPLING_FREQUENCY);
    prevFreq = peak;
    bufferReady = false;  // allow ISR to fill buffer again
  }

  if (prevFreq < 75) {
    PORTB |= _BV(5);  // digitalWrite(LED_BUILTIN, HIGH);
  } else {
    PORTB &= ~_BV(5);  // digitalWrite(LED_BUILTIN, LOW);
    strobe(prevFreq);
  }
}

void strobe(double freq) {

  int reading = analogRead(A0);
  int adjustment = map(reading, 0, 1023, -19, 20);

  // Set the interval, and 'On' portion of the strobe using integer math
  unsigned int freqInt = (unsigned int)freq;
  int adjustedFreq = freqInt + adjustment;
  if (adjustedFreq <= 0) {
    adjustedFreq = 1;  // prevent divide by zero or negative values
  }
  unsigned long interval = 1000000UL / adjustedFreq;
  unsigned long pause = interval / 5;
  interval -= pause;

  // Looping 'On' and 'Off', for one second
  for (int i = 0; i < freq; i++) {
    PORTD |= _BV(5);  // digitalWrite(ledPin, HIGH);
    delayMicroseconds(pause);
    PORTD &= ~_BV(5);  // digitalWrite(ledPin, LOW);
    delayMicroseconds(interval);
  }
}
