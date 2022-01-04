#include "ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool(int threadQuantity, TaskQueue* task_queue)
{
	
	threads = (HANDLE*)malloc(sizeof(HANDLE) * threadQuantity);
	this->threadQuantity = threadQuantity;
	InitializeCriticalSection(&csection);
	for (int i = 0; i < threadQuantity; i++){
		threads[i] = CreateThread(NULL, 0, &ThreadPool::DoThreadWork, (LPVOID)task_queue, CREATE_SUSPENDED, NULL);
		std::cout << "Thread descriptors "<< threads[i] << std::endl;
	}
	EnterCriticalSection(&csection);
	for (int i = 0; i < threadQuantity; i++)
		ResumeThread(threads[i]);
	LeaveCriticalSection(&csection);
}

void ThreadPool::WaitAll()
{
	WaitForMultipleObjects(threadQuantity, threads, TRUE, 20000);
}

ThreadPool::~ThreadPool()
{
	WaitAll();
	DeleteCriticalSection(&csection);
}

DWORD WINAPI ThreadPool::DoThreadWork(LPVOID obj)
{
	TaskQueue* tasks = (TaskQueue*)obj;
	while (1)
	{
		Task* temp = tasks->Front();
		if (temp == NULL)
			ExitThread(0);
		temp->func(temp->params);
	}
	return 0;

}

