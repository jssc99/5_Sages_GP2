#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>

using std::thread;
using std::cout;
using std::cin;
using std::endl;
using std::ref;

constexpr int nbSages = 5;

constexpr int sageEatingTimeMin = 1;
constexpr int sageEatingTimeMax = 5;

constexpr int sageThinkTimeMin = 2;
constexpr int sageThinkTimeMax = 5;

constexpr char thinking = 'T';
constexpr char waiting = 'W';
constexpr char eating = 'E';
constexpr char finished = 'F';

std::mutex mutex;

constexpr std::chrono::milliseconds sleepTime = std::chrono::milliseconds(100);

struct Sage
{
	int id = 0;

	bool isThinking = true;
	bool isWaiting = false;
	bool isEating = false;

	char status = thinking;
	bool hasFreeChopstick = true;

	std::chrono::duration<float> timerEating{ 0 };
	std::chrono::duration<float> timerThink{ 0 };
	float timerEatingTotal = 0.f;
	float thinkTime;
	float eatTime = 0.f;

	Sage()
	{
		thinkTime = (float)(rand() % sageThinkTimeMax + sageThinkTimeMin);
	}
};

void printThrdID(thread& t, int idThrd);
void printSagesStatus(Sage* sages, int nbSages);
void sageBehaviour(Sage& sage, bool& chopstickRight);
//void sageBehaviour(Sage& sage, std::mutex& chopstickRight);

int main()
{
	thread_local Sage sages[nbSages];
	std::vector<thread> sagesBehaviour(nbSages);

	for (int id = 0; id < nbSages; id++) {
		sages[id].id = id + 1;
		thread tmp{ sageBehaviour, ref(sages[id]), ref(sages[(id + 1) % nbSages].hasFreeChopstick) };

		sagesBehaviour[id] = std::move(tmp);
		//printThrdID(sagesBehaviour[id], id);
	}

	thread statusPrint{ printSagesStatus, ref(sages), nbSages };
	statusPrint.join();

	for (int id = 0; id < nbSages; id++)
		sagesBehaviour[id].join();

	return (EXIT_SUCCESS);
}

void printThrdID(thread& t, int idThrd)
{
	mutex.lock();
	if (t.get_id() == thread::id{}) // == id null
		cout << "Thread " << idThrd << " is not joinable" << endl;
	else
		cout << "Thread " << idThrd << " id is " << t.get_id() << endl;
	mutex.unlock();
}

void printSagesStatus(Sage* sages, int nbSages)
{
	while (true)
	{
		int hasFinished = 0;
		mutex.lock();
		cout << "Status =  ( ";
		for (int i = 0; i < nbSages; i++) {
			if (sages[i].status == finished) hasFinished++;
			cout << sages[i].status << " ";
		}
		cout << ")" << endl;
		mutex.unlock();
		if (hasFinished == nbSages) return;
		std::this_thread::sleep_for(sleepTime * 5);
	}
}

void sageBehaviour(Sage& sage, bool& chopstickRight)
//void sageBehaviour(Sage& sage, std::mutex& chopstickRight)
{
	while (sage.isThinking || sage.isWaiting || sage.isEating)
	{
		if (sage.isThinking)
		{
			if (sage.timerThink.count() >= sage.thinkTime) {
				sage.isWaiting = true;
				sage.isThinking = false;

				mutex.lock();
				sage.status = waiting;
				cout << "Sage " << sage.id << " has stopped thinking" << endl;
				mutex.unlock();
			}
			else
			{
				mutex.lock();
				cout << "Sage " << sage.id << " is thinking for " << sage.timerThink.count() << "s" << endl;
				mutex.unlock();

				sage.timerThink += sleepTime;
			}
		}

		else if (sage.isWaiting)
		{
			mutex.lock();
			if (sage.hasFreeChopstick && chopstickRight)
				//if (chopstick.try_lock() && chopstickRight.try_lock())
			{
				sage.hasFreeChopstick = chopstickRight = false;

				sage.status = eating;
				cout << "Sage " << sage.id << " has got two chopsticks" << endl;
				mutex.unlock();

				sage.isWaiting = false;
				sage.isEating = true;

				sage.eatTime = (float)(rand() % sageEatingTimeMax + sageEatingTimeMin);
				if (sage.eatTime + sage.timerEatingTotal > (float)sageEatingTimeMax)
					sage.eatTime = (float)sageEatingTimeMax - sage.timerEatingTotal;

				//chopstick.lock(); chopstickRight.lock();
			}
			else
			{
				cout << "Sage " << sage.id << " is waiting" << endl;
				mutex.unlock();

				//chopstick.unlock(); chopstickRight.unlock();
			}
		}

		else if (sage.isEating)
		{
			if (sage.timerEating.count() >= sage.eatTime)
			{
				sage.isEating = false;

				sage.hasFreeChopstick = chopstickRight = true;
				//chopstick.unlock(); chopstickRight.unlock();

				sage.timerEatingTotal += sage.timerEating.count();

				if (sage.timerEatingTotal >= sageEatingTimeMax)
				{
					sage.isWaiting = false;

					mutex.lock();
					sage.status = finished;
					cout << "Sage " << sage.id << " has finished eating" << endl;
					mutex.unlock();

				}
				else
				{
					sage.timerEating = std::chrono::duration<float>(0.f);

					sage.isWaiting = true;

					mutex.lock();
					sage.status = waiting;
					cout << "Sage " << sage.id << " has stopped eating" << endl;
					mutex.unlock();
				}
			}
			else
			{
				mutex.lock();
				cout << "Sage " << sage.id << " is eating for " << sage.timerEating.count() << "s" << endl;
				mutex.unlock();

				sage.timerEating += sleepTime;
			}
		}

		std::this_thread::sleep_for(sleepTime);
	}
}