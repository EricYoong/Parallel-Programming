#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "SDList.h"
#include <map>
using namespace std;

class CityParcel {
private:
	string pName;
	string type;
	int id;
	int pointX;
	int pointY;
	vector<SDList> shortestDistanceList;

public:
	void setPName(string);
	string getPName();
	void setid(int);
	int getid();
	void setX(int);
	int getX();
	void setY(int);
	int getY();
	void setType(string);
	string getType();
	void setShortestDistanceList(string, double);
	vector<SDList> getShortestDistanceList();
	double getShortestDistance(int);
	void CityParcel::setVectorSize(int);

	//default constructor
	CityParcel(string, int, int, int);
	CityParcel(string, int, int);
	CityParcel();
};

