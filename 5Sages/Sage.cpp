#include "Sage.hpp"

thread Sage::start(Chopstick* chopstick, Chopstick* nextChopstick)
{
	this->chopstick = chopstick;
	this->nextChopstick = nextChopstick;

	return thread([this] { behaviourUpdate(); });
}

unsigned long random(unsigned long low, unsigned long high)
{
	return low + rand() % (high - low + 1);
}

void Sage::setThinkingTime(unsigned long thinkTimeMin, unsigned long thinkTimeMax)
{
	timerThink = milliseconds(random(thinkTimeMin, thinkTimeMax));
}

void Sage::setEatingVars(unsigned long eatingTotalT, unsigned long eatingTimeMin, unsigned long eatingTimeMax)
{
	this->eatingTotalTime = eatingTotalT;
	this->eatingTimeMin = eatingTimeMin;
	this->eatingTimeMax = eatingTimeMax;
}

void Sage::setDisplayOptions(bool showText, bool fullLogs)
{
	this->showText = showText;
	this->fullLogs = fullLogs;
}

void Sage::setPrintPointers(mutex& mtxPrint, HANDLE& hConsole)
{
	this->mtxPrint = &mtxPrint;
	this->hConsole = &hConsole;
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

			eatTimeToDo = milliseconds(random(eatingTimeMin, eatingTimeMax)).count() / 1000.0;
			if ((eatTimeToDo + eatingTotalTimer) > (double)eatingTotalTime)
				eatTimeToDo = ((double)eatingTimeMax - eatingTotalTimer) / 1000.0;
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
	if (timerEating.count() >= eatTimeToDo)
	{
		isEating = false;

		chopstick->freeChopstick();
		nextChopstick->freeChopstick();

		eatingTotalTimer += timerEating.count();

		if (eatingTotalTimer >= eatingTotalTime)
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
			print(" is eating for " + std::to_string(eatTimeToDo - timerEating.count()).erase(3) + "s");

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
		cout << "Sage " << std::setw(4) << id;
		SetConsoleTextAttribute(*hConsole, 7);
		cout << txt << std::endl;
		mtxPrint->unlock();
	}
}