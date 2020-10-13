#include "SDList.h"


SDList::SDList() {

}

SDList::SDList(string name, double distancetmp)
{
	setPName(name);
	setDistance(distancetmp);
}

void SDList::setPName(string tmpName) {
	pName = tmpName;
}

void SDList::setDistance(double tmpDistance) {
	distance = tmpDistance;
}

string SDList::getPName() {
	return pName;
}

double SDList::getDistance() {
	return distance;
}

