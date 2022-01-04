#include <iostream>
#include <locale>
#include <windows.h>
#include <fstream>
#include <thread>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include "ThreadPool.h"
#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <iostream>
#include <string>


std::vector<std::string> data;
std::vector<std::vector<std::string>> separateData;
short threadQuantity;
short count;
bool GetInfoFromFile()
{
	std::string filename;
	std::cout << "Enter filename: ";
	std::cin >> filename;
	std::cout << "\n";
	std::cout << "reading file..";
	std::cout << "\n";
	std::ifstream inputStream(filename.c_str());
	if (!inputStream.is_open())
	{
		inputStream.close();
		return false;
	}

	while (!inputStream.eof())
	{

		std::string temp;
		getline(inputStream, temp, '\n');
		if (temp != "")
			data.push_back(temp);
	}
	inputStream.close();
	return true;
}

short GetThreadCapacity()
{
	short maxThreadQuantity = std::thread::hardware_concurrency();
	std::cout << "max number of thread: " << maxThreadQuantity << std::endl;
	std::cout << "enter threads count: ";
	std::cin >> threadQuantity;
	if ((threadQuantity > data.size()) || (threadQuantity > maxThreadQuantity) || (threadQuantity <= 0))
		return -1;
	std::cout << "enter threads count for pool: ";
	std::cin >> count;
	std::cout << "\n";
	if ((count > data.size()) || (count > maxThreadQuantity) || (count <= 0)||count> threadQuantity)
		return -1;
	return threadQuantity;
}


void SortPiece(void* arrPiece)
{
	std::vector<std::string>* obj = (std::vector<std::string>*)arrPiece;
	std::sort(obj->begin(), obj->end());
}

int GetMinValueIndex(std::vector<std::string> inData)
{
	std::string minValue;
	int index;
	for (int i = 0; i < inData.size(); i++)
		if (inData[i] != "")
		{
			minValue = inData[i];
			index = i;
			break;
		}

	for (int i = 0; i < inData.size(); i++)
		if (inData[i] != "")
			if (strcmp(minValue.c_str(), inData[i].c_str()) > 0)
				if (minValue > inData[i])
				{
					minValue = inData[i];
					index = i;
				}
	return index;
}
bool CorrectTempString(std::vector<std::string> inData)
{
	for (int i = 0; i < inData.size(); i++)
		if (inData[i] != "")
			return true;
	return false;
}

void JoinResult()
{
	data.clear();
	std::vector<int> counter;
	counter.reserve(separateData.size());
	for (int i = 0; i < separateData.size(); i++)
		counter.push_back(0);
	std::vector<std::string> buff;

	while (1)
	{
		buff.clear();
		for (int i = 0; i < separateData.size(); i++)
			if (separateData[i].size() > counter[i])
				buff.push_back(separateData[i][counter[i]]);
			else
				buff.push_back("");
		if (!CorrectTempString(buff))
			break;
		int index = GetMinValueIndex(buff);
		data.push_back(buff[index]);
		counter[index]++;
	}
}

void CreateTasks(std::vector<std::string> inData, short quantity, TaskQueue* result)
{
	bool isEven = inData.size() % quantity == 0;
	int stringCapacity = inData.size() / quantity;
	if (!isEven)
		stringCapacity = inData.size() / quantity + 1;
	int counter = 0;
	for (int i = 0; i < quantity; i++)
	{
		separateData[i].clear();
		int temp = 0;
		while (stringCapacity != temp)
		{
			if (counter == inData.size())
				break;
			separateData[i].push_back(inData[counter]);
			counter++;
			temp++;
		}
		Task* newTask = new Task(&SortPiece, (void*)&separateData[i]);
		result->Enqueue(newTask);
	}
}

int main()
{

	HANDLE pid = GetCurrentThread();

	std::cout <<pid << std::endl;
	setlocale(LC_ALL, "russian");
	if (!GetInfoFromFile())
	{
		std::cout << "File not found!" << std::endl;
		system("pause");
		return 1;
	}

	short threadQuantity = GetThreadCapacity();
	if (threadQuantity == -1)
	{
		std::cout << "Invalid thread count value!" << std::endl;
		system("pause");
		return 1;
	}
	separateData.resize(threadQuantity);
	TaskQueue* taskqueue = new TaskQueue();
	CreateTasks(data, threadQuantity, taskqueue);
	ThreadPool* threads = new ThreadPool(count, taskqueue);
	threads->WaitAll();
	delete(threads);
	delete(taskqueue);
	JoinResult();
	std::cout << "DONE\n" << std::endl;
	std::cout << "Enter output filename: ";

	std::string outputFile;
	std::cin >> outputFile;
	std::cout << std::endl;

	std::ofstream outputStream(outputFile);
	if (outputStream.good())
	{
		std::cout << "Writing data to file.." << std::endl;
		for (std::string str : data) {
			const char* cstr = str.c_str();
			outputStream.write(cstr, strlen(cstr));
			outputStream.put('\n');
		}
		outputStream.close();
		std::cout << "done" << std::endl;
	}
	else
	{
		std::cout << "Can't find output file" << std::endl;
	}

	data.clear();
	system("pause");
	return 0;
}
