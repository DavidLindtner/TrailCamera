#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<string>
#include<unistd.h>
#include<wiringPi.h>
#include<wiringPiI2C.h>

#define DETECT_PIN 0	//	logical pin 11 GPIO 17
#define SHUTDOWN_PIN 1	//	logical pin 12 GPIO 18
#define IR_LED_PIN 7	//	logical pin 7 GPIO 4

bool event = false;
bool night = false;
static unsigned long last_interrupt_time_detect = 0;
static unsigned long last_interrupt_time_shutdown = 0;

//	Return control to startup script
void InterruptShutdown(void)
{
	std::cout<<"BYE BYE"<<std::endl;
	exit(1);
}

void InterruptDetect(void)
{
	unsigned long interrupt_time = millis();
	if(interrupt_time - last_interrupt_time_detect > 50)
		event = true;
	last_interrupt_time_detect = interrupt_time;
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

void VideoCapture()
{
	std::string CurrTime[20];
	int videoCounter = 0;
	std::string picCommand = "sudo raspistill -md 2 -o /media/usb/" + CurrentTime() + ".jpg";

	std::cout<<"Taking photo ..."<<std::endl;
	system(picCommand.c_str());


	do{
		event = false;
		std::string vidCommand = "raspivid -o /home/pi/" + std::to_string(videoCounter) + ".h264 -md 5 -t 5000";
		CurrTime[videoCounter] = CurrentTime();
		videoCounter++;

		std::cout<<"Capturing video ..."<<std::endl;
		system(vidCommand.c_str());
	}while(event);


	for(int i = 0; i < videoCounter; i++)
	{
		std::string vidExportCommand = "sudo MP4Box -add /home/pi/" + std::to_string(i) + ".h264:fps=30 /media/usb/" + CurrTime[i] + ".mp4";
		std::string vidDelCommand = "rm /home/pi/" + std::to_string(i) + ".h264";

		std::cout<<"Converting video ..."<<std::endl;
		system(vidExportCommand.c_str());
		std::cout<<"Deleting video ..."<<std::endl;
		system(vidDelCommand.c_str());
	}

}

int main(void)
{
	std::cout<<"Program started"<<std::endl;

	wiringPiSetup();
	wiringPiISR(DETECT_PIN, INT_EDGE_RISING, &InterruptDetect);
	wiringPiISR(SHUTDOWN_PIN, INT_EDGE_RISING, &InterruptShutdown);
	pinMode(IR_LED_PIN,OUTPUT);

	int fd = wiringPiI2CSetup(0x10);

	int videoCounter = 0;

	while(true)
	{
		if(event)
		{
			event = false;
			videoCounter++;

			if(ReadNightI2C(fd))
			{
				digitalWrite(IR_LED_PIN,HIGH);
				VideoCapture();
				digitalWrite(IR_LED_PIN,LOW);
			}
			else
			{
				VideoCapture();
			}
		}
		usleep(100000);
	}

	return 0;
}

