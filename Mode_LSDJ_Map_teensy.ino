/**
 * TeensyBoy
 * 1.2.3
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



boolean midiTickHit;
unsigned long midiTickTime;
unsigned long currentTime;
int midiTickWait = 4000; //1 ms

void modeLSDJMapSetup()
{
  digitalWrite(pinStatusLed, LOW);
  DDRF = B00111111; //Set analog in pins as outputs
  PORTF = B00000001;
  midiSyncByte = 0xFF;
  blinkMaxCount = 1000;
  modeLSDJMap();
}

void modeLSDJMap()
{
  while (1)
  { //Loop forever
    checkClockTick();
    usbMIDI.read();
    setMode(); //Check if the mode button was depressed
    updateStatusLight();
    checkClockTick();
    updateBlinkLights();
  }
}

//NIB: function when a Midi realtime message is received and LSDJ Map mode is enabled
void RealTimeLSDJMap(byte incomingMidiByte)
{
  switch (incomingMidiByte)
  {
  case 0xF8: //Case: Clock Message Recieved
    if (sequencerStarted)
    {
      midiTickHit = true;
      midiTickTime = micros();
    }
    midiSyncByte = 0xFF;
    break;
  case 0xFA:          // Case: Transport Start Message
  case 0xFB:          // and Case: Transport Continue Message
    sequencerStart(); // Start the sequencer
    midiTickHit = false;
    break;
  case 0xFC:         // Case: Transport Stop Message
    sequencerStop(); // Stop the sequencer
    midiTickHit = false;
    break;
  default:
    break;
  }
}

//NIB: function when a Note On is received and LSDJ map mode is enabled
void NoteONLSDJMap(byte channel, byte note, byte velocity)
{
  if ((channel - 1) == (memory[MEM_LIVEMAP_CH]))
  {
    midiData[0] = false; //and reset the captured note
    midiData[1] = note;
    if (velocity != 0)
    {
      sendByteToGameboy(midiData[1]);
      updateVisualSync();
      midiTickHit = false;
    }
  }
}

void checkClockTick()
{
  currentTime = micros();
  if (midiTickHit && currentTime > (midiTickTime + midiTickWait))
  {
    sendByteToGameboy(0xFF);
    updateVisualSync();
    midiTickHit = false;
  }
}
