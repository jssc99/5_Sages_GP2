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
		app->chopsticksThrds[id] = app->chopsticks[id].start();

		Sage* sage = &app->sages[id];
		sage->id = id + 1;
		sage->setEatingVars(app->sageEatingTotalTime, app->sageEatingTimeMin, app->sageEatingTimeMax);
		sage->setThinkingTime(app->sageThinkTimeMax, app->sageThinkTimeMin);

		app->sagesThrds[id] = app->sages[id].start(&app->chopsticks[id],
			&app->chopsticks[(id + 1) % app->nbSages], app->mtxPrint, app->hConsole, app->displaySageText);
	}

	app->startStatusPrint().join();

	for (unsigned long id = 0; id < app->nbSages; id++) {
		app->sagesThrds[id].join();
		app->chopsticksThrds[id].join();
	}

	SetConsoleTextAttribute(app->hConsole, 7);
	return (EXIT_SUCCESS);
}