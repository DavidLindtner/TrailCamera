#!/bin/bash
# /etc/init.d/startup.sh
### BEGIN INIT INFO
# Provides: startup.sh
# Required-Start: $remote_fs $syslog $all
# Required-Stop:
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: Start daemon at boot time
# Description: Enable service provided by daemon.
### END INIT INFO

case "$1" in
	install)
		echo "Installing startup script ..."
		sudo cp /home/pi/Scripts/startup.sh /etc/init.d
		sudo update-rc.d startup.sh defaults
		;;
	*)
		echo "Mounting USB"
		sudo mount /dev/sda1 /media/usb
		#sudo chown -R pi:pi /media/usb
		echo "Starting program"
		/home/pi/TrailCamera/./Tcamera.out
		echo "Unmounting USB"
		sudo umount /media/usb
		;;
esac

exit 0
