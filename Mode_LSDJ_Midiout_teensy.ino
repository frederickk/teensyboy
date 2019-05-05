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



void modeLSDJMidioutSetup()
{
  digitalWrite(pinStatusLed, LOW);
  DDRF = B00000011; //Set analog in pins as inputs
  PORTF = B00000001;

  countGbClockTicks = 0;
  lastMidiData[0] = -1;
  lastMidiData[1] = -1;
  midiValueMode = false;
  blinkMaxCount = 60;
  modeLSDJMidiout();
}

void modeLSDJMidiout()
{
  while (1)
  {
    if (getIncommingSlaveByte())
    {
      if (incomingMidiByte > 0x6f)
      {
        switch (incomingMidiByte)
        {
        case 0x7F: //clock tick
          //Send the MIDI Clock Tick

          usbMIDI.sendRealTime(0xF8); //to use this USBMidi function you need to use the modified Teensy usbmidi lib i've made (see first file for the doc)
          break;
        case 0x7E: //seq stop
          //send MIDI transport stop message
          usbMIDI.sendRealTime(0xFC); //to use this USBMidi function you need to use the modified Teensy usbmidi lib i've made (see first file for the doc)
          stopAllNotes();
          break;
        case 0x7D: //seq start
          //send MIDI transport start message
          usbMIDI.sendRealTime(0xFA);
          break;
        default:
          midiData[0] = (incomingMidiByte - 0x70);
          midiValueMode = true;
          break;
        }
      }
      else if (midiValueMode == true)
      {
        midiValueMode = false;
        midioutDoAction(midiData[0], incomingMidiByte);
      }
    }
    else
    {
      setMode(); // Check if mode button was depressed
      updateBlinkLights();
    }
  }
}

void midioutDoAction(byte m, byte v)
{
  if (m < 4)
  {
    //note message
    if (v)
    {
      checkStopNote(m);
      playNote(m, v);
    }
    else if (midiOutLastNote[m] >= 0)
    {
      stopNote(m);
    }
  }
  else if (m < 8)
  {
    m -= 4;
    //cc message
    playCC(m, v);
  }
  else if (m < 0x0C)
  {
    m -= 8;
    playPC(m, v);
  }
  blinkLight(0x90 + m, v);
}

void checkStopNote(byte m)
{
  if ((midioutNoteTimer[m] + midioutNoteTimerThreshold) < millis())
  {
    stopNote(m);
  }
}

void stopNote(byte m)
{
  for (int x = 0; x < midioutNoteHoldCounter[m]; x++)
  {
    //Need testing ... Maybe channel +1
    usbMIDI.sendNoteOff(midioutNoteHold[m][x], 0x00, (memory[MEM_MIDIOUT_NOTE_CH + m]));
  }
  midiOutLastNote[m] = -1;
  midioutNoteHoldCounter[m] = 0;
}

void playNote(byte m, byte n)
{
  //midiData[0] = (0x90 + (memory[MEM_MIDIOUT_NOTE_CH+m]));
  //midiData[1] = n;
  //midiData[2] = 0x7F;
  //Serial.write(midiData,3);

  //Need testing ... Maybe channel +1
  usbMIDI.sendNoteOn(n, 0x7F, (memory[MEM_MIDIOUT_NOTE_CH + m]));
  midioutNoteHold[m][midioutNoteHoldCounter[m]] = n;
  midioutNoteHoldCounter[m]++;
  midioutNoteTimer[m] = millis();
  midiOutLastNote[m] = n;
}

void playCC(byte m, byte n)
{
  byte v = n;

  if (memory[MEM_MIDIOUT_CC_MODE + m])
  {
    if (memory[MEM_MIDIOUT_CC_SCALING + m])
    {
      v = (v & 0x0F) * 8;
      //if(v) v --;
    }
    n = (m * 7) + ((n >> 4) & 0x07);
    //midiData[0] = (0xB0 + (memory[MEM_MIDIOUT_CC_CH+m]));
    //midiData[1] = (memory[MEM_MIDIOUT_CC_NUMBERS+n]);
    //midiData[2] = v;
    //Serial.write(midiData,3);
    //Need testing ... Maybe channel +1

    usbMIDI.sendControlChange((memory[MEM_MIDIOUT_CC_NUMBERS + n]), v, (memory[MEM_MIDIOUT_CC_CH + m]));
  }
  else
  {
    if (memory[MEM_MIDIOUT_CC_SCALING + m])
    {
      float s;
      s = n;
      v = ((s / 0x6f) * 0x7f);
    }
    n = (m * 7);
    //midiData[0] = (0xB0 + (memory[MEM_MIDIOUT_CC_CH+m]));
    //midiData[1] = (memory[MEM_MIDIOUT_CC_NUMBERS+n]);
    //midiData[2] = v;
    //Serial.write(midiData,3);

    //Need testing ... Maybe channel +1

    usbMIDI.sendControlChange((memory[MEM_MIDIOUT_CC_NUMBERS + n]), v, (memory[MEM_MIDIOUT_CC_CH + m]));
  }
}

void playPC(byte m, byte n)
{
  //midiData[0] = (0xC0 + (memory[MEM_MIDIOUT_NOTE_CH+m]));
  //midiData[1] = n;
  //Serial.write(midiData,2);

  //Need testing ... Maybe channel +1

  usbMIDI.sendProgramChange(n, (memory[MEM_MIDIOUT_NOTE_CH + m]));
}

void stopAllNotes()
{
  for (int m = 0; m < 4; m++)
  {
    if (midiOutLastNote[m] >= 0)
    {
      stopNote(m);
    }
    //midiData[0] = (0xB0 + (memory[MEM_MIDIOUT_NOTE_CH+m]));
    //midiData[1] = 123;
    //midiData[2] = 0x7F;
    //Serial.write(midiData,3); //Send midi

    //Need testing ... Maybe channel +1

    usbMIDI.sendControlChange(123, 0x7F, (memory[MEM_MIDIOUT_NOTE_CH + m]));
  }
}

boolean getIncommingSlaveByte()
{
  delayMicroseconds(midioutBitDelay);
  PORTF = B00000000; //rightmost bit is clock line, 2nd bit is data to gb, 3rd is your mom
  delayMicroseconds(midioutBitDelay);
  PORTF = B00000001;
  delayMicroseconds(2);
  if ((PINF & B00010000))
  {
    incomingMidiByte = 0;
    for (countClockPause = 0; countClockPause != 7; countClockPause++)
    {
      PORTF = B00000000;
      delayMicroseconds(2);
      PORTF = B00000001;
      incomingMidiByte = (incomingMidiByte << 1) + ((PINF & B00010000) >> 4);
    }
    //serial.write ( incomingMidiByte);
    return true;
  }
  return false;
}
