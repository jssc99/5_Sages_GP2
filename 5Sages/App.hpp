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
	};
	~App() {};

	std::mutex mutex;
	HANDLE hConsole;
	bool displaySageText = true;

	std::vector<Sage> sages;
	std::vector<std::thread> sagesThrds;
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
	cout << " | T = Thinking, W = Waiting, E = Eating, F = Finished | \n" << std::endl;
	return std::thread([this] { printSagesStatus(); }); // Googled
}

inline void App::getSageValues()
{
	cout << "Enter nb of sages: ";
	cin >> nbSages;

	sages.resize(nbSages);
	sagesThrds.resize(nbSages);

	cout << "Enter sage minimum thinking time: ";
	cin >> sageThinkTimeMin;
	cout << "Enter sage maximum thinking time: ";
	cin >> sageThinkTimeMax;

	cout << "Enter sage TOTAL eating time: ";
	cin >> sageEatingTotalTime;

	cout << "Enter sage minimum eating time: ";
	cin >> sageEatingTimeMin;
	cout << "Enter sage maximum eating time: ";
	cin >> sageEatingTimeMax;

	cout << "Do you want to display every Sage actions ? y/n: ";
	char displayText;
	cin >> displayText;
	if (displayText == 'n')
		displaySageText = false;
	else
	{
		if (displayText != 'y')
			cout << "Wrong input, text will be showned.";
		displaySageText = true;
	}

	cout << std::endl;
}

inline void App::printSagesStatus()
{
	while (true)
	{
		unsigned long hasFinished = 0;

		mutex.lock();
		cout << '\n' << "Status =  ( ";
		for (unsigned long id = 0; id < nbSages; id++) {
			if (sages[id].status == finished) hasFinished++;

			SetConsoleTextAttribute(hConsole, sages[id].id % 15 + 1);
			cout << (char)sages[id].status << " ";
		}

		SetConsoleTextAttribute(hConsole, 7);
		cout << ")" << '\n' << std::endl;
		mutex.unlock();

		if (hasFinished == nbSages)
			return;
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}