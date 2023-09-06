#pragma once

#include <thread>
#include <mutex>

class Chopstick
{
public:
	Chopstick() {}
	~Chopstick() {}

	inline std::thread start()
	{
		mtx = new std::mutex;
		return std::thread([this] { loop(); }); // Googled
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
			return false;
	}

	inline void freeChopstick()
	{
		mtx->lock();
		isFree = true;
		mtx->unlock();
	}

private:
	bool isFree = true;
	std::mutex* mtx = nullptr;

	inline void loop()
	{
		while (!kill) {};
	}
};