# TrailCamera

## Small Program for Raspberry zero

### Compiling

Install gcc:

$ sudo apt-get install g++

Install wiringpi:

$ sudo apt-get install wiringpi

Install Mp4Box to convert video to MP4

$ sudo apt install -y gpac

Compile source code:

g++ TrailCamera.cpp -o Tcamera.out -lwiringPi
