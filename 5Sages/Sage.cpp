#include "Sage.hpp"

bool Sage::getChopstick()
{
	if (isChopstickFree&& mutex->try_lock()) {
		isChopstickFree = false;
		mutex->unlock();
		return true;
	}
	else
		return false;
}

void Sage::freeChopstick()
{
	mutex->lock();
	isChopstickFree = true;
	mutex->unlock();
}

std::thread Sage::start(Sage* sageRight, std::mutex& mutex, HANDLE& hConsole, bool showText)
{
	this->nextSage = sageRight;
	this->mutex = &mutex;
	this->hConsole = &hConsole;
	this->showText = showText;

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
	if (getChopstick())
	{
		if (nextSage->getChopstick()) {
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
			freeChopstick();
		}
	}
	else
	{
		print(" is waiting");
	}
}

void Sage::bEating()
{
	if (timerEating.count() >= eatTime)
	{
		isEating = false;

		freeChopstick();
		nextSage->freeChopstick();

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
	if (showText)
	{
		mutex->lock();
		SetConsoleTextAttribute(*hConsole, id % 15 + 1); // Sage id == Sage color
		std::cout << "Sage " << id;
		SetConsoleTextAttribute(*hConsole, 7);
		std::cout << txt << std::endl;
		mutex->unlock();
	}
}