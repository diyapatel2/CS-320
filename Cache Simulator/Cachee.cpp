#include <iostream>
#include <vector>
#include <cmath>
#include <climits>
#include "Cache.h"
#include <time.h>
using namespace std;
#define BLOCK_SIZE 32
#define HIT 1
#define MISS 0
Cache::Cache(vector<pair <int,char> > ddata){
	 data = ddata;
}
int Cache::directMapped(int traces){
	vector<vector<int> > vec2D(traces,vector<int>(2,0));
	//2D vector initialized to zero
	int hits = 0;
	for(auto d: data){
		int offset = (int)(log2(traces)+ log2(BLOCK_SIZE));
		int my_address = floor(d.first /BLOCK_SIZE);
		int my_index = my_address % traces;
		int my_tag = my_address - (offset + my_index);
		if(vec2D[my_index][0] == HIT){	//if page is valid
			if(vec2D[my_index][1] == my_tag){	//tag matches the cache block
				hits++;				//then its a hit
			}
			else{
				vec2D[my_index][1] = my_tag;	//if page was a hit, but the tags dont match then set the new tag and set the valid bit (bring from mem)
				vec2D[my_index][0] = HIT;
			}
		}else{
			vec2D[my_index][1] = my_tag;	//if it was a miss then bring the data in
			vec2D[my_index][0] = HIT;

		}
	}
	return hits;
}
int** Cache::arrr(int entries, int length){
	//create a new array
	int **arr = new int*[entries];
	for(int i = 0; i < entries; i++){
		arr[i] = new int[length];
	}
	//zero out the double array
	for(int i = 0; i < entries; i++){
		for(int j = 0; j < length; j++){
			arr[i][j] = 0;
		}
	}
	return arr;
}
void Cache::freearr(int **arr, int entries){	//frees allocated array
	for(int i = 0; i < entries; i++){
		delete[] arr[i];
	}
	delete[] arr;
}
int Cache::SetAssociative(int ways){
	const int CACHE_ENTRIES = 512 / ways;
	const int CACHE_LENGTH = 3 * ways;
	int hits = 0;
	int time = 0; 
	int **arr;
	arr = arrr(CACHE_ENTRIES, CACHE_LENGTH);			//initializes the array 
	for (auto it = data.begin(); it != data.end(); it++){		//iterate through the data
		int offset = (int) (log2(BLOCK_SIZE));			//offset 
    		int my_address = (int) (it->first / BLOCK_SIZE);	//address is the instruction/block_size	
    		int my_index = my_address % CACHE_ENTRIES;		//index into the address 
    		int my_tag = my_address - (offset+my_index);		//tag is the total address length - (offset+index);	
    		int *block = arr[my_index];				//creates a block in the cache array
    		bool found = false;
    		for (int i = 0; i < CACHE_LENGTH; i += 3){		//block[i] -> VALID OR NOT, block[i+1] -> TAG, block[i+2] -> TIME
        		if (block[i] == HIT){				//if the block is valid
            			if (block[i + 1] == my_tag){		//if the tag matches what we are looking for
                			hits++;				//its a hit
                			block[i+2] = time++;		//increment time ->tells you when it was last accessed
                			found = true;			//the entry is found so don't look up in memory
                			break;
            			}
        		}
    		}		
    		if (!found){						//if the data was not found in the cache then add it to the cache
        		bool evict = false;
        		for (int i = 0; i < CACHE_LENGTH; i += 3){	//iterate through the cache again 
            			if (block[i] == MISS){			//if the block is not valid
                			block[i] = HIT;			//set it to a valid block
                			block[i + 1] = my_tag;		//give it a tag
                			block[i+2] = time++;		//update time accessed
                			evict = true;			
                			break;
            			}
        		}
			if (!evict){					//if something was inserted into the cache, then you need to evict the least recently used
            			int index = LRU(INT32_MAX, 0, 3, 2, CACHE_LENGTH, block);
            			block[index+1] = my_tag;
            			block[index+2] = time++;	//update the LRU block to the new tag and increment time
        		}
   
		}
	}
	freearr(arr, CACHE_ENTRIES);					//free memory on the heap
	return hits;	
}

int Cache::LRU(int ret, int index, int way, int val, int CACHE_LENGTH, int *block){
	for(int i = 0; i < CACHE_LENGTH; i+=way){
		if(block[i+val] < ret){
			ret = block[i+val];
			index = i;
		}
		
	}
	return index; 
}
int Cache::FullyAssociativeLRU(){
	return SetAssociative(512);
}
int Cache::FullyAssociativeHCU(){
	int hit = 0;
	int * cache = new int[512]; 					//create an array of size 512
	int * HCU = new int[512];					//initialize every element in array to 0 
	for(int i =0 ; i < 512; i++){
		cache[i] = 0;
		HCU[i] = 0;
	}
	for(auto it = data.begin(); it != data.end(); it++){
		int offset = (int) (log2(BLOCK_SIZE));		
		int my_tag = it->first >> offset;
		bool found = false;
		int index = 0;
		for(int i = 0; i < 512; i++){				//go through all entries and check for a tag match
			if(cache[i] == my_tag){
				hit++;
				index = i;
				found = true;
				break;
			}
		}
		if(found){						//if the entry was found update it in HCU
			int table_index = index + 511;			 
			while(table_index != 0){			
				if(table_index % 2 == 0){	
					table_index = (table_index -2)/2;
					HCU[table_index] = 0;
				}else{
					table_index = (table_index -1)/2;
					HCU[table_index] = 1;
				}
			}
		}else{							//if the entry was not found, bring it into the cache and update IN HCU
			int table_index = 0;				// table_index = 0 1 2 3 4 5 6 7 8 
			for(int j =0; j < 9; j++){
				if(HCU[table_index] == 0){		//if it is cold
					HCU[table_index] = 1;		// then set it to hot
					table_index = table_index *2 +1; //get the actual table_index 
				}
				else{
					HCU[table_index] = 0;		//if it is not cold, then make it cold
					table_index = table_index *2 +2;	//get the actual table_index 
				}

			}
			cache[table_index - 511] = my_tag;		//use the table_index to add it into the cache
		}
	}
	delete[] cache;
	delete[] HCU;
	return hit;
}
int Cache::noAllocWriteMiss(int ways){
	const int CACHE_ENTRIES = 512 / ways;
     	const int CACHE_LENGTH = 3 * ways;
     	int hits = 0;
     	int time = 0; 
     	int **arr;
     	arr = arrr(CACHE_ENTRIES, CACHE_LENGTH);
     	for (auto it = data.begin(); it != data.end(); it++){
     		int offset = (int) (log2(BLOCK_SIZE));			//offset 
    		int my_address = (int) (it->first / BLOCK_SIZE);	//address is the instruction/block_size	
    		int my_index = my_address % CACHE_ENTRIES;		//index into the address 
    		int my_tag = my_address - (offset+my_index);		//tag is the total address length - (offset+index);	
         	int *block = arr[my_index];
         	bool found = false;
         	for (int i = 0; i < CACHE_LENGTH; i += 3){
             		if (block[i] == HIT){				//It if a cache hit
                 		if (block[i + 1] == my_tag){		
                     			hits++;
                     			block[i+2] = time++;
                     			found = true;
                     			break;
                 		}
             		}
         	}
         	/*If it is a miss and the instruction is a STORE instruction then write it straight to memory*/
		if (!found){						//if entry was not found in the cache
             		bool evict = false;
             		if (it->second == 'S'){
             			evict = true;
             		}
             		for (int i = 0; ((i < CACHE_LENGTH) &&  (it->second != 'S')); i += 3){	//if the second instruction is not a STORE then write to cache
                 		if (block[i] == MISS){
                     			block[i] = HIT;
                     			block[i + 1] = my_tag;
                     			block[i+2] = time++;
                     			evict = true;
                     			break;
                 		}	
             		}
             		if (!evict){							//if the second instruction is was a STORE write it to mem
                		int index = LRU(INT32_MAX, 0, 3,2, CACHE_LENGTH, block);
                 		block[index + 1] = my_tag;
                 		block[index+2] = time++;
             		}	
         }
     }

     freearr(arr, CACHE_ENTRIES);
     return hits;

}
int** Cache::arr2(int entries, int length){
	//create a new array
	int **arr = new int*[entries];
	for (int i = 0; i < entries; i++){
        	arr[i] = new int[length];
    	}
	auto it = data.begin();
    	for (int i = 0; i < entries; i++){
        	for (int j = 0; j < length; j += 2){
            		arr[i][j] = it->first;
            		arr[i][j+1] = -1;
            		it++;
        }
    }
	return arr;
}
int Cache::Set_Prefetching(int ways){
	const int CACHE_ENTRIES = 512 / ways;
	const int CACHE_LENGTH = 2 * ways;
	int hits = 0;
	int time = 0; 
	int **arr;
	arr = arr2(CACHE_ENTRIES, CACHE_LENGTH);			//allocate array
    	for (auto it = data.begin(); it != data.end(); it++){
    		int offset = (int)(log2(CACHE_ENTRIES) + log2(BLOCK_SIZE));
        	int my_address = floor(it->first / BLOCK_SIZE);
        	int my_index = my_address % CACHE_ENTRIES;
        	int my_tag= it->first >> offset;
        	int my_index2 = (my_address + 1) % CACHE_ENTRIES;
        	int my_tag2 = (it->first + BLOCK_SIZE) >> offset;
        	int *block = arr[my_index];
        	int *block2 = arr[my_index2];
        	bool found = false;
        	bool found2 = false;
        	for (int i = 0; i < CACHE_LENGTH; i += 2){		//go through the entries to see a match in the cache for the first address
         		if (block[i] == my_tag){
                		hits++;
                		block[i + 1] = time++;
                		found = true;
                		break; 
            		}
        	}

        	for (int i = 0; i < CACHE_LENGTH; i += 2){		//also check the cache for the next address 
            		if (block2[i] == my_tag2){
                		block2[i + 1] = time++;
                		found2 = true;
                		break;
            		}
        	}
        	if (!found){						//if the first address was not found then use LRU to free and add it into the cache
			int index = LRU(block[1], 0, 2,1, CACHE_LENGTH, block);
            		block[index] = my_tag;
            		block[index + 1] = time++;
        	}
        	if (!found2){						//if the second address was not found then use LRU to free and add it into the cache
			int index = LRU(block2[1], 0, 2,1, CACHE_LENGTH, block2);
            		block2[index] = my_tag2;
           		block2[index + 1] = time++;
        	}
    	}

       	freearr(arr, CACHE_ENTRIES);
    	return hits;
}


int Cache::Prefetch_Miss(int ways){
	const int CACHE_ENTRIES = 512 / ways;
	const int CACHE_LENGTH = 2 * ways;
	int hits = 0;
	int time = 0; 
	int **arr;
	arr = arr2(CACHE_ENTRIES, CACHE_LENGTH);
	for (auto it = data.begin(); it != data.end(); it++){
		int offset = (int)(log2(CACHE_ENTRIES) + log2(BLOCK_SIZE));
		int my_address = floor(it->first / BLOCK_SIZE);
		int my_index = my_address % CACHE_ENTRIES;
		int my_tag = it->first >> offset;
		int my_index2 = (my_address +1) % CACHE_ENTRIES; 
		int my_tag2 = (it->first + BLOCK_SIZE) >> offset;
		int *block = arr[my_index];
		int *block2 = arr[my_index2];
		bool found = false;
		bool found2 = false;
		/*You would only prefetch if the first entry is a miss*/
       		for (int i = 0; i < CACHE_LENGTH; i += 2){		//check cache for hit
            		if (block[i] == my_tag){
                		hits++;
                		block[i + 1] = time++;
                		found = true;
                		break;
            		}
        	}
        	if (!found){						//if first entry was not found then prefetch second entry
            		for (int i = 0; i < CACHE_LENGTH; i += 2){	
                		if (block2[i] == my_tag2){
                    			block2[i + 1] = time++;
                    			found2 = true;
                    			break;
                		}
            		}
			int index = LRU(block[1], 0, 2, 1, CACHE_LENGTH, block);	//use LRU for replacement and update the cache with the tag and increment time
            		block[index] = my_tag;
            		block[index + 1] = time++;
        	}	
        	if (!found2 && !found){							//if second entry and first is not in cache then use LRU to replace and update cache with tag and time
        		int index2 = LRU(block2[1], 0, 2,1, CACHE_LENGTH, block2);	
            		block2[index2] = my_tag2;
            		block2[index2 + 1] = time++;
        	}
    	}
    	freearr(arr, CACHE_ENTRIES);
    	return hits;

}
int Cache::getSize(){
	return data.size();
}
