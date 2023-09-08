#pragma once

#include <thread>
#include <mutex>

using std::thread;
using std::mutex;

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