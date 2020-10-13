#include "CityParcel.h"

CityParcel::CityParcel() {

}

CityParcel::CityParcel(string name, int x, int y) {
	setPName(name);
	setX(x);
	setY(y);
	if (name.find("Parcel") != string::npos)
		setType("Parcel");
	else if (name.find("Point") != string::npos)
		setType("Point");
	else
		setType("Depot");
}

CityParcel::CityParcel(string name, int x, int y, int id) {
	setPName(name);
	setX(x);
	setY(y);
	setid(id);
	if (name.find("Parcel") != string::npos)
		setType("Parcel");
	else if (name.find("Point") != string::npos)
		setType("Point");
	else
		setType("Depot");
}

void CityParcel::setPName(string name) {
	pName = name;
}

string CityParcel::getPName() {
	return pName;
}

void CityParcel::setid(int tmp) {
	id = tmp;
}

int CityParcel::getid() {
	return id;
}

void CityParcel::setX(int x) {
	pointX = x;
}

int CityParcel::getX() {
	return pointX;
}

void CityParcel::setY(int y) {
	pointY = y;
}

int CityParcel::getY() {
	return pointY;
}

void CityParcel::setType(string type1) {
	type = type1;
}

string CityParcel::getType() {
	return type;
}

void CityParcel::setShortestDistanceList(string name, double distance) {
	shortestDistanceList.push_back(SDList(name, distance));
}

vector<SDList> CityParcel::getShortestDistanceList() {
	return shortestDistanceList;
}

double CityParcel::getShortestDistance(int id) {
	return shortestDistanceList[id].getDistance();
}


void CityParcel::setVectorSize(int num) {
	shortestDistanceList.resize(num);
}
