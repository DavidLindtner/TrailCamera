#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<string>
#include<unistd.h>
#include<wiringPi.h>
#include<wiringPiI2C.h>

#define DETECT_PIN 0	//	logical pin 11 GPIO 17
#define IR_LED_PIN 7	//	logical pin 7 GPIO 4

bool event = false;
bool night = false;
static unsigned long last_interrupt_time = 0;

void InterruptDetect(void)
{
	unsigned long interrupt_time = millis();
	if(interrupt_time - last_interrupt_time > 50)
		event = true;
	last_interrupt_time = interrupt_time;
}

std::string CurrentTime(void)
{
	char buff [30];
	time_t t = time(NULL);
	struct tm curr_time = *localtime(&t);
	sprintf(buff, "%d_%d_%d_%d_%d_%d", curr_time.tm_year+1900, curr_time.tm_mon+1, curr_time.tm_mday, curr_time.tm_hour, curr_time.tm_min, curr_time.tm_sec);
	std::string retVal(buff);
	return (retVal);
}

bool ReadNightI2C(int fd)
{
	if(wiringPiI2CWrite(fd, 0x00) < 0)
	{
		std::cout<<"Error writing I2C"<<std::endl;
		return false;
	}

	int data = wiringPiI2CRead(fd);
	
	if(data < 0)
	{
		std::cout<<"Error reading I2C"<<std::endl;
		return false;
	}
	else if(data == 0)
		return false;
	else
		return true;
}

int main(void)
{
	std::cout<<"Program started"<<std::endl;

	wiringPiSetup();
	wiringPiISR(DETECT_PIN, INT_EDGE_RISING, &InterruptDetect);
	pinMode(IR_LED_PIN,OUTPUT);

	int fd = wiringPiI2CSetup(0x10);

	while(true)
	{
		if(event)
		{
			event = false;
			std::string picCommand = "sudo raspistill -md 2 -o /media/usb/" + CurrentTime() + ".jpg";
			std::cout<<picCommand<<std::endl;

			if(ReadNightI2C(fd))
			{
				digitalWrite(IR_LED_PIN,HIGH);
				system(picCommand.c_str());
				digitalWrite(IR_LED_PIN,LOW);
			}
			else
			{
				system(picCommand.c_str());
			}
		}
	}

	return 0;
}
