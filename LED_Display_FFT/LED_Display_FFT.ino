/*
 * 
 * Control a LED matrix to create a FFT audio spectrum visualizer
 rows are the anodes
 cols are the cathodes

 */
#include <FFT.h>

#define FFT_N 16

// 2-dimensional array of row pin numbers:
const int row[2] = {
  2,3 };

// 2-dimensional array of column pin numbers:
const int col[2] = {
  4,5  };

// 2-dimensional array of pixels:
int pixels_bar[4];
int previous_bars[4];
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
      previous_bars[y];
    }
  // initialize the pixel matrix:
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      pixels[x][y] = LOW;
    }
  }
  pinMode(state_LED, OUTPUT);
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x41; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
}

void loop() {
  cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fft_input[i] = k; // put real data into even bins
      fft_input[i+1] = 0; // set odd bins to 0
    }
    
  // read input:
  //readSensors();
  fft_window();
  fft_reorder();
  fft_run();
  //fft_mag_lin();
  sei();
  read_fft(fft_input);
  // draw the screen:
  refreshScreen();
}

void read_fft(int fft[32]){
  int i = 0;
  int k = 1;
  for(i=3; i<=7*2; i+=2){
    fill_bars(k, map(fft[i],0,1023,-1,4));
    k++;
  }
}

void readSensors() {
  // turn off the last position:
  pixels_bar[0] = LOW;
  pixels_bar[1] = LOW;
  // read the sensors for X and Y values:
  x = map(analogRead(A0), 0, 1023, -1, 4);
  y = map(analogRead(A1), 0, 1023, -1, 4);
  // set the new pixel position high so that the LED will turn on
  // in the next screen refresh:
  pixels_bar[0] = x;
  pixels_bar[1] = y;
  fill_bars(1, pixels_bar[0]);
  fill_bars(2, pixels_bar[1]);
}

void fill_bars(int bar, int value){
  int start=0;
//  if(bar == 2)
//    start = 2;
  for (int x = bar-1; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      if (y <= value)
        pixels[y][x] = HIGH;
      else
        pixels[y][x] = LOW;
    }
  }
}

void refreshScreen() {
  int c = 0;
  while(c<7){
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
    c++;
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
