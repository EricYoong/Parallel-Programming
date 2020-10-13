#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "SDList.h"
using namespace std;


class CityParcel {
private:
	string pName;
	string type;
	int pointX;
	int pointY;
	vector<SDList> shortestDistanceList;

public:
	void setPName(string);
	string getPName();
	void setX(int);
	int getX();
	void setY(int);
	int getY();
	void setType(string);
	string getType();
	void setShortestDistanceList(string, double);
	vector<SDList> getShortestDistanceList();
	double getShortestDistance(string);
	double getShortestDistance(int);

	//default constructor
	CityParcel(string, int, int, int);
	CityParcel(string, int, int);
	CityParcel();
};

