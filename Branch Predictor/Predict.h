#ifndef PREDICT_H
#define PREDICT_H
#include <string>
#include <vector>

#include <iostream>
using namespace std; 

class Predict{
	private:
		vector<unsigned long> address;
		vector<std::string>perform;
		pair<unsigned long, unsigned long> alwaysTaken(std::string check);
		pair<unsigned long, unsigned long> alwaysNotTaken(std::string check);
	public: 
		Predict();
		pair<unsigned long,unsigned long> bimodalSingleBit(int tablesize);
		Predict(vector<unsigned long> aaddress, vector<std::string> pperform){
		address= aaddress;
		perform = pperform;}
		void runBranch(Predict, std::string);
		pair<unsigned long, unsigned long> getTaken(){return alwaysTaken("T");};
		pair<unsigned long, unsigned long>  getNotTaken(){return alwaysTaken("NT");};
		pair<unsigned long, unsigned long>bimodalDoubleBit(int tablesize);
		pair<unsigned long, unsigned long>gShare(int history);
		pair<unsigned long, unsigned long>tournament(int ghistory, int Btablesize);

};
#endif
