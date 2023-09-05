#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <string>
#include <windows.h>

#include "App.hpp"

int main()
{
	App* app = new App(GetStdHandle(STD_OUTPUT_HANDLE));

	for (unsigned long id = 0; id < app->nbSages; id++) {
		app->sages[id].id = id + 1;

		app->sages[id].thinkTime = (float)(rand() % app->sageThinkTimeMax + app->sageThinkTimeMin);

		app->sages[id].sageEatingTotalTime = app->sageEatingTotalTime;
		app->sages[id].sageEatingTimeMin = app->sageEatingTimeMin;
		app->sages[id].sageEatingTimeMax = app->sageEatingTimeMax;

		app->sagesThrds[id] = app->sages[id].start(
			&app->sages[(id + 1) % app->nbSages], 
			app->mutex, app->hConsole, app->displaySageText);
	}

	std::thread statusPrint = app->startStatusPrint();
	statusPrint.join();

	for (unsigned long id = 0; id < app->nbSages; id++)
		app->sagesThrds[id].join();

	SetConsoleTextAttribute(app->hConsole, 7);
	return (EXIT_SUCCESS);
}