#pragma once

#include <vector>

#include "Sage.hpp"

using std::vector;

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

	void getSageValues();
	void startThreads();
	void thrdWatcher();

	void printSagesStatus();

	bool firstDisplay = true;
	bool displayOneLine = false;
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
	cout << "Enter nb of sages: ";
	cin >> nbSages;

	sages.resize(nbSages);
	sagesThrds.resize(nbSages);

	chopsticks.resize(nbSages);
	chopsticksThrds.resize(nbSages);

	cout << "Enter sage minimum thinking time (sec): ";
	cin >> sageThinkTimeMin;
	cout << "Enter sage maximum thinking time (sec): ";
	cin >> sageThinkTimeMax;

	cout << "Enter sage TOTAL eating time (sec): ";
	cin >> sageEatingTotalTime;

	cout << "Enter sage minimum eating time (sec): ";
	cin >> sageEatingTimeMin;
	cout << "Enter sage maximum eating time (sec): ";
	cin >> sageEatingTimeMax;

	cout << "Do you want to display every Sage actions ? y/n: "; // TODO + detailed action
	char displayText;
	cin >> displayText;
	if (displayText == 'n' || displayText == '0')
	{
		displaySageText = false;
	}
	else
	{
		if (displayText != 'y' && displayText != '1')
			cout << "Wrong input, sages text will be showned by default.\n";
		displaySageText = true;
	}
	cout << "Do you want to the very fancy autorefresh display ? y/n: ";
	cin >> displayText;
	if (displayText == 'y' || displayText == '1')
		displayOneLine = true;

	cout << std::endl;
	system("cls");

	printSagesStatus();
	startThreads();
}

inline void App::startThreads()
{
	for (unsigned long id = 0; id < nbSages; id++) {
		chopsticksThrds[id] = chopsticks[id].start();

		sages[id].id = id + 1;
		sages[id].setEatingVars(sageEatingTotalTime, sageEatingTimeMin, sageEatingTimeMax);
		sages[id].setThinkingTime(sageThinkTimeMax, sageThinkTimeMin);

		sagesThrds[id] = sages[id].start(&chopsticks[id],
			&chopsticks[(id + 1) % nbSages], mtxPrint, hConsole, displaySageText);
	}

	thrdWatcher();
}

inline void App::thrdWatcher()
{
	while (!thrdsEnded) {
		unsigned long hasFinished = 0;
		for (unsigned long id = 0; id < nbSages; id++)
			if (sages[id].status == finished) hasFinished++;

		if (hasFinished == nbSages) {
			for (unsigned long id = 0; id < nbSages; id++)
			{
				sagesThrds[id].join();
				chopsticksThrds[id].join();
			}
			thrdsEnded = true;
			printSagesStatus();
			appEnded = true;
			return;
		}

		printSagesStatus();
		std::this_thread::sleep_for(milliseconds(500));
	}
}

inline void App::printSagesStatus()
{
	mtxPrint.lock();

	if (displayOneLine)
	{
		system("cls");
		cout << " | T = Thinking, W = Waiting, E = Eating, F = Finished |\n";
	}
	else if (firstDisplay)
	{
		cout << " | T = Thinking, W = Waiting, E = Eating, F = Finished |\n";
		firstDisplay = false;
	}

	cout << "\nStatus = ( ";
	for (unsigned long id = 0; id < nbSages; id++) {
		SetConsoleTextAttribute(hConsole, sages[id].id % 15 + 1); // changes text color (+1 to avoid black on black txt)
		cout << (char)sages[id].status << " ";
	}
	SetConsoleTextAttribute(hConsole, 7); // 7 is default txt
	cout << ")\n" << std::endl;

	mtxPrint.unlock();
}