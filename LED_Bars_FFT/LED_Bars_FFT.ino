
/*
  This code is for use with a Micrel 5450 LED driver
  The functions are setup for use with LED bar displays
 ---------
 
 Pin numbers:
 *Serial Data Pin 2
 *Clock Pin 3
 *Data Enable Pin 4
 
 */
#define LOG_OUT 1 // use the log output function
#include <FFT.h>
#include <LED_bars.h>

#define FFT_N 32
#define REORDER 0
// Serial pins
#ifndef DATA
#define DATA 2
#define CLK 3
#define ENABLE 4
#endif

Micrel output;
Effects effects;
PID integrate;

volatile Data data;
volatile Data PID[20];

ISR (TIMER1_OVF_vect){
//  digitalWrite(6, HIGH);
//  delay(200);
//  digitalWrite(6,LOW);
//  output.shiftOut(&data);
  output.shiftOut_mult(&data, 0x2);
  output.shiftOut(&data);
  output.Clear();
//  digitalWrite(6, HIGH);

}

void setup() {
  data.a = 0x0;
  data.b = 0x0;
  data.c = 0x0;
  data.d = 0x0;
  data.e = 0x0;
  data.f = 0x0;
  
  Serial.begin(9600);
 
 pinMode(6, OUTPUT);
 digitalWrite(6, LOW);
  TCCR1A = 0x0; //Timer 1 Counter Comopare A clear on compare match
  TCCR1B = 0x1; //Timer 1 1 prescaler
  TCCR1C = 0x0;
  TIMSK1 = 0x1; //Timer 1 overflow
  OCR1A = 0xff; //Timer 1 output compare register set to 0xff
//  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x01; // use adc1
  DIDR0 = 0x02; // turn off the digital input for adc1
//  analogReference(EXTERNAL);
  effects.horz_fill(&data, 50, &output);
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
    
  fft_window();
  fft_reorder();
  fft_run();
  fft_mag_log();
  sei();


  read_fft(fft_log_out, &data, PID);
  delay(10);
//Serial.println(fft_input[18]);
//  effects.scan(&data, 1, &output);
//  effects.ping_pong(&data, 100, &output);
//  effects.horz_fill(&data, 100, &output);
//  effects.scan_spread(&data, 100, &output);
//  effects.ping_pong_inv(&data, 200, &output);

}


void read_fft(uint8_t fft[], volatile Data *data, volatile Data *PID){
  int to = 255;
  int i = 0;
  int k = 1; int v; 
//  for(i=7; i<=11; i+=2){
//    v = map(fft[i],0,1023,-1,max_level);
    cli();
    integrate.shift(PID);
    PID[0].a = level(map(fft[2],0,to,-1,BAR_SEG-1)); //Map 8 bit value to ten LED bar
    PID[0].b = level(map(fft[7],0,to,-1,BAR_SEG-1));
    PID[0].c = level(map(fft[12],0,to,-1,BAR_SEG-1));
    PID[0].d = level(map(fft[17],0,to,-1,BAR_SEG-1));
    PID[0].e = level(map(fft[22],0,to,-1,BAR_SEG-1));
    PID[0].f = level(map(fft[27],0,to,-1,BAR_SEG-1));
    
    data->a = level(map(fft[2],0,to,-1,BAR_SEG-1)); //Map 8 bit value to ten LED bar
    data->b = level(map(fft[7],0,to,-1,BAR_SEG-1));
    data->c = level(map(fft[12],0,to,-1,BAR_SEG-1));
    data->d = level(map(fft[17],0,to,-1,BAR_SEG-1));
    data->e = level(map(fft[22],0,to,-1,BAR_SEG-1));
    data->f = level(map(fft[27],0,to,-1,BAR_SEG-1));
    sei();
//    Serial.println(fft[18]);
//    output.shiftOut(data);
//    output.shiftOut_mult(data, 0x2);
//    delay(500);
//    fill_bars(k, v);
//    k++;
//  }
}




