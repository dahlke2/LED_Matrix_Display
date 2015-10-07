
/*
 * 
 * Control a LED matrix to create a FFT audio spectrum visualizer, with a serial interface
 rows are the anodes
 cols are the cathodes
 
 */
#include <FFT.h>
#include <LED_bars.h>
#include <SPI.h>

#define FFT_N 16
// Serial pins
#define DATA 2
#define CLK 3
#define ENABLE 4

Micrel output;
Effects effects;

Data data; 

void setup() {
  data.a = 0x0;
  data.b = 0x0;
  data.c = 0x0;
 
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0xC1; // use adc1
  DIDR0 = 0x01; // turn off the digital input for adc1
  analogReference(INTERNAL);
  effects.scan(&data);
  SPI.begin();
  SPI.setBitOrder(LSBFIRST);
  SPI.setDataMode(SPI_MODE0);
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
  //fft_mag_log();
  sei();
//  for(int i=0;i<20;i++)
//  {
//    add(fft_input);
//    average();
//  }
//  read_fft(fft_input, &data);
//  delay(250);
//  effects.scan(&data);
//  effects.ping_pong(&data);
//  effects.horz_fill(&data);
//  effects.scan_spread(&data);
output.enable();
SPI.transfer(0xff);
SPI.transfer(0xff);
SPI.transfer(0xff);
SPI.transfer(0xff);
SPI.transfer(0x7f);
output.disable();
delay(1000);
}


void read_fft(int fft[32], Data *data){
  int i = 0;
  int k = 1; int v; 
//  for(i=7; i<=11; i+=2){
//    v = map(fft[i],0,1023,-1,max_level);
    data->a = level(map(fft[7],0,1023,-1,BAR_SEG));
    data->b = level(map(fft[9],0,1023,-1,BAR_SEG));
    data->c = level(map(fft[11],0,1023,-1,BAR_SEG));
    output.shiftOut(data);
    delay(750);
//    fill_bars(k, v);
//    k++;
//  }
}


