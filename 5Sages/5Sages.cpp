#include "App.hpp"

int main()
{
	SetConsoleTitleA("5 Sages");
	char quit = 'r';
	do {
		system("cls");

		App* app = new App(GetStdHandle(STD_OUTPUT_HANDLE));

		std::thread appThrd = app->startApp();

		while (!app->isAppEnded()); // waiting for app to end

		appThrd.join();

		SetConsoleTextAttribute(app->hConsole, 7); // just in case

		cout << "\nType q to quit\nType r to restart\n";
		cin >> quit;
	} while (quit == 'r');

	return (EXIT_SUCCESS);
}