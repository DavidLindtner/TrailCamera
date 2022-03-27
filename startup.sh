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
		sudo cp /home/pi/TrailCamera/startup.sh /etc/init.d
		sudo chmod +x startup.sh
		sudo update-rc.d startup.sh defaults
		echo "Startup script was succesfully installed"
		;;
	uninstall)
		echo "Uninstalling startup script ..."
		sudo rm /etc/init.d/startup.sh
		sudo update-rc.d startup.sh remove
		echo "Startup script was succesfully uninstalled"
		;;
	*)
		echo "Starting program"
		rfkill unblock wifi
		sleep 10
		ssidName=$(/sbin/iwgetid -r)
		if [ "$ssidName" = "D&L" ]; then
			echo "Connecting to wifi"
		else
			echo "No connection to wifi"
			echo 0 | sudo tee /sys/class/leds/led0/brightness
			/usr/bin/tvservice -o
			rfkill block wifi
			rfkill block bluetooth
			/home/pi/TrailCamera/./Tcamera.out
			echo 1 | sudo tee /sys/class/leds/led0/brightness
			rfkill unblock wifi
			sudo halt
		fi

		;;
esac

exit 0
