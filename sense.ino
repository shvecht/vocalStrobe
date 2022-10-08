 /*
 Sketch Name: sense
 
 partly based on sketch by Clyde A. Lettsome, PhD, PE, MEM. (https://clydelettsome.com/blog/2019/12/18/my-weekend-project-audio-frequency-detector-using-an-arduino/).
 
 Description: This sketch recognises the approximate frequency of the loudest sound detected by a sound detection module, and strobe light accordingly. For this project, the analog output from the sound module detector sends the analog audio signal detected to A1 of the Arduino Nano. The analog signal is sampled and quantized (digitized). A Fast Fourier Transform (FFT) is then performed on the digitized data. The FFT converts the digital data from the approximate discrete-time domain result. The maximum frequency of the approximate discrete-time domain result is then determined and transferred into a approximately one second loop of HIGH and LOW LED pin, which is than can be manually adusted using potentiometer connected to A0 pin, allowing for tracking vocal folds movement when connected to an endoscope.

 Note: The arduinoFFT.h library needs to be added to the Arduino IDE before compiling and uploading this script/sketch to an Arduino.
 
 Copyright (C) 2021 Isaac Shochat, MD.

 License: This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License (GPL) version 3, or any later version of your choice, as published by the Free Software Foundation. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/


#include <arduinoFFT.h>
#include<Wire.h>

#define SAMPLES 128              //SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 1024  //Ts = Based on Nyquist, must be 2 times the highest expected frequency.

arduinoFFT FFT = arduinoFFT();

unsigned int samplingPeriod;
unsigned long microSeconds;

double vReal[SAMPLES];  //create vector of size SAMPLES to hold real values
double vImag[SAMPLES];  //create vector of size SAMPLES to hold imaginary values

void setup()
{
  Wire.begin();
  // Serial.begin(9600);
  samplingPeriod = round(1000000 * (1.0 / SAMPLING_FREQUENCY));  //Period in microseconds
}

void loop()
{

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
  // Serial.println(peak);

  if (peak < 75) {
    send(0);
    PORTB |= _BV(5);  // digitalWrite(LED_BUILTIN, HIGH);
  } else {
    PORTB &= ~_BV(5);  // digitalWrite(LED_BUILTIN, LOW);
    send(peak);
  }

  delay(400); // optional, determins the rate of frequency update sent to the other unit
}

void send(int i) {
  Wire.beginTransmission(0x40);  //slaveAddress
  Wire.write(highByte(i)); //I2C is byte oriented; write(a);
  Wire.write(lowByte(i));
  Wire.endTransmission();
  // Serial.println(i);
}