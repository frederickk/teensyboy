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



void modeNanoloopSetup()
{
  digitalWrite(pinStatusLed, LOW);
  blinkMaxCount = 1000;
  modeNanoloopSync();
}

void modeNanoloopSync()
{
  while (1)
  { //Loop forever
    usbMIDI.read();
    setMode(); //Check if the mode button was depressed
    updateStatusLight();
  }
}

/**
 * NIB: function when a Realtime midi message is received and nanoloop mode is enabled
 */
void RealTimeNanoloop(byte realtimebyte)
{
  switch (realtimebyte)
  {
  case 0xF8: // Clock Message Recieved
             // Send a clock tick out if the sequencer is running
    if (sequencerStarted)
    {
      nanoSkipSync = !nanoSkipSync;
      if (countSyncTime)
      {
        nanoState = sendTickToNanoloop(nanoState, false);
      }
      else
      {
        nanoState = sendTickToNanoloop(true, true);
      }
      nanoState = sendTickToNanoloop(nanoState, nanoSkipSync);
      updateVisualSync();
      break;
    }
    break;
  case 0xFA: // Transport Start Message
  case 0xFB: // Transport Continue Message
    sequencerStart();
    break;
  case 0xFC: // Transport Stop Message
    sequencerStop();
    break;
  default:
    break;
  }
}

boolean sendTickToNanoloop(boolean state, boolean last_state)
{
  if (!state)
  {
    if (last_state)
    {
      PORTF = B00000010;
      PORTF = B00000011;
    }
    else
    {
      PORTF = B00000000;
      PORTF = B00000001;
    }
    return true;
  }
  else
  {
    PORTF = B00000010;
    PORTF = B00000011;
    return false;
  }
}
