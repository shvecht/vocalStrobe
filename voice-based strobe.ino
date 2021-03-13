#include "arduinoFFT.h"
 
#define SAMPLES 128             //SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 1024 //Ts = Based on Nyquist, must be 2 times the highest expected frequency.
 
arduinoFFT FFT = arduinoFFT();
 
unsigned int samplingPeriod;
unsigned long microSeconds;
 
double vReal[SAMPLES]; //create vector of size SAMPLES to hold real values
double vImag[SAMPLES]; //create vector of size SAMPLES to hold imaginary values

const int ledPin = 5; //the LED pin

unsigned long previousMicros = 0; //previous time point
unsigned long currentMicros = 0; //current time point

long interval = 500000; //initial interval between blinks
long wait;
 
void setup() 
{
    samplingPeriod = round(1000000*(1.0/SAMPLING_FREQUENCY)); //Period in microseconds 
    pinMode(5, OUTPUT);
}
 
void loop() 
{  
    int reading = analogRead(A0);
    float adjustment = map(reading, 0, 1024, -25, 26);

    /*Sample SAMPLES times*/
    for(int i=0; i<SAMPLES; i++)
    {
        microSeconds = micros();    //Returns the number of microseconds since the Arduino board began running the current script. 
     
        vReal[i] = analogRead(A1); //Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
        vImag[i] = 0; //Makes imaginary term 0 always

        /*remaining wait time between samples if necessary*/
        while(micros() < (microSeconds + samplingPeriod))
        {
          //wait for it...
        }

        currentMicros = micros();
        if (currentMicros - previousMicros >= interval) {
          previousMicros = currentMicros;

         digitalWrite(ledPin, HIGH);
         delayMicroseconds(wait);
         digitalWrite(ledPin, LOW);

        }
    }
 
    /*Perform FFT on samples*/
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

    /*Find peak frequency */
    double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
    interval = (1 / (peak + adjustment)) * 1000000UL;
    wait = interval/4;
    interval -= wait;

}
