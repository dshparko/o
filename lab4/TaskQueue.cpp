#include "TaskQueue.h"

TaskQueue::TaskQueue()
{
	InitializeCriticalSectionAndSpinCount(&csection, SPINS);
	first = NULL;
	last = NULL;
}

Task* TaskQueue::Front()
{
	EnterCriticalSection(&csection);
	if (Empty())
	{
		LeaveCriticalSection(&csection);
		return NULL;
	}
	if (first == last)
	{
		Node* temp = first;
		first = NULL;
		last = NULL;
		LeaveCriticalSection(&csection);
		return temp->data;
	}
	Node* tempLast = last;
	last = last->prev;
	last->next = NULL;
	LeaveCriticalSection(&csection);
	return tempLast->data;
}

void TaskQueue::Enqueue(Task* inData)
{
	EnterCriticalSection(&csection);
	Node* temp = new Node();
	temp->data = inData;
	temp->next = NULL;
	temp->prev = NULL;

	if (Empty())
	{
		first = temp;
		last = temp;
	}
	else
	{
		Node* tempFirst = first;
		first = temp;
		first->next = tempFirst;
		tempFirst->prev = first;
	}
	LeaveCriticalSection(&csection);
}

void TaskQueue::Dequeue()
{

	Task* temp = Front();
	delete(temp);
}

bool TaskQueue::Empty()
{
	return ((first == NULL) && (last == NULL));
}

TaskQueue::~TaskQueue()
{
	while (!Empty())
		Dequeue();
	DeleteCriticalSection(&csection);
}
