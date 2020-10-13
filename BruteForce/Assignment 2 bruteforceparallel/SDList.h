#include <iostream>
#include <string>
#pragma once

using namespace std;


class SDList
{
private:
	string pName;
	double distance;

public:
	void setPName(string);
	string getPName();
	void setDistance(double);
	double getDistance();

	SDList(string, double);
	SDList();
	
};

