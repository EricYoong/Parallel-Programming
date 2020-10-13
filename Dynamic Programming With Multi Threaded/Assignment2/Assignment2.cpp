#include "CityParcel.h"
#include "SDList.h"
#include "dynamic_prgramming.h"
#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>
#include <math.h>
#include <algorithm>
#include <Windows.h>
#include <omp.h>
using namespace std;

typedef long double INT999;

vector<CityParcel> cityPointer, parcelPointer, allPointer;
ifstream infile;
ofstream outFile;
string tmp, tmp2;
int tmpX = 0, tmpY = 0;

bool wayToSort2(int i, int j) { return i > j; }

bool readFile() {
	infile.open("tsp.txt");

	if (infile.is_open()) {
		while (!infile.eof()) {

			getline(infile, tmp, ' ');
			getline(infile, tmp2, ' ');
			tmpX = stoi(tmp2);
			getline(infile, tmp2);
			tmpY = stoi(tmp2);

			if (tmp.find("Depot") != string::npos)
				cityPointer.push_back(CityParcel(tmp, tmpX, tmpY));
			else if (tmp.find("Point") != string::npos) {
				cityPointer.push_back(CityParcel(tmp, tmpX, tmpY));
			}
			else if (tmp.find("Parcel") != string::npos) {
				parcelPointer.push_back(CityParcel(tmp, tmpX, tmpY));
			}
		}

		allPointer = cityPointer;

		for (int i = 0; i < parcelPointer.size(); i++) {
			allPointer.push_back(parcelPointer[i]);
		}

		infile.close();
		return true;
	}
	else
		return false;
}

//Shortest distance between two pointss
void calculation_short_distance(CityParcel &tmp, vector<CityParcel> &tmp2) {
	double x = tmp.getX(), y = tmp.getY(), x1, y1;
	string tmpName;
	bool valid = true;

	#pragma omp parallel for
	for (int i = 0; i < tmp2.size(); i++) {
		tmpName = tmp2[i].getPName();
		if (tmp.getPName().find(tmpName) == string::npos && tmp.getType().find(tmp2[i].getType()) == string::npos) {
			x1 = tmp2[i].getX();
			y1 = tmp2[i].getY();
			tmp.setShortestDistanceList(tmpName, (double)sqrt(pow((x - x1), 2) + pow((y - y1), 2)));
		}
		else {
			tmp.setShortestDistanceList(tmpName, 0);
		}
			
	}
}

void brute_force_distance(vector<CityParcel> &tmp) {

	#pragma omp parallel for
	for (int i = 0; i < tmp.size(); i++) {
		if (tmp[i].getType() == "Depot") {
			calculation_short_distance(tmp[i], cityPointer);
			calculation_short_distance(tmp[i], parcelPointer);
		}
		else if (tmp[i].getType() == "Point") {
			calculation_short_distance(tmp[i], cityPointer);
			calculation_short_distance(tmp[i], parcelPointer);
		}
		else if (tmp[i].getType() == "Parcel") {
			calculation_short_distance(tmp[i], cityPointer);
			calculation_short_distance(tmp[i], parcelPointer);
		}
	}
}

int main(void)
{
	HANDLE hProcess = GetCurrentProcess();
	SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS);

	if (readFile()) {
		brute_force_distance(allPointer);
		dynamic_prgramming d2 = dynamic_prgramming(allPointer, cityPointer.size(),  parcelPointer.size(), 4);
		vector<double> result = d2.getResult();
		outFile.open("solution.txt");
		for (int a = 0; a < result.size() ; a++)
			outFile << allPointer[result[a]].getPName() << endl;
		outFile.close();
	}
	else {
		outFile.open("solution.txt");
		outFile << "Error Read File" << endl;
		outFile.close();
	}
}

