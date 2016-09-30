
void checkKnob() {
  //float knobValue 
  threshold = analogRead(knobPin);  
  //set threshold to knobValue mapping
  threshold = mapfloat(threshold, knobMin, knobMax, threshMin, threshMax);
   
}

void knobMode() {
  //scroll through menus and select values using only a single knob
  //keep dreamin' kid,
}

void rampUp(int ledPin, int value, int time) {
LEDFader *led = &leds[ledPin];
// led->set_value(0);
  led->fade(value, time);  
}

void rampDown(int ledPin, int value, int time) {     
  LEDFader *led = &leds[ledPin];
 // led->set_value(255); //turn on
  led->fade(value, time); //fade out
}

void checkLED(){
//iterate through LED array and call update  
 for (byte i = 0; i < LED_NUM; i++) {
    LEDFader *led = &leds[i];
    led->update();    
 }
}

void checkButton() {
  //no button in this build...
}

long readVcc() {  //https://code.google.com/p/tinkerit/wiki/SecretVoltmeter
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

void checkBattery(){
  //check battery voltage against internal 1.1v reference
  //if below the minimum value, turn off the light show to save power
  //don't check on every loop, settle delay in readVcc() slows things down a bit
 if(batteryCheck < currentMillis){
  batteryCheck = currentMillis+10000; //reset for next battery check
   
  if(readVcc() < batteryLimit) {   //if voltage > valueV
    //battery failure  
    if(checkBat) { //first battery failure
      for(byte j=0;j<LED_NUM;j++) { leds[j].stop_fade(); leds[j].set_value(0); }  //reset leds, power savings
      noteLEDs = 0;  //shut off lightshow set at noteOn event, power savings
      checkBat = 0; //update, first battery failure identified
    } else { //not first low battery cycle
      //do nothing, lights off indicates low battery
      //MIDI continues to flow, MIDI data eventually garbles at very low voltages
      //some USB-MIDI interfaces may crash due to garbled data
    } 
  } 
 }
}

void bootLightshow(){
 //light show to be displayed on boot 
  for (byte i = 5; i > 0; i--) {
    LEDFader *led = &leds[i-1];
//    led->set_value(200); //set to max

    led->fade(200, 150); //fade up
    while(led->is_fading()) checkLED();
   

    led->fade(0,150+i*17);  //fade down
    while(led->is_fading()) checkLED();
   //move to next LED
  }
}


//provide float map function
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//debug SRAM memory size
int freeRAM() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
} // print free RAM at any point



