#include "Sage.hpp"

std::thread Sage::start(bool& chopstickRight, std::mutex& mtxPrint, HANDLE& hConsole)
{
	this->chopstickRight = &chopstickRight;
	this->mtxPrint = &mtxPrint;
	this->hConsole = &hConsole;

	return std::thread([this] { behaviourUpdate(); }); // Googled
}

void Sage::behaviourUpdate()
{
	while (isThinking || isWaiting || isEating)
	{
		if (isThinking)
			bThinking();
		else if (isWaiting)
			bWaiting();
		else if (isEating)
			bEating();

		std::this_thread::sleep_for(sleepTime);
	}
}

void Sage::bThinking()
{
	if (timerThink.count() >= thinkTime) {
		isWaiting = true;
		isThinking = false;
		status = waiting;

		print(" has stopped thinking");
	}
	else
	{
		print(" is thinking for " + std::to_string(timerThink.count()) + "s");

		timerThink += sleepTime;
	}
}

void Sage::bWaiting()
{
	mtxPrint->lock();
	if (hasFreeChopstick && chopstickRight)
	{
		hasFreeChopstick = *chopstickRight = false;
		mtxPrint->unlock();

		print(" has got two chopsticks");

		isWaiting = false;
		isEating = true;
		status = eating;

		eatTime = (float)(rand() % sageEatingTimeMax + sageEatingTimeMin);
		if (eatTime + timerEatingTotal > (float)sageEatingTotalTime)
			eatTime = (float)sageEatingTotalTime - timerEatingTotal;
	}
	else
	{
		mtxPrint->unlock();
		print(" is waiting");
	}
}

void Sage::bEating()
{
	if (timerEating.count() >= eatTime)
	{
		isEating = false;

		hasFreeChopstick = *chopstickRight = true;

		timerEatingTotal += timerEating.count();

		if (timerEatingTotal >= sageEatingTotalTime)
		{
			isWaiting = false;
			status = finished;

			print(" has finished eating");
		}
		else
		{
			timerEating = std::chrono::duration<float>(0.f);

			isWaiting = true;
			status = waiting;

			print(" has stopped eating");
		}
	}
	else
	{
		print(" is eating for " + std::to_string(timerEating.count()) + "s");

		timerEating += sleepTime;
	}
}

void Sage::print(std::string txt)
{
	mtxPrint->lock();
	SetConsoleTextAttribute(*hConsole, id % 15 + 1); // Sage id == Sage color
	std::cout << "Sage " << id << txt << std::endl;
	SetConsoleTextAttribute(*hConsole, 7);
	mtxPrint->unlock();
}