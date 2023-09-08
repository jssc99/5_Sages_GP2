#pragma once

#include <vector>
#include <limits>

#include "Sage.hpp"

using std::vector;
using std::string;

class App
{
public:
	App(HANDLE console) : hConsole(console) {}
	~App() {}

	HANDLE hConsole;

	thread startApp();

	bool isAppEnded() const;

private:
	mutex mtxPrint;

	vector<Sage> sages;
	vector<thread> sagesThrds;
	vector<Chopstick> chopsticks;
	vector<thread> chopsticksThrds;
	unsigned long nbSages = 0;

	void setSageValues();
	bool getAnswer(string txt);
	unsigned long getNumber(string txt);

	void startThreads();
	void thrdWatcher();

	void printSagesStatus();

	bool firstDisplay = true;
	bool displayOneLine = false;

	bool thrdsEnded = false;
	bool appEnded = false;

	unsigned long sageThinkTimeMin = 0;
	unsigned long sageThinkTimeMax = 0;
};

inline thread App::startApp()
{
	return thread([this] { setSageValues(); });
}

inline bool App::isAppEnded() const
{
	return appEnded;
}

inline void App::setSageValues()
{
	nbSages = getNumber("Enter nb of sages: ");

	unsigned long sageRefreshSpeed = getNumber("Enter sage refresh rate (per seconds)(5 to 10 is best): ");

	sageThinkTimeMin = getNumber("Enter sage minimum thinking time (sec): ") * 1000;
	sageThinkTimeMax = getNumber("Enter sage maximum thinking time (sec): ") * 1000;      // sec to ms

	unsigned long sageEatingTotalTime = getNumber("Enter sage TOTAL eating time (sec): ");
	unsigned long sageEatingTimeMin = getNumber("Enter sage minimum eating time (sec): ") * 1000;
	unsigned long sageEatingTimeMax = getNumber("Enter sage maximum eating time (sec): ") * 1000;

	bool displaySageText, displayAllLogs = false;
	if (displaySageText = getAnswer("Do you want to display the Sages actions ? y/n: "))
		displayAllLogs = getAnswer("Do you want a complete and detailed log of actions ? y/n: ");
	displayOneLine = getAnswer("Do you want to the very fancy autorefresh display ? y/n: ");

	Sage baseSage;
	baseSage.setEatingVars(sageEatingTotalTime, sageEatingTimeMin, sageEatingTimeMax);
	baseSage.setDisplayOptions(displaySageText, displayAllLogs);
	baseSage.setPrintPointers(mtxPrint, hConsole);
	baseSage.sleepTime = milliseconds((long long)((1.0 / sageRefreshSpeed) * 1000));

	sages.resize(nbSages, baseSage);
	sagesThrds.resize(nbSages);

	chopsticks.resize(nbSages);
	chopsticksThrds.resize(nbSages);

	startThreads();
}

inline bool App::getAnswer(string txt)
{
	do {
		cout << txt;
		char answer;
		cin >> answer;
		cin.clear();
		cin.ignore(1000, '\n');

		if (answer == 'n' || answer == '0')
			return false;
		else if (answer == 'y' || answer == '1')
			return true;

		cout << "Wrong input, try again.\n";
	} while (true);
}

inline unsigned long App::getNumber(string txt)
{
	do {
		cout << txt;
		unsigned long answer;
		cin >> answer;
		cin.clear();
		cin.ignore(1000, '\n');

		if (!answer)
			cout << "Wrong input, try again.\n";
		else
			return answer;
	} while (true);
}

inline void App::startThreads()
{
	for (unsigned long id = 0; id < nbSages; id++) {
		chopsticksThrds[id] = chopsticks[id].start();

		sages[id].id = id + 1;
		sages[id].color = (id % 15) + 1;

		sages[id].setThinkingTime(sageThinkTimeMin, sageThinkTimeMax);

		sagesThrds[id] = sages[id].start(&chopsticks[id], &chopsticks[(id + 1) % nbSages]);
	}

	printSagesStatus();
	thrdWatcher();
}

inline void App::thrdWatcher()
{
	do {
		std::this_thread::sleep_for(milliseconds(500));
		unsigned long hasFinished = 0;
		for (unsigned long id = 0; id < nbSages; id++)
			if (sages[id].status == finished)
				hasFinished++;

		if (hasFinished == nbSages) {
			for (unsigned long id = 0; id < nbSages; id++)
			{
				sagesThrds[id].join();
				chopsticksThrds[id].join();
			}
			thrdsEnded = true;
		}

		printSagesStatus();
	} while (!thrdsEnded);
	appEnded = true;
}

inline void App::printSagesStatus()
{
	mtxPrint.lock();

	if (displayOneLine)
	{
		system("cls");
		cout << "\n\n | T = Thinking, W = Waiting, E = Eating, F = Finished |\n";
	}
	else if (firstDisplay)
	{
		cout << "\n\n | T = Thinking, W = Waiting, E = Eating, F = Finished |\n";
		firstDisplay = false;
	}

	cout << "\nStatus = | ";

	for (unsigned long id = 0; id < nbSages; id++)
	{
		SetConsoleTextAttribute(hConsole, sages[id].color); // changes text color
		cout << (char)sages[id].status << ' ';
	}

	SetConsoleTextAttribute(hConsole, 7); // 7 is default txt
	cout << "|\n" << std::endl;

	mtxPrint.unlock();
}