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



void modeLSDJSlaveSyncSetup()
{
  digitalWrite(pinStatusLed, LOW);
  DDRF = B00111111; //Set analog in pins as outputs
  PORTF = B00000001;
  blinkMaxCount = 1000;
  modeLSDJSlaveSync();
}

void modeLSDJSlaveSync()
{
  while (1)
  { //Loop forever
    usbMIDI.read();
    setMode(); //Check if the mode button was depressed
    updateStatusLight();
  }
}

//NIB: function when a realtime midi message is received and LSDJ slavemode is enabled

void RealTimeLSDJSlave(byte realtimebyte)
{
  switch (realtimebyte)
  {
  case 0xF8:                                                        //Case: Clock Message Recieved
    if ((sequencerStarted && midiSyncEffectsTime && !countSyncTime) //If the seq has started and our sync effect is on and at zero
        || (sequencerStarted && !midiSyncEffectsTime))
    { //or seq is started and there is no sync effects
      if (!countSyncPulse && midiDefaultStartOffset)
      { //if we received a note for start offset
        //sendByteToGameboy(midiDefaultStartOffset);              //send the offset ??????????? why disabled
      }

      sendClockTickToLSDJ(); //send the clock tick
      updateVisualSync();
    }
    if (midiSyncEffectsTime)
    {                                                 //If sync effects are turned on
      countSyncTime++;                                //increment our tick counter
      countSyncTime = countSyncTime % countSyncSteps; //and mod it by the number of steps we want for the effect
    }
    break;
  case 0xFA:          // Case: Transport Start Message
  case 0xFB:          // and Case: Transport Continue Message
    sequencerStart(); // Start the sequencer
    break;
  case 0xFC:         // Case: Transport Stop Message
    sequencerStop(); // Stop the sequencer
    break;
  default:
    break;
  }
}

//NIB: function when a Note ON is received and LSDJ slave mode is enabled
void NoteONLSDJSlave(byte channel, byte note, byte velocity)
{

  if ((channel - 1) == (memory[MEM_LSDJSLAVE_MIDI_CH]))
  { //if a midi note was received and its on the channel of the sync effects channel
    midiData[0] = note;
    if (velocity != 0)
    {
      getSlaveSyncEffect(midiData[0]); //then call our sync effects function
    }
  }
}

/*
  sendClockTickToLSDJ is a lovely loving simple function I wish they where all this short
  Technicallyly we are sending nothing but a 8bit clock pulse
*/
void sendClockTickToLSDJ()
{
  for (countLSDJTicks = 0; countLSDJTicks < 8; countLSDJTicks++)
  {
    PORTF = B00000000;
    PORTF = B00000001;
  }
}

/*
  getSlaveSyncEffect receives a note, and assigns the propper effect of that note
*/
void getSlaveSyncEffect(byte note)

{

  switch (note)
  {
  case 48: //C-3ish, Transport Start
    sequencerStart();
    break;
  case 49: //C#3 Transport Stop
    sequencerStop();
    break;
  case 50: //D-3 Turn off sync effects
    midiSyncEffectsTime = false;
    break;
  case 51: //D#3 Sync effect, 1/2 time
    midiSyncEffectsTime = true;
    countSyncTime = 0;
    countSyncSteps = 2;
    break;
  case 52: //E-3 Sync Effect, 1/4 time
    midiSyncEffectsTime = true;
    countSyncTime = 0;
    countSyncSteps = 4;
    break;
  case 53: //F-3 Sync Effect, 1/8 time
    midiSyncEffectsTime = true;
    countSyncTime = 0;
    countSyncSteps = 8;
    break;
  default: //All other notes will make LSDJ Start at the row number thats the same as the note number.
    midiDefaultStartOffset = midiData[0];
    break;
  }
}
