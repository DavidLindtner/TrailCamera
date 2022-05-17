echo "Starting program"
echo "Turning off WiFi and BlueTooth"
echo 0 | sudo tee /sys/class/leds/led0/brightness
/usr/bin/tvservice -o
rfkill block wifi
rfkill block bluetooth
/home/pi/TrailCamera/./Tcamera.out
echo 1 | sudo tee /sys/class/leds/led0/brightness
rfkill unblock wifi
sudo halt

