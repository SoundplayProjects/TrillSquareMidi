/*
 ____  _____ _        _
| __ )| ____| |      / \
|  _ \|  _| | |     / _ \
| |_) | |___| |___ / ___ \
|____/|_____|_____/_/   \_\
http://bela.io

\example square-print

Trill Square Print
==================

This is an example of how to communicate with the Trill Square
sensor using the Trill Arduino library.

The sensor is set to Centroid mode and 2D touch location and size
printed to the serial port.

You can find our Processing library for visualising here:
https://github.com/BelaPlatform/trill-processing-library/
The accompanying Processing sketch, `TrillSquare.pde`, listens for
touch information on the Arduino serial port and displays it in a
render of a Trill Square.

You may need to update the Processing port number (gPortNumber)
to match that of your Arduino and set `verbose = false` below in order to
make the serial data parseable by the Processing sketch.
*/

#include <Trill.h>
#include <Bounce.h>

// the MIDI channel number to send messages
const int channel = 1;

Trill trillSensor;
boolean touchActive = false;
const boolean verbose = true; // set this to false to communicate to the Processing GUI via UART.

void setup() {
  // Initialise serial and touch sensor
  Serial.begin(115200);
  int ret = trillSensor.setup(Trill::TRILL_SQUARE);
  if(ret != 0) {
    Serial.println("failed to initialise trillSensor");
    Serial.print("Error code: ");
    Serial.println(ret);
  } else {
    if(verbose)
      Serial.println("Success initialising trillSensor");
  }
}

void loop() {
  // Read 20 times per second
  int oldCC = 0;
  int CCval1 = 0;
  int CCval2 = 0;
  int touch = 0;
  delay(50);
  trillSensor.read();

  if(trillSensor.getNumTouches() > 0 || trillSensor.getNumHorizontalTouches() > 0) {
    if(verbose)
      Serial.print("V[");
    Serial.print(trillSensor.getNumTouches());
    touch = (trillSensor.getNumTouches());
    if(verbose) {
      Serial.print("] ");
      Serial.print("H[");
    }
    else
      Serial.print(" ");
    Serial.print(trillSensor.getNumHorizontalTouches());
    if(verbose)
      Serial.print("] ");
    else
      Serial.print(" ");

    for(int i = 0; i < trillSensor.getNumTouches(); i++) {
        Serial.print(trillSensor.touchLocation(i));
        Serial.print(" ");
        CCval1 = ((trillSensor.touchLocation(i))/14);
        Serial.print(trillSensor.touchSize(i));
        Serial.print(" ");
    }
    for(int i = 0; i < trillSensor.getNumHorizontalTouches(); i++) {
        Serial.print(trillSensor.touchHorizontalLocation(i));
        Serial.print(" ");
        CCval2 = ((trillSensor.touchHorizontalLocation(i))/14);
        Serial.print(trillSensor.touchHorizontalSize(i));
        Serial.print(" ");
    }

    Serial.println("");
        // only transmit MIDI messages if analog input changed
if (CCval1 != oldCC)  {
      usbMIDI.sendControlChange(21, CCval2, channel);
      usbMIDI.sendControlChange(22, CCval1, channel);
      oldCC = CCval1;
}
//if(trillSensor.getNumTouches() < 1 || trillSensor.getNumHorizontalTouches() < 1)  {
//    usbMIDI.sendControlChange(21, 0, channel);
//  }
    touchActive = true;
  }
  else if(touchActive) {
    // Print a single line when touch goes off
    if(verbose)  {
      Serial.println("V[0] H[0]");
      usbMIDI.sendControlChange(21, 0, channel);  
    }
    else
      Serial.println("0 0");
    touchActive = false;
  }
}
