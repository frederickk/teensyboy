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



/**
 * NIB: usbMIDI function called when a midi realtime data is received
 * @param incomingMidiByte
 */
void RealTimeSystem(byte incomingMidiByte)
{
  switch (memory[MEM_MODE])
  {
  case 0:
    RealTimeLSDJSlave(incomingMidiByte);
    break;

  case 3:
    RealTimeNanoloop(incomingMidiByte);
    break;

  case 5:
    RealTimeLSDJMap(incomingMidiByte);
    break;

  default:
    break;
  }
}

/**
 * NIB: usbMIDI function called when a midi note ON is received
 * @param channel
 * @param note
 * @param velocity
 */
void NoteON(byte channel, byte note, byte velocity)
{
  switch (memory[MEM_MODE])
  {
  case 0:
    NoteONLSDJSlave(channel, note, velocity);
    break;

  case 2:
    NoteONLSDJKeyboard(channel, note, velocity);
    break;

  case 4:
    NoteONMgb(channel, note, velocity);
    break;

  case 5:
    NoteONLSDJMap(channel, note, velocity);
    break;

  default:
    break;
  }
}

/**
 * NIB: usbMIDI function called when a midi note OFF is received
 * @param channel
 * @param note
 * @param velocity
 */
void NoteOFF(byte channel, byte note, byte velocity)
{
  switch (memory[MEM_MODE])
  {
  case 4:
    NoteOFFMgb(channel, note, velocity);
    break;

  default:
    break;
  }
}

/**
 * NIB: usbMIDI function called when a midi Program Change is received
 * @param channel
 * @param program
 */
void mProgramChange(byte channel, byte program)
{
  switch (memory[MEM_MODE])
  {
  case 2:
    ProgchangeLSDJKeyboard(channel, program);
    break;

  case 4:
    ProgchangeMgb(channel, program);
    break;

  default:
    break;
  }
}

/**
 * NIB: usbMIDI function called when a Control Change  is received
 * @param channel
 * @param control
 * @param value
 */
void mControlChange(byte channel, byte control, byte value)
{
  switch (memory[MEM_MODE])
  {
  case 4:
    CCchangeMgb(channel, control, value);
    break;

  default:
    break;
  }
}

/**
 * NIB: usbMIDI function called when a Pitch Change is received
 * @param channel
 * @param pitch
 */
void mPitchChange(byte channel, uint16_t pitch)
{
  switch (memory[MEM_MODE])
  {
  case 4:
    PitchchangeMgb(channel, pitch);
    break;

  default:
    break;
  }
}

/**
 * NIB: debug led function to be deleted
 */
void testled()
{
  pinMode(pinLeds[5], OUTPUT);
  digitalWrite(pinLeds[5], HIGH);
  delay(50);
  digitalWrite(pinLeds[5], LOW);
}

/**
 * sendMidiClockSlaveFromLSDJ waits for 8 clock bits from LSDJ,
 * sends the transport start command if sequencer hasnt started yet,
 * sends the midi clock tick, and sends a note value that corrisponds to
 * LSDJ's row number on start (LSDJ only sends this once when it starts)
 */
void sendMidiClockSlaveFromLSDJ()
{
  if (!countGbClockTicks)
  { // If we hit 8 bits
    if (!sequencerStarted)
    { // If the sequencer hasnt started
      usbMIDI.sendNoteOn(readGbSerialIn, 127, memory[MEM_LSDJMASTER_MIDI_CH]);

      // send MIDI transport start message
      usbMIDI.sendRealTime(0xFA);
      sequencerStart(); // call the global sequencer start function
    }
    // Send the MIDI Clock Tick
    usbMIDI.sendRealTime(0xF8);
    countGbClockTicks = 0; // Reset the bit counter
    readGbSerialIn = 0x00; // Reset our serial read value
    updateVisualSync();
  }
  countGbClockTicks++; // Increment the bit counter
  if (countGbClockTicks == 8) {
    countGbClockTicks = 0;
  }
}