#pragma once

#include <mutex>
#include <thread>
#include <iostream>

#include "Sage.hpp"

using std::cout;
using std::cin;

class App
{
public:
	App(HANDLE console) : hConsole(console)
	{
		getSageValues();
	}
	~App() {}

	std::mutex mtxPrint;
	HANDLE hConsole;
	bool displaySageText = true;

	std::vector<Sage> sages;
	std::vector<std::thread> sagesThrds;
	std::vector<Chopstick> chopsticks;
	std::vector<std::thread> chopsticksThrds;
	unsigned long nbSages = 0;

	unsigned long sageThinkTimeMin = 2; // old default values
	unsigned long sageThinkTimeMax = 5;

	unsigned long sageEatingTotalTime = 5;
	unsigned long sageEatingTimeMin = 1;
	unsigned long sageEatingTimeMax = 5;

	std::thread startStatusPrint();

private:
	void getSageValues();

	void printSagesStatus();
};

inline std::thread App::startStatusPrint()
{
	return std::thread([this] { printSagesStatus(); }); // Googled
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

	cout << "Do you want to display every Sage actions ? y/n: ";
	char displayText;
	cin >> displayText;
	if (displayText == 'n')
		displaySageText = false;
	else
	{
		if (displayText != 'y')
			cout << "Wrong input, sages text will be showned by default.";
		displaySageText = true;
	}

	cout << std::endl;
}

inline void App::printSagesStatus()
{
	mtxPrint.lock();
	cout << "\n | T = Thinking, W = Waiting, E = Eating, F = Finished | \n";
	mtxPrint.unlock();
	while (true)
	{
		unsigned long hasFinished = 0;

		mtxPrint.lock();
		cout << '\n' << "Status = ( ";
		for (unsigned long id = 0; id < nbSages; id++) {
			if (sages[id].status == finished) hasFinished++;

			SetConsoleTextAttribute(hConsole, sages[id].id % 15 + 1); // changes text color (+1 to avoid black on black txt)
			cout << (char)sages[id].status << " ";
		}

		SetConsoleTextAttribute(hConsole, 7); // 7 is default txt
		cout << ")" << '\n' << std::endl;
		mtxPrint.unlock();

		if (hasFinished == nbSages) {
			for (unsigned long id = 0; id < nbSages; id++)
				chopsticks[id].kill = true;
			return;
		}
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}