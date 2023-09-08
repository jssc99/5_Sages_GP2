#pragma once

#include <chrono>
#include <string>
#include <windows.h>
#include <iostream>
#include <iomanip>

#include "Chopstick.hpp"

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

	thread start(Chopstick* chopstick, Chopstick* nextChopstick,
		mutex& mtxPrint, HANDLE& hConsole, bool showText, bool fullLogs);

	void setThinkingTime(unsigned long thinkTimeMin, unsigned long thinkTimeMax);
	void setEatingVars(unsigned long eatingTotalT, unsigned long eatingTimeMin, unsigned long eatingTimeMax);

private:
	milliseconds sleepTime = milliseconds(100);
	HANDLE* hConsole = nullptr;
	mutex* mtxPrint = nullptr;

	Chopstick* chopstick = nullptr;
	Chopstick* nextChopstick = nullptr;

	bool showText = true;
	bool fullLogs = false;

	bool isChopstickFree = true;

	bool isThinking = true;
	bool isWaiting = false;
	bool isEating = false;

	duration<double> timerThink{ 0.0 };

	unsigned long eatingTotalTime = 0;
	unsigned long eatingTimeMin = 0;
	unsigned long eatingTimeMax = 0;

	duration<double> timerEating{ 0.0 };
	double timerEatingTotal = 0.0;
	double eatTime = 0.0;

	void behaviourUpdate();
	void bThinking();
	void bWaiting();
	void bEating();

	void print(std::string txt);
};