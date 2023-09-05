#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <string>
#include <windows.h>

#include "Sage.hpp"
#include "App.hpp"

using std::thread;
using std::cout;
using std::cin;
using std::endl;
using std::ref;

int main()
{
	App *app = new App(GetStdHandle(STD_OUTPUT_HANDLE));

	for (unsigned int id = 0; id < app->nbSages; id++) {
		app->sages[id].id = id + 1;

		app->sages[id].thinkTime = (float)(rand() % app->sageThinkTimeMax + app->sageThinkTimeMin);

		app->sages[id].sageEatingTotalTime = app->sageEatingTotalTime;
		app->sages[id].sageEatingTimeMin = app->sageEatingTimeMin;
		app->sages[id].sageEatingTimeMax = app->sageEatingTimeMax;

		thread tmp = app->sages[id].start(
			app->sages[(id + 1) % app->nbSages].hasFreeChopstick, app->mtxPrint, app->hConsole);

		app->sagesThrds[id] = std::move(tmp);
	}

	thread statusPrint = app->startStatusPrint();
	statusPrint.join();

	for (unsigned int id = 0; id < app->nbSages; id++)
		app->sagesThrds[id].join();

	SetConsoleTextAttribute(app->hConsole, 7);
	return (EXIT_SUCCESS);
}