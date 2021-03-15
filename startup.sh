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
		#echo "Mounting USB"
		#sudo mount /dev/sda1 /mnt/usb
		#sudo chown -R pi:pi /media/usb
		#sudo /opt/vc/bin/tvservice -o
		#sudo /opt/vc/bin/tvservice -s
		echo "Starting program"
		rfkill unblock wifi
		sleep 20
		ssidName=$(/sbin/iwgetid -r)
		if [ "$ssidName" = "D&L" ]; then
			echo "Connecting to wifi"
		else
			rfkill block wifi
			echo "Disconecting"
		fi
		/home/pi/TrailCamera/./Tcamera.out
		echo "Unmounting USB"
		#sudo umount /mnt/usb
		rfkill unblock wifi
		sudo halt
		;;
esac

exit 0
