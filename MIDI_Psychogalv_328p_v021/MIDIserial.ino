void setNote(int value, int velocity, long duration, int notechannel)
{
  //find available note in array (velocity = 0);
  for(int i=0;i<polyphony;i++){
    if(!noteArray[i].velocity){
      //if velocity is 0, replace note in array
      noteArray[i].type = 0;
      noteArray[i].value = value;
      noteArray[i].velocity = velocity;
      noteArray[i].duration = currentMillis + duration;
      noteArray[i].channel = notechannel;
     
      
        if(QY8) { midiSerial(144, notechannel, value, velocity); } 
        else { midiSerial(144, channel, value, velocity); }


      if(noteLEDs){ 
          for(byte j=0; j<(LED_NUM-1); j++) {   //find available LED and set
            if(!leds[j].is_fading()) { rampUp(i, 255, duration);  break; }
          }
      }

      break;
    }
  }
}

void setControl(int type, int value, int velocity, long duration)
{
  controlMessage.type = type;
  controlMessage.value = value;
  controlMessage.velocity = velocity;
  controlMessage.period = duration;
  controlMessage.duration = currentMillis + duration; //schedule for update cycle
}


void checkControl()
{
  //need to make this a smooth slide transition, using high precision 
  //distance is current minus goal
  signed int distance =  controlMessage.velocity - controlMessage.value; 
  //if still sliding
  if(distance != 0) {
    //check timing
    if(currentMillis>controlMessage.duration) { //and duration expired
        controlMessage.duration = currentMillis + controlMessage.period; //extend duration
        //update value
       if(distance > 0) { controlMessage.value += 1; } else { controlMessage.value -=1; }
       
       //send MIDI control message after ramp duration expires, on each increment
       midiSerial(176, channel, controlMessage.type, controlMessage.value); 
        
        //send out control voltage message on pin 17, PB3, digital 11
        if(controlVoltage) { if(distance > 0) { rampUp(controlLED, map(controlMessage.value, 0, 127, 0 , 255), 5); }
                                            else { rampDown(controlLED, map(controlMessage.value, 0, 127, 0 , 255), 5); }
        }
    }
  }
}

void checkNote()
{
  for (int i = 0;i<polyphony;i++) {
    if(noteArray[i].velocity) {
      if (noteArray[i].duration <= currentMillis) {
        //send noteOff for all notes with expired duration    
          if(QY8) { midiSerial(144, noteArray[i].channel, noteArray[i].value, 0); }
          else { midiSerial(144, channel, noteArray[i].value, 0); }
        noteArray[i].velocity = 0;
        rampDown(i, 0, 225);
      }
    }
  }

}

void MIDIpanic()
{
  //120 - all sound off
  //123 - All Notes off
 // midiSerial(21, panicChannel, 123, 0); //123 kill all notes
  
  //brute force all notes Off
  for(byte i=1;i<128;i++) {
    delay(1); //don't choke on note offs!
    midiSerial(144, channel, i, 0); //clear notes on main channel

       if(QY8){ //clear on all four channels
         for(byte k=1;k<5;k++) {
          delay(1); //don't choke on note offs!
          midiSerial(144, k, i, 0);
         }
       } 
  }
  
  
}

void midiSerial(int type, int channel, int data1, int data2) {

  cli(); //kill interrupts, probably unnessisary
    //  Note type = 144
    //  Control type = 176	
    // remove MSBs on data
		data1 &= 0x7F;  //number
		data2 &= 0x7F;  //velocity
		
		byte statusbyte = (type | ((channel-1) & 0x0F));
		
		Serial.write(statusbyte);
		Serial.write(data1);
		Serial.write(data2);
  sei(); //enable interrupts
}


