#pragma once

#include <thread>
#include <mutex>

class Chopstick
{
private:
	bool isFree = true;
	std::mutex* mtx = nullptr;

public:
	Chopstick() {};
	~Chopstick() {};

	bool kill = false;

	inline std::thread start()
	{
		mtx = new std::mutex;
		return std::thread([this] { loop(); }); // Googled
	};

	inline void loop()
	{
		while (!kill) {};
	}

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
};
