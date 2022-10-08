#include<Wire.h>

volatile byte b1, b2;
volatile int f0;

int interval = 800000;
int pause = 200000;

void setup() {
  Wire.begin(0x40);
  // Serial.begin(9600);
  Wire.onReceive(receiveEvent);
  DDRD |= _BV (4); // pinMode(4, OUTPUT);
  DDRB |= _BV (4); // pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    PORTD &= ~_BV (4); // digitalWrite(ledPin, LOW);
    delayMicroseconds(pause);
    PORTD |= _BV (4); // digitalWrite(ledPin, HIGH);
    delayMicroseconds(interval);
}

void receiveEvent(int howMany) {
  b1 = Wire.read();  //x1 hilds upper byte of received a
  b2 = Wire.read();  //x2 holds lower byte of received a
  f0 = (int)b1 << 8 | (int)b2;

  float reading = analogRead(A0);
  float adjustment = map(reading, 0, 1024, -7, 8);

  // Set the interval, and 'On' portion of the strobe
  // Serial.println(f0 + adjustment, DEC);
  interval = 1000000/(f0 + adjustment);
  pause = interval/5;
  interval -= pause;
}
