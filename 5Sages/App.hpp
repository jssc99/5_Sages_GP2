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

	unsigned long sageThinkTimeMin = 2; // old default values
	unsigned long sageThinkTimeMax = 5;

	unsigned long sageEatingTotalTime = 5;
	unsigned long sageEatingTimeMin = 1;
	unsigned long sageEatingTimeMax = 5;

	unsigned long settingNumber(string txt);
	bool settingQuestion(string txt);
	void getSageValues();

	void startThreads();
	void thrdWatcher();

	void printSagesStatus();

	bool firstDisplay = true;
	bool displayOneLine = false;
	bool displayAllLogs = false;
	bool displaySageText = true;

	bool thrdsEnded = false;
	bool appEnded = false;
};

inline thread App::startApp()
{
	return thread([this] { getSageValues(); });
}

inline bool App::isAppEnded() const
{
	return appEnded;
}

inline void App::getSageValues()
{
	nbSages = settingNumber("Enter nb of sages: ");

	sages.resize(nbSages);
	sagesThrds.resize(nbSages);

	chopsticks.resize(nbSages);
	chopsticksThrds.resize(nbSages);

	sageThinkTimeMin = settingNumber("Enter sage minimum thinking time (sec): ");
	sageThinkTimeMax = settingNumber("Enter sage maximum thinking time (sec): ");

	sageEatingTotalTime = settingNumber("Enter sage TOTAL eating time (sec): ");
	sageEatingTimeMin = settingNumber("Enter sage minimum eating time (sec): ");
	sageEatingTimeMax = settingNumber("Enter sage maximum eating time (sec): ");

	displaySageText = settingQuestion("Do you want to display the Sages actions ? y/n: ");
	if (displaySageText)
		displayAllLogs = settingQuestion("Do you want a complete and detailed log of actions ? y/n: ");
	displayOneLine = settingQuestion("Do you want to the very fancy autorefresh display ? y/n: ");

	startThreads();
}

inline unsigned long App::settingNumber(string txt)
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

inline bool App::settingQuestion(string txt)
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
		else
			cout << "Wrong input, try again.\n";
	} while (true);
}

inline void App::startThreads()
{
	for (unsigned long id = 0; id < nbSages; id++) {
		chopsticksThrds[id] = chopsticks[id].start();

		sages[id].id = id + 1;
		sages[id].color = (id % 15) + 1;

		sages[id].setEatingVars(sageEatingTotalTime, sageEatingTimeMin, sageEatingTimeMax);
		sages[id].setThinkingTime(sageThinkTimeMax, sageThinkTimeMin);

		sagesThrds[id] = sages[id].start(&chopsticks[id], &chopsticks[(id + 1) % nbSages],
			mtxPrint, hConsole, displaySageText, displayAllLogs);
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
		cout << "\n | T = Thinking, W = Waiting, E = Eating, F = Finished |\n";
	}
	else if (firstDisplay)
	{
		cout << "\n | T = Thinking, W = Waiting, E = Eating, F = Finished |\n";
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