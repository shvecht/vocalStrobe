// pure 1-200hz strobe light, with fractionated HIGH, based on the requiered frequency

unsigned long microSeconds;
unsigned int samplingPeriod;

const int ledPin = 5; //the LED pin
int ledState = LOW; //start with the LED off

unsigned long previousMicros = 0; //previous time point
unsigned long currentMicros = 0; //current time point

long interval = 500000; //initial interval between blinks
long wait;
 
void setup() 
{
    pinMode(5, OUTPUT);
}
 
void loop() 
{
    int reading = analogRead(A0);
    float adjustment = map(reading, 0, 1024, 1, 200);

    for(int i=0; i<256; i++)
    {

        currentMicros = micros();
        if (currentMicros - previousMicros >= interval) {
          previousMicros = currentMicros;
     
         digitalWrite(ledPin, HIGH);
         delayMicroseconds(wait);
         digitalWrite(ledPin, LOW);
         
        }
    }
 
    interval = (1000000 / adjustment);
    wait = interval/6;
    interval -= wait;

}
