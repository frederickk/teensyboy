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



boolean checkMemory()
{
  byte chk;
  for (int m = 0; m < 4; m++)
  {
    chk = EEPROM.read(MEM_CHECK + m);
    if (chk != defaultMemoryMap[MEM_CHECK + m])
    {
      return false;
    }
  }
  return true;
}

void initMemory(boolean reinit)
{
  if (!alwaysUseDefaultSettings)
  {
    if (reinit || !checkMemory())
    {
      for (int m = (MEM_MAX); m >= 0; m--)
      {
        EEPROM.write(m, defaultMemoryMap[m]);
      }
    }
    loadMemory();
  }
  else
  {
    for (int m = 0; m <= MEM_MAX; m++)
    {
      memory[m] = defaultMemoryMap[m];
    }
  }
  changeTasks();
}

void loadMemory()
{
  for (int m = (MEM_MAX); m >= 0; m--)
  {
    memory[m] = EEPROM.read(m);
  }
  changeTasks();
}

void saveMemory()
{
  for (int m = (MEM_MAX - 1); m >= 0; m--)
  {
    EEPROM.write(m, memory[m]);
  }
  changeTasks();
}

void changeTasks()
{
  midioutByteDelay = memory[MEM_MIDIOUT_BYTE_DELAY] * memory[MEM_MIDIOUT_BYTE_DELAY + 1];
  midioutBitDelay = memory[MEM_MIDIOUT_BIT_DELAY] * memory[MEM_MIDIOUT_BIT_DELAY + 1];
}
