#include "App.hpp"

int main()
{
	SetConsoleTitleA("5 Sages");

	std::string quit;
	do {
		system("cls");

		App* app = new App(GetStdHandle(STD_OUTPUT_HANDLE));

		std::thread appThrd = app->startApp();

		while (!app->isAppEnded()); // waiting for app to end

		appThrd.join();

		SetConsoleTextAttribute(app->hConsole, 7);

		cout << "\n\nType q to quit\nType r to restart\n";
		quit.clear();
		cin >> quit;
	} while (quit == "r" || quit == "restart");

	return (EXIT_SUCCESS);
}