#include "CityParcel.h"
#include "SDList.h"
#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>
#include <math.h>
#include <algorithm>
#include <map>
#include <string>
#include <Windows.h>
#include <omp.h>
#include <future>
using namespace std;

typedef long double INT999;
vector<CityParcel> cityPointer, parcelPointer;
map<string, int> flag_table;
vector<int> permutationList2, permutationList3, cityTmp, parcelTmp, tmpPointer;
vector<vector<int>>parcelPermutation, cityPermutation;
ifstream infile;
ofstream outFile;
string tmp, tmp2;

double min1 = 9999999999;

bool wayToSort2(int i, int j) { return i > j; }

bool readFile() {
	int tmpX = 0, tmpY = 0;

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
				permutationList3.push_back(cityPointer.size());
				cityPointer.push_back(CityParcel(tmp, tmpX, tmpY, cityPointer.size()));
			}
			else if (tmp.find("Parcel") != string::npos) {
				permutationList2.push_back(parcelPointer.size());
				parcelPointer.push_back(CityParcel(tmp, tmpX, tmpY, parcelPointer.size()));
			}
		}
		sort(permutationList2.begin(), permutationList2.end(), wayToSort2);
		sort(permutationList3.begin(), permutationList3.end(), wayToSort2);
		infile.close();
		return true;
	}
	else
		return false;
}

//Factorial
INT999 calculator(int tmp12) {
	INT999 num = 0;

	for (int i = 0; i <= tmp12 - 1; i++) {
		if (num == 0)
			num = i + 1;
		num *= (i + 1);
	}
	return num;
}

int calculator1(int tmp13) {
	int result;
	if (tmp13 == 1)
		return tmp13;
	result = tmp13 * calculator1(tmp13 - 1);
	return result;
}

bool compareArray(string flag_tag) {
	bool valid = true;

	if (flag_table[flag_tag] == 1)
		valid = false;

	if (!valid)
		return false;
	return true;
}

void calculate_total_distance2(vector<int> &parcel, vector<int> &city) {
	double totalDistance = 0;
	//tmp is current pointer
	//tmp2 is next pointer

	totalDistance += cityPointer[0].getShortestDistance(parcel[0]);

	for (int i = 0; i < parcel.size(); i++) {
		totalDistance += parcelPointer[parcel[i]].getShortestDistance(city[i]);
		if (parcel.size() - 1 != i)
			totalDistance += cityPointer[city[i]].getShortestDistance(parcel[i + 1] + 1);
	}

	totalDistance += cityPointer[city[city.size() - 1]].getShortestDistance(0);
	
	#pragma omp critical
	if (min1 > totalDistance) {
		min1 = totalDistance;
		cityTmp = city;
		parcelTmp = parcel;
	}
}

//Shortest distance between two pointss
void calculation_short_distance(CityParcel &tmp, vector<CityParcel> &tmp2) {
	double x = tmp.getX(), y = tmp.getY(), x1, y1;
	string tmpName;

	if (tmp.getType() == "Point") {
		//get the point to depot distance
		x1 = cityPointer[0].getX();
		y1 = cityPointer[0].getY();
		tmp.setShortestDistanceList("Depot", (double)sqrt(pow((x - x1), 2) + pow((y - y1), 2)));
	}

	for (int i = 0; i < tmp2.size(); i++) {
		x1 = tmp2[i].getX();
		y1 = tmp2[i].getY();
		tmpName = tmp2[i].getPName();
		tmp.setShortestDistanceList(tmpName, (double)sqrt(pow((x - x1), 2) + pow((y - y1), 2)));
	}
}

void brute_force_distance(vector<CityParcel> &tmp) {
	for (int i = 0; i < tmp.size(); i++) {
		if (tmp[i].getType() == "Depot")
			calculation_short_distance(tmp[i], parcelPointer);
		else if (tmp[i].getType() == "Point")
			calculation_short_distance(tmp[i], parcelPointer);
		else if (tmp[i].getType() == "Parcel")
			calculation_short_distance(tmp[i], cityPointer);
	}
}

void combinePermutation2() {
#pragma omp parallel for schedule (dynamic, 1)
	for (int a = 0; a < cityPermutation.size(); a++) {
		#pragma omp parallel for schedule (dynamic, 1)
		for (int i = 0; i < parcelPermutation.size(); i++) {
			calculate_total_distance2(parcelPermutation[i], cityPermutation[a]);
		}
	}
}

//Calculate permutation for parcel
// Iterative function to find permutations of a String
void parcelPermutations()
{
	map<int, boolean> tracker_parcel;
	INT999 n_minus_r = parcelPointer.size() - (cityPointer.size() - 1);
	if (n_minus_r == 0)
		n_minus_r = 1;
	INT999 noOfPermutationParcel = calculator(parcelPointer.size()) / calculator(n_minus_r);
	INT999 noOfPermutationCity = calculator(cityPointer.size() - 1) / calculator(1);
	INT999 tmpLineNo = 0, count2 = 0;
	int n = parcelPointer.size(), tmpSize = cityPointer.size() * 2, l = cityPointer.size() - 1, tracker = 0;
	string flag_tag = "P";
	bool isDuplicate = false;
	tmpPointer.clear();
	tmpPointer.resize(cityPointer.size() - 1);

	while (1)
	{
		flag_tag = "P";
		//print out
		if (!isDuplicate) {
			if (count2 == 0) {
				for (int a = 0; a < cityPointer.size() - 1; a++)
					tmpPointer[a] = permutationList2[a];

				for (int i = 0; i < tmpPointer.size(); i++)
					flag_tag.append(to_string(tmpPointer[i] + cityPermutation.size()) + " ");

				flag_table[flag_tag] = true;

				parcelPermutation.push_back(tmpPointer);
				isDuplicate = false;
			}
			else {
				for (int a = 0; a < cityPointer.size() - 1; a++)
					tmpPointer[a] = permutationList2[a];

				for (int i = 0; i < tmpPointer.size(); i++)
					flag_tag.append(to_string(tmpPointer[i] + cityPermutation.size()) + " ");

				if (parcelPermutation.size() != noOfPermutationParcel) {
					//check is the number of parcel is more than city
					if (n - l == 0 || n - l == 1) {
						parcelPermutation.push_back(tmpPointer);
						isDuplicate = false;

						flag_table[flag_tag] = true;
					
					}
					else {
						if (compareArray(flag_tag)) {
							parcelPermutation.push_back(tmpPointer);
							isDuplicate = false;

							flag_table[flag_tag] = true;

						}
						else {
							tmpLineNo = count2 + noOfPermutationCity - 1;
							isDuplicate = true;
						}
					}
				}
				else
					return;
			}
		}
		else {
			if (count2 == tmpLineNo)
				isDuplicate = false;
		}

		/* below code will rearrange the string to next lexicographically
		ordered permutation (if any) or return if we are already at
		highest possible permutation */
		// Find largest index k such that s[i + 1] is more than s[i]
		int k = n - 1;
		int i = 0;
		while (permutationList2[i + 1] >= permutationList2[i])
		{
			// if i is first index of the string, we are already at last
			// possible permutation (string is sorted in reverse order)
			if (++i == k)
				return;
		}

		// find highest index j to the right of index i such that 
		// s[j] > s[i + 1] (s[i..i + 1] is sorted in reverse order)
		int j = 0;
		while (j < i && permutationList2[j] <= permutationList2[i + 1])
			j++;

		// Swap characters at index i + 1 with index j
		swap(permutationList2[i + 1], permutationList2[j]);

		// reverse the substring s[i..i + 1] and return true
		reverse(permutationList2.begin(), permutationList2.begin() + i + 1);
		count2++;
	}
}

// Iterative function to find permutations of a String
void cityPermutations()
{
 	int l = cityPointer.size() - 1;
	tmpPointer.clear();
	tmpPointer.resize(cityPointer.size() - 1);

	while (1)
	{
		//Output
		for (int a = 0; a < cityPointer.size() - 1; a++)
			tmpPointer[a] = permutationList3[a];

		cityPermutation.push_back(tmpPointer);

		/* below code will rearrange the string to next lexicographically
		ordered permutation (if any) or return if we are already at
		highest possible permutation */
		// Find largest index k such that s[i + 1] is more than s[i]
		int k = l - 1;
		int i = 0;
		while (permutationList3[i + 1] >= permutationList3[i])
		{
			// if i is first index of the string, we are already at last
			// possible permutation (string is sorted in reverse order)
			if (++i == k)
				return;
		}

		// find highest index j to the right of index i such that 
		// s[j] < s[i + 1] (s[i..n + 1] is sorted in reverse order)
		int j = 0;
		while (j < i && permutationList3[j] <= permutationList3[i + 1])
			j++;

		// Swap characters at index i + 1 with index j
		swap(permutationList3[i + 1], permutationList3[j]);

		// reverse the substring s[i..n + 1] and return true
		reverse(permutationList3.begin(), permutationList3.begin() + i + 1);
	}
}

void main()
{
	HANDLE hProcess = GetCurrentProcess();
	//SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS);
	int parcelCounter, cityCounter;
	vector<string> tmpList;

	if (readFile()) {
		brute_force_distance(cityPointer);
		brute_force_distance(parcelPointer);
		parcelPermutations();
		cityPermutations();

		combinePermutation2();

		outFile.open("solution.txt");

		parcelCounter = 0, cityCounter = 0;
		outFile << "Depot" << endl;
		for (int a = 1; a < cityPointer.size() * 2 - 1; a++) {
			if (a % 2 == 1)
				outFile << parcelPointer[parcelTmp[parcelCounter++]].getPName() << endl;
			else if (a % 2 == 0)
				outFile << cityPointer[cityTmp[cityCounter++]].getPName() << endl;
		}
		outFile << "Depot" << endl;
		outFile.close();
	}
	else {
		outFile.open("solution.txt");

		outFile << "Error Read File" << endl;

		outFile.close();
	}
}

