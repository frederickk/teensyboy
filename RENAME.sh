#!/usr/bin/env bash

# New name for Midi Device
NAME="Teensyboy"
# New device ID for Midi Device
ID="0x0495"

# Old name for Midi Device
OLD_NAME="Teensy MIDI"
# Old device ID for Midi Device
OLD_ID="0x0485"

# Default location for Teensy files for MacOS
# TODO (frederickk): Add support for other platforms; Linux, Windows.
ARDUINO_MAC="/Applications/Arduino.app/Contents/Java/hardware/teensy/avr/cores/usb_midi"


# Flag for reverting
REVERT=false

while [ ! $# -eq 0 ]
do
	case "$1" in
		--revert | -r)
			REVERT=true
			;;
	esac
	shift
done


if [ -d "$ARDUINO_MAC" ] && [ $REVERT == false ]
then
  cd $ARDUINO_MAC

  # Make a copy of 'usb_private.h', just in case...
  if [ ! -f "./usb_private.bak" ]
  then
    cp usb_private.h usb_private.bak
    echo "📦  usb_private.h backed up!"
  else
    echo "📦  usb_private.h already backed up!"
  fi

  RANDNUM=$((1 + RANDOM % 10))

  # Change device name
  NAME="$NAME-$RANDNUM"
  sed -i '' "s/#define STR_PRODUCT             L\"$OLD_NAME\"/#define STR_PRODUCT             L\"$NAME\"/" usb_private.h
  echo "🎹  Midi device name changed to $NAME"

  # Change product ID
  sed -i '' "s/#define PRODUCT_ID              $OLD_ID/#define PRODUCT_ID              $ID/" usb_private.h
  echo "🎹  Midi device name changed to $ID"

elif [ -d "$ARDUINO_MAC" ] && [ $REVERT == true ]
then
  cd $ARDUINO_MAC

  if [ -f "./usb_private.bak" ]
  then
    rm usb_private.h
    mv usb_private.bak usb_private.h
    echo "👍  Everything is back to what it once was."
  else
    echo "👎  Hmm... no usb_private.bak, nothing changed."
  fi

else
  echo "😢 $ARDUINO_MAC doesn't exist"
fi
