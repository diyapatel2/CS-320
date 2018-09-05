#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std; 
#include "Cache.h"


void runCache(Cache mycache, string filename);

int main(int argc, char *argv[]){
	vector<pair<int,char>> mydata; 
	if(argc != 3){
		cout << "Invalid input" << endl;
		return -1;
	}
	ifstream logfile(argv[1]);


	if(logfile.is_open()){
		string line;
		while(getline(logfile, line)){
			string myaddress = line.substr(2);
			char myflag = line.at(0);
			unsigned int hexAddress = (unsigned int) (stoul(myaddress, nullptr, 16));
			mydata.push_back(pair<int,char>(hexAddress,myflag));
	
			
		}
		logfile.close();		
	}
	else{
		cerr << "Error opening file" << argv[1] << endl;
		return -2; 
	}
	Cache mycache(mydata);
	runCache(mycache, argv[2]);
	return 0; 
}

void runCache(Cache mycache, string filename){
	int myTable[4] = {32, 128, 512, 1024};
	vector<int> mySets = {2,4,8,16};
	int ret;
	ofstream logfile(filename);
	if(logfile.is_open()){
		for(int i = 0; i < 4; i++){
			ret = mycache.directMapped(myTable[i]);
			logfile << ret << "," << mycache.getSize() << "; ";

		}
		logfile << "\n";

		for(int i = 0; i < 4; i++){
			ret = mycache.SetAssociative(mySets.at(i));
			logfile << ret << "," << mycache.getSize() << "; ";
		}
		logfile << "\n";

		ret = mycache.FullyAssociativeLRU();
		logfile << ret << "," << mycache.getSize() << "; ";
	
		logfile << "\n";
		
		ret = mycache.FullyAssociativeHCU();
		logfile << ret << "," << mycache.getSize() << "; ";
	
		logfile << "\n";	
		for(int i =0; i < 4; i++){
			ret = mycache.noAllocWriteMiss(mySets.at(i));
			logfile << ret << "," << mycache.getSize() << "; ";
		}
			
		logfile << "\n";
		for(int i =0; i < 4; i++){
			ret = mycache.Set_Prefetching(mySets.at(i));
			logfile << ret  << "," << mycache.getSize() << "; ";
		}
		logfile << "\n";
		for(int i = 0; i < 4; i++){
			ret = mycache.Prefetch_Miss(mySets.at(i));
			logfile << ret << "," << mycache.getSize() << "; ";
		}


		
	}

}
