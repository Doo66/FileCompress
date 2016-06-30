#define _CRT_SECURE_NO_WARNINGS 1
#ifndef __TESTFILECOMPRESSION_H__
#define __TESTFILECOMPRESSION_H__
#include <iostream>
using namespace std;

#include <Windows.h>

#include "HaffmanTree.h"
#include "FileCompression.h"
#include "Heap.h"

void TestCompress()
{
	FileCompress fc;
	fc.Compress("Input");
}

void TestUnCompress()
{
	FileCompress fc;
	fc.UnCompress("Input");
}


void Test1()
{
	int arr[] = {1,3,5,6,7,9,8,4,2,0};
	Heap<int, Less<int> > hp(arr, 10);
}

int main()
{
	int begin = GetTickCount();
	TestCompress();
	int end = GetTickCount();
	cout<<"Compress Time: "<<end-begin<<endl;

	int begin2 = GetTickCount();
	TestUnCompress();
	int end2 = GetTickCount();
	cout<<"UnCompress Time: "<<end2-begin2<<endl;

	system("pause");
	return 0;
}

#endif