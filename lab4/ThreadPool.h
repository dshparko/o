#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "TaskQueue.h"

#define SPINS 4000

class ThreadPool
{
public:

	ThreadPool(int threadQuantity, TaskQueue* task_queue);
	void WaitAll();
	~ThreadPool();
private:
	HANDLE* threads;
	CRITICAL_SECTION csection;
	int threadQuantity;
	static DWORD WINAPI DoThreadWork(LPVOID obj);
};

#endif
