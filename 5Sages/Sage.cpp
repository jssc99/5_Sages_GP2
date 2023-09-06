#include "Sage.hpp"

std::thread Sage::start(Chopstick* chopstick, Chopstick* nextChopstick,
	std::mutex& mtxPrint, HANDLE& hConsole, bool showText)
{
	this->chopstick = chopstick;
	this->nextChopstick = nextChopstick;
	this->mtxPrint = &mtxPrint;
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
		print(" has been thinking for " + std::to_string(timerThink.count()) + "s");

		timerThink += sleepTime;
	}
}

void Sage::bWaiting()
{
	if (chopstick->getChopstick())
	{
		if (nextChopstick->getChopstick()) {
			print(" has got their two chopsticks");

			isWaiting = false;
			isEating = true;
			status = eating;

			eatTime = (float)(rand() % sageEatingTimeMax + sageEatingTimeMin);
			if (eatTime + timerEatingTotal > (float)sageEatingTotalTime)
				eatTime = (float)sageEatingTotalTime - timerEatingTotal;
		}
		else
		{
			print(" is waiting (next sage doesnt have their chopstick free)");
			chopstick->freeChopstick();
		}
	}
	else
	{
		print(" is waiting, they dont have their chopstick");
	}
}

void Sage::bEating()
{
	if (timerEating.count() >= eatTime)
	{
		isEating = false;

		chopstick->freeChopstick();
		nextChopstick->freeChopstick();

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
		print(" has been eating for " + std::to_string(timerEating.count()) + "s");

		timerEating += sleepTime;
	}
}

void Sage::print(std::string txt)
{
	if (showText)
	{
		mtxPrint->lock();
		SetConsoleTextAttribute(*hConsole, id % 15 + 1); // Sage id == Sage color
		std::cout << "Sage " << std::setw(3) << id;
		SetConsoleTextAttribute(*hConsole, 7);
		std::cout << txt << std::endl;
		mtxPrint->unlock();
	}
}