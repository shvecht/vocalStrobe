# Vocal Strobe

Vocal Strobe is an Arduino sketch that drives a strobe light based on audio input. It performs a Fast Fourier Transform on microphone data to detect the dominant frequency and flashes an LED accordingly. The project can be used for visualizing vocal fold movement when paired with an endoscope.

## Hardware

- **Microphone input** on analog pin **A1**
- **Potentiometer** for manual adjustment on analog pin **A0**
- **Strobe LED** on digital pin **D5**
- Uses the built-in LED on pin **13** as an indicator
- Designed to run on Arduino Nano/Uno class boards

3D printable case files are provided in the `3D model` folder (`Top.STL`, `Bottom-left.STL`, `Bottom-right.STL`, and `3D Case.zip`). See `Strobe.pdf` for assembly guidance.

## Building the Sketch

1. Install the **arduinoFFT** library using the Arduino IDE Library Manager.
2. Open `strobe.ino` in the Arduino IDE.
3. Select the correct board and port.
4. Compile and upload the sketch.

Once running, the microphone input on A1 is sampled, the potentiometer on A0 adjusts the strobe timing, and the LED connected to D5 flashes in sync with the detected frequency.
