#pragma once
#include <mutex>
#include <condition_variable>
#include <deque>
#include <queue>

template <typename T> class Blocking_Queue {
	std::condition_variable _cvCanPop;
	std::mutex _sync;
	std::queue<T> _qu;
	bool _bShutdown = false;

public:
	void Enqueue(const T& item)
	{
		{
			std::unique_lock<std::mutex> lock(_sync);
			_qu.push(item);
		}
		_cvCanPop.notify_one();
	}

	void RequestShutdown() {
		{
			std::unique_lock<std::mutex> lock(_sync);
			_bShutdown = true;
		}
		_cvCanPop.notify_all();
	}

	bool Dequeue(T &item) {
		std::unique_lock<std::mutex> lock(_sync);
		for (;;) {
			if (_qu.empty()) {
				if (_bShutdown) {
					return false;
				}
			}
			else {
				break;
			}
			_cvCanPop.wait(lock);
		}
		item = std::move(_qu.front());
		_qu.pop();
		return true;
	}

	void Reset()
	{
		_bShutdown = false;
	}

	void Clear()
	{
		// one way may be to improve this is, we can:
		// in lock, swap tempQ with _qu where tempQ will be a new stack instance of queue
		// after swap outside lock, we can pop tempQ elements. 
		// this way we will occupy lock and critical section for very small amount of time.
		{
			std::unique_lock<std::mutex> lock(_sync);
			Reset();
			while (!_qu.empty())
			{
				_qu.pop();
			}
		}
		_cvCanPop.notify_all();
	}
};