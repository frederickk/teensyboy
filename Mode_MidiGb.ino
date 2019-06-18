/**
 * TeensyBoy
 * 1.2.4
 * ArduinoBoy Firmware for Teensy 2.0
 *
 * noizeinabox@gmail.com
 * http://noizeinabox.blogspot.com/2012/12/teensyboy.htm
 *
 *
 * Original ArduinoBoy
 * Timothy Lamb (trash80@gmail.com)
 * https://github.com/trash80/Arduinoboy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */




void modeMidiGbSetup()
{
  digitalWrite(pinStatusLed, LOW);
  DDRF = B00111111; //Set analog in pins as outputs
  blinkMaxCount = 1000;
  modeMidiGb();
}

//NIB: Main function for the mGB mode
void modeMidiGb()
{
  //  boolean sendByte = false;
  while (1)
  { //Loop foreverrrr
    usbMIDI.read();
    setMode(); // Check if mode button was depressed
    updateBlinkLights();
  }
}

//NIB: function created by Trash80  but don't know when it is used.
boolean checkGbSerialStopped()
{
  countClockPause++; //Increment the counter
  if (countClockPause > 16000)
  {                      //if we've reached our waiting period
    countClockPause = 0; //reset our clock
                         //Serial.write(0xFC);                      //send the transport stop message
    usbMIDI.sendRealTime(0xFC);
    return true;
  }
  return false;
}

/*
 sendByteToGameboy does what it says. yay magic
 */
void sendByteToGameboy(byte send_byte)
{
  delayMicroseconds(500); // NIB: REV2.2 changed from 100 to 500 .delay needed otherwise this function is triggered toofast for the gameboy
  for (countLSDJTicks = 0; countLSDJTicks != 8; countLSDJTicks++)
  { //we are going to send 8 bits, so do a loop 8 times
    if (send_byte & 0x80)
    {
      PORTF = B00000010;
      PORTF = B00000011;
    }
    else
    {
      PORTF = B00000000;
      PORTF = B00000001;
    }
    send_byte <<= 1;
  }
}

//NIB: function when a note On is received and mgb mode is enabled
void NoteONMgb(byte channel, byte note, byte velocity)
{
  if (channel < 6)
  {
    midiData[0] = false;
    midiData[0] = 0x90 + (channel - 1);
    sendByteToGameboy(midiData[0]);
    midiData[1] = note;
    sendByteToGameboy(midiData[1]);
    midiData[2] = velocity;
    sendByteToGameboy(midiData[2]);
    blinkLight(midiData[0], midiData[2]);
  }
}

//NIB: function when a note Off is received and mgb mode is enabled
void NoteOFFMgb(byte channel, byte note, byte velocity)
{
  if (channel < 6)
  {
    midiData[0] = false;
    midiData[0] = 0x80 + (channel - 1);
    sendByteToGameboy(midiData[0]);
    midiData[1] = note;
    sendByteToGameboy(midiData[1]);
    midiData[2] = velocity;
    sendByteToGameboy(midiData[2]);
    blinkLight(midiData[0], midiData[2]);
  }
}

//NIB: function when a program change is received and mgb mode is enabled
void ProgchangeMgb(byte channel, byte program)
{
  if (channel < 6)
  {
    midiData[0] = false;
    midiData[0] = 0xC0 + (channel - 1);
    sendByteToGameboy(midiData[0]);
    midiData[1] = program;
    sendByteToGameboy(midiData[1]);
    midiData[2] = program;
    sendByteToGameboy(midiData[2]);
    blinkLight(midiData[0], midiData[2]);
  }
}

//NIB: function when a Control change is received and mgb mode is enabled
void CCchangeMgb(byte channel, byte control, byte value)
{
  if (channel < 6)
  {
    midiData[0] = false;
    midiData[0] = 0xB0 + (channel - 1);
    sendByteToGameboy(midiData[0]);
    midiData[1] = control;
    sendByteToGameboy(midiData[1]);
    midiData[2] = value;
    sendByteToGameboy(midiData[2]);
    blinkLight(midiData[0], midiData[2]);
  }
}

//NIB: function when a Pitch change is received and mgb mode is enabled
//void PitchchangeMgb(byte channel, uint16_t pitch){
void PitchchangeMgb(uint8_t channel, int pitch)
{
  if (channel < 6)
  {
    midiData[0] = false;
    midiData[0] = 0xE0 + (channel - 1);
    sendByteToGameboy(midiData[0]);
    midiData[1] = usbMIDI.getData1();
    sendByteToGameboy(midiData[1]);
    midiData[2] = usbMIDI.getData2();
    sendByteToGameboy(midiData[2]);
    blinkLight(midiData[0], midiData[2]);
  }
}
