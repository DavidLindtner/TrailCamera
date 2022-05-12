#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<string>
#include<unistd.h>
#include<wiringPi.h>
#include<wiringPiI2C.h>
#include<signal.h>
#include<fstream>

#define DETECT_PIN 0	//	logical pin 11 GPIO 17
#define SHUTDOWN_PIN 1	//	logical pin 12 GPIO 18
#define IR_LED_PIN 7	//	logical pin 7 GPIO 4
#define SHUTDOWN_VOLTAGE 9	// [V]
#define VIDEO_DURATION 60	// [s]

bool event = false;
bool night = false;
static unsigned long last_interrupt_time_detect = 0;
static unsigned long last_interrupt_time_shutdown = 0;

int fd;

double batteryVoltage = 0;
double lightVoltage = 0;

double batteryV [10];
double lightV [10];

bool dayLight = true;

char logName[30] = "/home/pi/TrailCamLog.log";

bool firstRead = true;


std::string CurrentTime(void)
{
	char buff [30];
	time_t t = time(NULL);
	struct tm curr_time = *localtime(&t);
	sprintf(buff, "%d_%02d_%02d_%02d_%02d_%02d", curr_time.tm_year+1900, curr_time.tm_mon+1, curr_time.tm_mday, curr_time.tm_hour, curr_time.tm_min, curr_time.tm_sec);
	std::string retVal(buff);
	return (retVal);
}

void LogData(std::string message, bool stdOut)
{
	std::ofstream file;
	file.open(logName, std::ios::app);
	file << "[" << CurrentTime() << "]" << ": " << message << std::endl;
	file.close();
	if(stdOut)
		std::cout << "[" << CurrentTime() << "]" << ": " << message << std::endl;
}

//	Return control to startup script
void InterruptShutdown(void)
{
	LogData("INTERRUPT SHUTDOWN", true);
	LogData(" __        __ ",true);
	LogData("|   |\\  | |  \\",true);
	LogData("|-- | \\ | |  |",true);
	LogData("|__ |  \\| |__/",true);
	exit(1);
}

void InterruptDetect(void)
{
	if(!digitalRead(DETECT_PIN))
	{
		LogData("Detection",true);
		unsigned long interrupt_time = millis();
		if(interrupt_time - last_interrupt_time_detect > 500)
			event = true;
		last_interrupt_time_detect = interrupt_time;
	}
}

double Median(double arr[10])
{
	double tmp = 0;
	double tmpArr[10];
	for(int i = 0; i < 9; i++)
	{
		for(int j = 0; j < 9-i; j++)
		{
			tmp = arr[j+1];
			tmpArr[j+1] = arr[j];
			tmpArr[j] = tmp;
		}
	}
	return (tmpArr[4] + tmpArr[5])/2;
}

void readI2C(int s)
{
	int data = 0;
	if(wiringPiI2CWrite(fd, 0x00) < 0)
		LogData("Error writing I2C", true);
	data = wiringPiI2CRead(fd);
	if(data < 0)
		LogData("Error reading I2C", true);
	else
	{
		for(int i = 9; i > 0; i--)
			batteryV[i] = batteryV[i-1];
		batteryV[0] = double(data)*0.0728;
	}

	if(wiringPiI2CWrite(fd, 0x01) < 0)
		LogData("Error writing I2C", true);
	data = wiringPiI2CRead(fd);
	if(data < 0)
		LogData("Error reading I2C", true);
	else
	{
		for(int i = 9; i > 0; i--)
			lightV[i] = lightV[i-1];
		lightV[0] = double(data)*0.0132;
	}

	batteryVoltage = Median(batteryV);
	lightVoltage = Median(lightV);

	if(lightVoltage > 2)
		dayLight = false;
	else
		dayLight = true;

	if(batteryVoltage < SHUTDOWN_VOLTAGE)
	{
		LogData("LOW BATTERY - SHUTDOWN", true);
		LogData(" __        __ ",true);
		LogData("|   |\\  | |  \\",true);
		LogData("|-- | \\ | |  |",true);
		LogData("|__ |  \\| |__/",true);
		exit(1);
	}

	std::string batVoltStr = "Battery: " + std::to_string(batteryVoltage) + " V, Light " + std::to_string(lightVoltage) + " V, Day: " + std::to_string(dayLight);
	LogData(batVoltStr, true);
	
	if(firstRead)
		firstRead = false;

	alarm(60);
}




void VideoCapture()
{
	std::string CurrTime[50];
	int videoCounter = 0;
	std::string picCommand = "sudo raspistill -md 2 -o /home/pi/" + CurrentTime() + ".jpg";

	//LogData("Taking photo", true);
	//system(picCommand.c_str());


	do{
		event = false;
		std::string vidCommand = "raspivid -o /home/pi/" + std::to_string(videoCounter) + ".h264 -md 2 -t " + std::to_string(VIDEO_DURATION * 1000);
		CurrTime[videoCounter] = CurrentTime();
		videoCounter++;
		if(dayLight)
			LogData("Capturing video during day ...", true);
		else
			LogData("Capturing video during night ...", true);

		system(vidCommand.c_str());
		if(!digitalRead(DETECT_PIN))
			event = true;
	}while(event);


	for(int i = 0; i < videoCounter; i++)
	{

		//std::string vidExportCommand = "sudo MP4Box -add /home/pi/" + std::to_string(i) + ".h264:fps=30 /mnt/usb/" + CurrTime[i] + ".mp4";
		std::string vidExportCommand = "MP4Box -add /home/pi/" + std::to_string(i) + ".h264:fps=15 /home/pi/" + CurrTime[i] + ".mp4";
		std::string vidDelCommand = "rm /home/pi/" + std::to_string(i) + ".h264";

		LogData("Converting video ...", true);
		system(vidExportCommand.c_str());
		LogData("Deleting video ...", true);
		system(vidDelCommand.c_str());
	}

}




int main(void)
{
	std::ifstream iFile;
	iFile.open(logName);
//	if(iFile)
//	{
//		iFile.close();
//		remove(logName);
//	}
	

	LogData("Program started", true);

	for(int i = 0; i < 10; i++)
	{
		batteryV[i] = 12;
		lightV[i] = 0.1;
	}

	wiringPiSetup();
	wiringPiISR(DETECT_PIN, INT_EDGE_FALLING, &InterruptDetect);
	wiringPiISR(SHUTDOWN_PIN, INT_EDGE_RISING, &InterruptShutdown);
	pinMode(IR_LED_PIN,OUTPUT);

	fd = wiringPiI2CSetup(0x37);

	signal(SIGALRM, readI2C);
	alarm(5);

	int videoCounter = 0;

	while(true)
	{
		if(event)
		{
			event = false;
			videoCounter++;

			if(dayLight)
			{
				VideoCapture();
			}
			else
			{
				digitalWrite(IR_LED_PIN,HIGH);
				VideoCapture();
				digitalWrite(IR_LED_PIN,LOW);
			}
		}
		usleep(1E5);
	}

	return 0;
}

