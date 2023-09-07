#pragma once

#include <thread>
#include <mutex>

using std::thread;
using std::mutex;

using std::cout;
using std::cin;

using std::chrono::milliseconds;
using std::chrono::duration;

class Chopstick
{
public:
	Chopstick() {}
	~Chopstick() {}

	inline thread start()
	{
		mtx = new mutex;
		return thread([this] {});
	}

	bool kill = false;

	inline bool getChopstick()
	{
		if (isFree && mtx->try_lock()) {
			isFree = false;
			mtx->unlock();
			return true;
		}
		else
		{
			return false;
		}
	}

	inline void freeChopstick()
	{
		mtx->lock();
		isFree = true;
		mtx->unlock();
	}

private:
	bool isFree = true;
	mutex* mtx = nullptr;
};