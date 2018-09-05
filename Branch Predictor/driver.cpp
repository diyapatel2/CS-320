#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <map>
#include "Predict.h"
using namespace std; 

void runBranch(Predict, std::string);
int main(int argc, char *argv[]){ 
	vector<unsigned long> input;
	vector<string> perform;
	string line; 
	if(argc != 3){
		cout << "Number of arguments is incorrect." << endl;
	}	
		ifstream file(argv[1]);
		if(file.is_open()){
			while(getline(file, line)){
				std::string getAddress = line.substr(0, line.find(" "));
				std::string getperform = line.substr(line.find(" ") + 1, getAddress.length());
				unsigned long newAddress = stoul(getAddress, nullptr, 16); //create hex address
				input.push_back(newAddress);//at it to the end of the vector
				perform.push_back(getperform);
			}
			file.close();
	}
		else{
			cerr << "Error opening file" << argv[1] << "\n";
			exit(1);
		}
		Predict p;
		Predict predict(input, perform); 
		p.runBranch(predict,argv[2]);
		return 0; 
}

