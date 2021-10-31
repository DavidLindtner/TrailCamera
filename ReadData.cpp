#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<string>
#include<unistd.h>
#include<wiringPi.h>
#include<wiringPiI2C.h>

#define DETECT_PIN 0	//	logical pin 11 GPIO 17
#define SHUTDOWN_PIN 1	//	logical pin 12 GPIO 18

double batteryVoltage = 0;
double lightVoltage = 0;
bool dayLight = true;
int fd;
int main(void)
{
	wiringPiSetup();

	fd = wiringPiI2CSetup(0x37);

	int data = 0;
	if(wiringPiI2CWrite(fd, 0x00) < 0)
		std::cout << "Error writing I2C" << std::endl;
	data = wiringPiI2CRead(fd);
	if(data < 0)
		std::cout << "Error reading I2C" << std::endl;
	else
		batteryVoltage = double(data)*0.0728;
	

	if(wiringPiI2CWrite(fd, 0x01) < 0)
		std::cout << "Error writing I2C" << std::endl;
	data = wiringPiI2CRead(fd);
	if(data < 0)
		std::cout << "Error reading I2C" << std::endl;
	else
		lightVoltage = double(data)*0.0132;

	if(lightVoltage > 2)
		dayLight = false;
	else
		dayLight = true;

	std::cout << "Detection: " << !digitalRead(DETECT_PIN) << std::endl;
	std::cout << "ShutDown Button pressed: " << digitalRead(SHUTDOWN_PIN) << std::endl;
	std::cout << "Battery: " << batteryVoltage << " [V]"  << std::endl;
	std::cout << "Light: " << lightVoltage << " [V]" << std::endl;
	std::cout << "Day: " << dayLight << std::endl;
	
	return 0;
}

