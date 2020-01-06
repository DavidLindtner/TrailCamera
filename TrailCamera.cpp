#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<string>
#include<unistd.h>

std::string CurrentTime(void)
{
	char buff [30];
	time_t t = time(NULL);
	struct tm curr_time = *localtime(&t);
	sprintf(buff, "%d_%d_%d_%d_%d_%d", curr_time.tm_year+1900, curr_time.tm_mon+1, curr_time.tm_mday, curr_time.tm_hour, curr_time.tm_min, curr_time.tm_sec);
	std::string retVal(buff);
	return (retVal);
}

int main(void)
{
	for(int i = 0; i < 10; i++)
	{
		std::string picCommand = "raspistill -md 2 -o /home/pi/" + CurrentTime() + ".jpg";
		std::cout<<picCommand<<std::endl;
		system(picCommand.c_str());
	}
	return 0;
}
