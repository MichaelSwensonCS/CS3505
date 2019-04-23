/*
 * Author: Michael Swenson
 * Class: CS3505
 * Date 1/11/19
 *
 * Assignment 1A: This class takes in total number of seconds, it can
 * be a decimal, and converts it into the form HH:MM:SS and assumes all
 * input will be valid to fit these requirements
*/

#include <iostream>
#include <stdio.h>

int main()
{
	int timeInSeconds;
	double userInput;

	//These could be output directly in a cout/printf but this is clearer
	int hours;
	int minutes;
	int seconds;

	printf("Enter a time in seconds: ");
	scanf("%lf", &userInput);

	//Suggested In class by Dr.Jensen
	timeInSeconds = (int)(userInput + .5);

	//Separated variables for read-ability
	hours = timeInSeconds / 3600;
	minutes = timeInSeconds % 3600 / 60;
	seconds = timeInSeconds % 3600 % 60;

	//Probably could be more readable in a printf(%d%d%d) format but
	//class intent felt like cout is what was expected
	//Enter a time in seconds: 1 seconds --> 00:00:01
	printf("%d seconds --> %02d:%02d:%02d\n", timeInSeconds, hours, minutes, seconds);
}