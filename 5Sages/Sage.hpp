#pragma once

#include <chrono>
#include <string>
#include <windows.h>
#include <iostream>
#include <thread>
#include <mutex>

enum Status
{
	thinking = 'T',
	waiting = 'W',
	eating = 'E',
	finished = 'F',
};
/*
struct Chopstick
{
	bool isFree = true;
	std::mutex mtx;

	bool getChopstick()
	{
		if (isFree && mtx.try_lock()) {
			isFree = false;
			mtx.unlock();
			return true;
		}
		else
			return false;
	}
	void freeChopstick()
	{
		mtx.lock();
		isFree = true;
		mtx.unlock();
	}
};
*/
class Sage
{
public:
	Sage() {};
	~Sage() {};

	unsigned long id = 0;
	Status status = thinking;

	std::chrono::duration<double> timerThink{ 0 };
	double thinkTime = 0.f;

	unsigned long sageEatingTotalTime = 0;
	unsigned long sageEatingTimeMin = 0;
	unsigned long sageEatingTimeMax = 0;

	std::chrono::duration<double> timerEating{ 0 };
	double timerEatingTotal = 0.f;
	double eatTime = 0.f;

	std::thread start(Sage* sageRight, std::mutex& mtxPrint, HANDLE& hConsole, bool showText = true);

	bool getChopstick();
	void freeChopstick();

private:
	std::chrono::milliseconds sleepTime = std::chrono::milliseconds(100);
	HANDLE* hConsole = nullptr;
	std::mutex* mutex = nullptr;
	Sage* nextSage = nullptr;

	bool showText = true;

	bool isChopstickFree = true;

	bool isThinking = true;
	bool isWaiting = false;
	bool isEating = false;

	void behaviourUpdate();
	void bThinking();
	void bWaiting();
	void bEating();

	void print(std::string txt);
};