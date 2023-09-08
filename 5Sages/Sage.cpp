#include "Sage.hpp"

thread Sage::start(Chopstick* chopstick, Chopstick* nextChopstick,
	mutex& mtxPrint, HANDLE& hConsole, bool showText, bool fullLogs)
{
	this->chopstick = chopstick;
	this->nextChopstick = nextChopstick;
	this->mtxPrint = &mtxPrint;
	this->hConsole = &hConsole;
	this->showText = showText;
	this->fullLogs = fullLogs;

	return thread([this] { behaviourUpdate(); });
}

void Sage::setThinkingTime(unsigned long thinkTimeMin, unsigned long thinkTimeMax)
{
	timerThink = milliseconds((rand() % (thinkTimeMin * 10) + (thinkTimeMax * 10)) * 100);
}

void Sage::setEatingVars(unsigned long eatingTotalT, unsigned long eatingTimeMin, unsigned long eatingTimeMax)
{
	this->eatingTotalTime = eatingTotalT;
	this->eatingTimeMin = eatingTimeMin;
	this->eatingTimeMax = eatingTimeMax;
}

void Sage::behaviourUpdate()
{
	if (!fullLogs)
		print(" is thinking");

	while (status != finished) // thread loop
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
	if (timerThink.count() <= 0.01) {
		isWaiting = true;
		isThinking = false;
		status = waiting;

		print(" has stopped thinking");
	}
	else
	{
		if (fullLogs)
			print(" is thinking for " + std::to_string(timerThink.count()).erase(3) + "s");

		timerThink -= sleepTime;
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

			eatTime = (double)(rand() % (eatingTimeMax * 10) + (eatingTimeMin * 10)) / 10.0;
			if (eatTime + timerEatingTotal > (double)eatingTotalTime)
				eatTime = (double)eatingTotalTime - timerEatingTotal;
		}
		else
		{
			if (fullLogs)
				print(" is waiting (next sage doesnt have their chopstick free)");
			chopstick->freeChopstick();
		}
	}
	else
	{
		if (fullLogs)
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

		if (timerEatingTotal >= eatingTotalTime)
		{
			isWaiting = false;
			status = finished;

			print(" has finished eating");
		}
		else
		{
			timerEating = duration<double>(0.0);

			isWaiting = true;
			status = waiting;

			print(" has stopped eating");
		}
	}
	else
	{
		if (fullLogs)
			print(" is eating for " + std::to_string(eatTime - timerEating.count()).erase(3) + "s");

		timerEating += sleepTime;
	}
}

void Sage::print(std::string txt)
{
	if (showText)
	{
		if (fullLogs)
			mtxPrint->lock();
		else if (!mtxPrint->try_lock()) return;

		SetConsoleTextAttribute(*hConsole, color);
		std::cout << "Sage " << std::setw(4) << id;
		SetConsoleTextAttribute(*hConsole, 7);
		std::cout << txt << std::endl;
		mtxPrint->unlock();
	}
}