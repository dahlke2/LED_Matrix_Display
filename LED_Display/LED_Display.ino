/*
This code controls a 2d array of LEDs
 
 rows are the anodes
 cols are the cathodes
 */


// row pin numbers
const int row[2] = {
  2,3 };

// column pin numbers
const int col[2] = {
  4,5  };

// array of pixels
int pixels_bar[2];
int pixels[4][4];

// cursor position:
int x = 2;
int y = 2;
const int state_LED = 6;

void setup() {
  // initialize the I/O pins as outputs
  // iterate over the pins:
  for (int thisPin = 0; thisPin < 4; thisPin++) {
    // initialize the output pins:
    pinMode(col[thisPin], OUTPUT); 
    pinMode(row[thisPin], OUTPUT);  
    // take the col pins (i.e. the cathodes) high to ensure that
    // the LEDS are off: 
    digitalWrite(col[thisPin], LOW);    
  }

  // initialize the pixel matrix:
    for (int y = 0; y < 4; y++) {
      pixels_bar[y] = LOW;
    }
  // initialize the pixel matrix:
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      pixels[x][y] = LOW;
    }
  }
  pinMode(state_LED, OUTPUT);
}

void loop() {
  // read input:
  readSensors();

  // draw the screen:
  refreshScreen();
}

void readSensors() {
  // turn off the last position:
  pixels_bar[0] = LOW;
  pixels_bar[1] = LOW;
  // read the sensors for X and Y values:
  x = map(analogRead(A1), 0, 1023, -1, 4);
  y = map(analogRead(A2), 0, 1023, -1, 4);
  // set the new pixel position high so that the LED will turn on
  // in the next screen refresh:
  pixels_bar[0] = x;
  pixels_bar[1] = y;
  fill_bars(1, pixels_bar[0]);
  fill_bars(2, pixels_bar[1]);
}

void fill_bars(int bar, int value){
  int start=0;
  if(bar == 2)
    start = 2;
  for (int x = start; x < start+2; x++) {
    for (int y = 0; y < 4; y++) {
      if (y <= value)
        pixels[y][x] = HIGH;
      else
        pixels[y][x] = LOW;
    }
  }
}

void refreshScreen() {
  // iterate over the rows (anodes):
  for (int thisRow = 0; thisRow < 4; thisRow++) {
    // iterate over the cols (cathodes):
    for (int thisCol = 0; thisCol < 4; thisCol++) {
      // get the state of the current pixel;
      int thisPixel = pixels[thisRow][thisCol];
      // the LED where they meet turns on:
      binary_out(thisRow, row);
      binary_out(thisCol, col);
      digitalWrite(state_LED, thisPixel);
      delayMicroseconds(500);
      }
    }
  }


void binary_out(int integer, const int pins[2])
{
  switch(integer){
    case 0:
    digitalWrite(pins[0], LOW);
    digitalWrite(pins[1], LOW);
    break;
    case 1:
    digitalWrite(pins[0], HIGH);
    digitalWrite(pins[1], LOW);
    break;
    case 2:
    digitalWrite(pins[0], LOW);
    digitalWrite(pins[1], HIGH);
    break;
    case 3:
    digitalWrite(pins[0], HIGH);
    digitalWrite(pins[1], HIGH);
    break;
  }
}
