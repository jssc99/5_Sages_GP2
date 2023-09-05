#pragma once

#include <mutex>
#include <thread>
#include <iostream>

using std::cout;
using std::cin;

class App
{
public:
	App(HANDLE console) : hConsole(console) 
	{
		getSageValues();
	};

	std::mutex mtxPrint;
	HANDLE hConsole;

	std::vector<Sage> sages;
	std::vector<std::thread> sagesThrds;
	unsigned long nbSages = 0;

	unsigned long sageThinkTimeMin = 2; // old default values
	unsigned long sageEatingTotalTime = 5;
	unsigned long sageThinkTimeMax = 5;
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
	cout << "Enter nb of sages : ";
	cin >> nbSages;

	sages.resize(nbSages);
	sagesThrds.resize(nbSages);

	cout << "Enter sage min thinking time : ";
	cin >> sageThinkTimeMin;
	cout << "Enter sage max thinking time : ";
	cin >> sageThinkTimeMax;

	cout << "Enter sage TOTAL eating time : ";
	cin >> sageEatingTotalTime;

	cout << "Enter sage min eating time : ";
	cin >> sageEatingTimeMin;
	cout << "Enter sage max eating time : ";
	cin >> sageEatingTimeMax;

	cout << std::endl;
}

void App::printSagesStatus()
{
	while (true)
	{
		unsigned long hasFinished = 0;

		mtxPrint.lock();
		cout << '\n' << "Status =  ( ";
		for (unsigned long id = 0; id < nbSages; id++) {
			if (sages[id].status == finished) hasFinished++;

			SetConsoleTextAttribute(hConsole, sages[id].id % 15 + 1);
			cout << (char)sages[id].status << " ";
		}

		SetConsoleTextAttribute(hConsole, 7);
		cout << ")" << '\n' << std::endl;
		mtxPrint.unlock();

		if (hasFinished == nbSages) 
			return;
		else
			std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}