#!/bin/sh

RULES_DIR="/etc/udev/rules.d"
POLAR_RULES="99-polar.rules"
RELOAD_BIN="sudo udevadm control --reload-rules"

if [ ! -d "$RULES_DIR" ]; then
	echo "Udev rules.d directory doesn't exist! Modify RULES_DIR in this script to the correct Udev rules.d location"
	exit -1
fi

echo "Copying 99-polar.rules to the Udev rules directory"
sudo cp $POLAR_RULES $RULES_DIR

echo "Reloading Udev rules..."
$RELOAD_BIN

echo "Done!"
