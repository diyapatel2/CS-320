#ifndef CACHE_H
#define CACHE_H
#include <vector>
#include <iostream>
using namespace std;

#define HIT 1 
#define MISS 0

class Cache{
	private: 

		vector<pair <int, char> > data;
	public:
		Cache(vector<pair<int,char>>);
		int directMapped(int);
		int SetAssociative(int);
		int FullyAssociativeLRU();
		int noAllocWriteMiss(int ways);
		int Set_Prefetching(int ways);
		int Prefetch_Miss(int ways);
		int getSize();
		int LRU(int ret, int index, int way, int val, int CACHE_LENGTH, int *block);
		int** arrr(int entries, int length);
		int** arr2(int entries, int len);
		void freearr(int **arr, int length);
		int FullyAssociativeHCU();

};
#endif
