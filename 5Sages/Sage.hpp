#pragma once

#include <chrono>
#include <string>
#include <windows.h>
#include <iostream>
#include <iomanip>
#include <random>

#include "Chopstick.hpp"

using std::cout;
using std::cin;

using std::chrono::milliseconds;
using std::chrono::duration;

enum Status
{
	thinking = 'T',
	waiting = 'W',
	eating = 'E',
	finished = 'F'
};

class Sage
{
public:
	Sage() {}
	~Sage() {}

	unsigned long id = 0;
	WORD color{};

	Status status = thinking;
	milliseconds sleepTime = milliseconds(100);

	thread start(Chopstick* chopstick, Chopstick* nextChopstick);

	void setThinkingTime(unsigned long thinkTimeMin, unsigned long thinkTimeMax);
	void setEatingVars(unsigned long eatingTotalT, unsigned long eatingTimeMin, unsigned long eatingTimeMax);
	void setDisplayOptions(bool showText, bool fullLogs);
	void setPrintPointers(mutex& mtxPrint, HANDLE& hConsole);

private:
	HANDLE* hConsole = nullptr;
	mutex* mtxPrint = nullptr;

	Chopstick* chopstick = nullptr;
	Chopstick* nextChopstick = nullptr;

	bool showText = true;
	bool fullLogs = false;

	duration<double> timerThink{ 0.0 };

	unsigned long eatingTotalTime = 0;	// how long the sage needs to eat in total
	unsigned long eatingTimeMin = 0;	// how long the sage can eat minimum in ms
	unsigned long eatingTimeMax = 0;	// how long the sage can eat maximum in ms

	duration<double> timerEating{ 0.0 }; // ongoing timer when sage is eating (timer starts at 0 and goes up)
	double eatingTotalTimer = 0.0;		 // how long the sage has already ate in total (all timers added)
	double eatTimeToDo = 0.0;			 // how long the sage needs to eat (this timer, random time)

	void behaviourUpdate();				 // sage behaviour loop
	void bThinking();
	void bWaiting();
	void bEating();

	void print(std::string txt);
};