#include <stdio.h>
#include "Predict.h"
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
/*
0 - Strongly Not Taken
1 - Weakly Not Taken
2 - Weakly Taken
3 - Strongly Taken
*/
using namespace std;
Predict::Predict(){
}
void Predict::runBranch(Predict predict, std::string file){
	vector<int> tableValues = {16, 32, 128, 256, 512, 1024, 2048};
	pair<unsigned long, unsigned long> ret;  
	ofstream logfile(file);
	if(!logfile.is_open()){
		cout << "Unable to write to file." << endl;
	
	}
	ret = predict.getTaken();
	logfile << ret.first << "," << ret.second << "; ";
	logfile << "\n";
	ret = predict.getNotTaken();
	logfile << ret.first << "," << ret.second << "; ";;
	logfile << "\n";
	for(int i = 0; i < tableValues.size(); i++){
		ret = predict.bimodalSingleBit(tableValues.at(i)); //at 16, 32, 128, 256... 2048
		logfile << ret.first << "," << ret.second << "; ";
	}
	logfile << "\n";
	for(int i = 0; i < tableValues.size(); i++){
		ret = predict.bimodalDoubleBit(tableValues.at(i));
		logfile << ret.first << "," << ret.second<< "; ";
	}
	logfile << "\n";
	for(int i = 3; i <= 11; i++){
		ret = predict.gShare(i);
		logfile << ret.first << "," << ret.second<< "; ";
	}
	logfile << "\n";
	ret = predict.tournament(11, tableValues.size());
	logfile << ret.first << "," << ret.second<< "; ";
	logfile.close();
	
}
pair<unsigned long, unsigned long> Predict::alwaysTaken(std::string check){
	 unsigned long ret = 0; 
	/*Until the end of instructions, check to see if actual prediction matches predicted Taken*/
	for(int i = 0; i < perform.size(); i++){
		/*If actual == predicted, then increment the number of correct predictions*/
		if(perform.at(i) == check){
			ret++; 
		}
	}

	return pair<unsigned long, unsigned long> (ret, perform.size());
}

pair<unsigned long, unsigned long> Predict::alwaysNotTaken(std::string check){
	unsigned long ret = 0; 
	/*Until the end of instructions, check to see if actual prediction matches predicted Not Taken*/
	for(int i = 0; i < perform.size(); i++){
		/*If actual == predicted, then increment the number of correct predictions*/
		if(perform.at(i) == check){
			ret++; 
		}
	}
	
	return pair<unsigned long, unsigned long> (ret, perform.size());
}

pair<unsigned long, unsigned long> Predict::bimodalSingleBit(int tablesize){
	
	unsigned long correct = 0;
	vector<int>Table(tablesize, 1); //create a table of size with all values set to TAKEN
	/*Loop until the size of instructions*/	
	for(int i = 0; i < perform.size(); i++){
		/*calculate the index at address mod tablesize*/
		int index = address.at(i) % tablesize; 
		/*Look at the Table for that index and get the prediction*/
		int guess = Table.at(index);
		/*Get the actual prediction at that index*/
		std::string actual = perform.at(i); 
		if((guess == 1) && (actual == "T")){
			//prediction was correct
			correct++;	
		}
		if((guess == 1) && (actual == "NT")){	
			//prediction was wrong so update the table to 0
			Table.at(index) = 0; 
		}
		if((guess == 0) && (actual == "NT")){
			//prediction was correct
			correct++;
		}
		if((guess == 0) && (actual == "T")){
			//prediction was wrong, so update the table to 1
			Table.at(index) = 1;
		}
	}
	return pair<unsigned long, unsigned long> (correct, perform.size());

}

pair<unsigned long, unsigned long> Predict::bimodalDoubleBit(int tablesize){
	//create table of size and set all values to Strongly Taken	
	vector<int>Table(tablesize, 3); 
	unsigned long correct = 0;
	/*Loop until the size of instructions*/	
	for(int i = 0; i < perform.size(); i++){
		/*calculate the index at address mod tablesize*/
		int index = address.at(i) % tablesize; 
		/*Look at the Table for that index and get the prediction*/
		int guess = Table.at(index);
		/*Get the actual prediction at that index*/
		std::string actual = perform.at(i); 
		/*If prediction was weakly taken or strongly taken*/
		if(guess >=2){
			/*actual was Taken*/
			if(actual == "T"){
				/*guess was weakly taken*/
				if(guess == 2){
					/*set the table to strongly taken*/
					Table.at(index) = 3;				
				}
					correct++;
					 //prediction was correct
			}
			else{
				/*predictin was wrong so move down to weakly not taken*/
				Table.at(index) = guess -1; 
			}
		}
		/*if prediction was strongly not taken and weakly not taken*/
		else if(guess >= 0){
			/*if actual prediction was NT*/
			if(actual == "NT"){
				/*my guess was weakly not taken*/
				if(guess == 1){
					/*prediction was correct so move to strongly not taken*/
					Table.at(index) = 0;	
				}
				correct++;
				//prediction was correct
			}
			else{
				/*prediction was not correct so move up to weakly taken*/
				Table.at(index) = guess +1; 
				 			
			}
		}
	}
	return pair<unsigned long, unsigned long> (correct, perform.size());

}
pair<unsigned long, unsigned long>  Predict::gShare(int history){
	unsigned int gsize = (int) pow(2,history)-1;
	vector<int>Table(2048, 3); 
	unsigned long correct = 0;
	int GR = 0; //starts with all zeros (NT)
	for(int i = 0; i < perform.size(); i++){ 
		int index = (address.at(i) ^ (GR &gsize)) % 2048; 
		int guess = Table.at(index);
		std::string actual = perform.at(i); 	
		
		if(guess >=2){
			if(actual == "T"){
				if(guess == 2){
					Table.at(index) = 3;				
				}
					correct++;
					 
			}
			else{
				Table.at(index) = guess -1; 
			}
		}
		else if(guess >= 0){
			if(actual == "NT"){
				if(guess == 1){
					Table.at(index) = 0;	
				}
				correct++;
			}
			else{
				Table.at(index) = guess +1; 
				 			
			}
		}
		int valid = 0; 
		if(actual == "T"){
			 valid = 1;
		}
		GR <<=1;
		GR |= valid;
		GR = (int) (GR & (int)(pow(2,history)-1));
	
	}
	return pair<unsigned long, unsigned long> (correct, perform.size());
}

pair<unsigned long, unsigned long> Predict::tournament(int ghistory, int Btablesize){

	unsigned int gsize = (int) pow(2,ghistory)-1;
	vector<int>GTable(2048, 3); 
	vector<int>TTable(2048, 0);
	vector<int>BTable(2048, 3);  
	bool hit1;
	bool hit2; 
	unsigned long correct = 0;
	//unsigned long misprediction = 0;
	int GR = 0; 
	for(int i = 0; i < perform.size(); i++){ 
		int index1 = (address.at(i) ^ (GR &gsize)) % 2048; 
		int guess1 = GTable.at(index1);
		std::string actual1 = perform.at(i);
		if(guess1 >=2){
			if(actual1 == "T"){
				if(guess1 == 2){
					GTable.at(index1) = 3;				
				}
					//correct++;
					hit1 = true; 
			}
			else{
				GTable.at(index1) = guess1 -1; 
				hit1 = false;
			}
		}
		else if(guess1 >= 0){
			if(actual1 == "NT"){
				if(guess1 == 1){
					GTable.at(index1) = 0;	
				}
				//correct++;
				hit1 = true;
			}
			else{
				GTable.at(index1) = guess1 +1; 
				hit1 = false; 			
			}
		}

		int valid = 0; 
		if(actual1 == "T"){
			 valid = 1;
		}
		GR <<=1;
		GR |= valid;
		int index2 = address.at(i) % 2048; 
		int guess2 = BTable.at(index2);
		std::string actual2 = perform.at(i);
		
		if(guess2 >=2){
			if(actual2== "T"){
				if(guess2 == 2){
					BTable.at(index2) = 3;				
				}
					hit2 = true;
			}
			else{
				BTable.at(index2) = guess2 -1; 
				hit2 = false;
			}
		}
		else if(guess2 >= 0){
			if(actual2 == "NT"){
				if(guess2 == 1){
					BTable.at(index2) = 0;	
				}
				hit2 = true;
			}
			else{
				BTable.at(index2) = guess2 +1; 	
				hit2 = false;		
			}
		}
/*Selection sort*/
	int index = address.at(i) % 2048;
	int guess = TTable.at(index);
	/*If prediction is weakly taken*/
	if(guess >= 2){
		/*Bimodal prediction was true*/
		if(hit2 == true){
			/*my guess was weakly taken and gshare was false*/
			if(guess ==2 && hit1 == false){
				/*set table to strongly taken*/
				TTable.at(index) = 3; 		
			}
			correct++;
			//prediction was correct
		}
		/*Bimodal was wrong and ghsare was correct*/
		if(hit2 == false && hit1 == true){
			/*Move the state to weakly not taken*/
			TTable.at(index) = guess -1;
		}
	
	}
	/*prediction is strong not taken or weakly not taken*/
	else if(guess >= 0){
		 if(hit1 == true){ //gshare is true
			if(guess == 1 && hit2 == false){	//prediction is weakly not taken, and bimodal was wrong
				TTable.at(index) = 0;		//move the state to strongly not taken 
			}
			correct++;	
			//prediction was correct
		}
		if(hit1 == false && hit2 == true){		//gshare is wrong and bimodal is correct
			TTable.at(index) = guess +1;		//move the state to weakly taken
		}
	}
	


	}
	return pair<unsigned long, unsigned long> (correct, perform.size());

}














