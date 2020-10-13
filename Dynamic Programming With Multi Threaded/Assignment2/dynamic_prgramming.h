#pragma once
#include "CityParcel.h"
#include "SDList.h"
#include "limits.h"
#include <vector>;
#include <string>;
#include <unordered_map> 
#include "omp.h"
#include <thread>
#include <Windows.h>
#include <future>
#include <boost/thread/barrier.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost\thread\mutex.hpp>

using namespace std;

struct state
{
	double distance;
	vector<double> distance_paths;
};


class dynamic_prgramming
{
private:
	unordered_map< int, state > matrix;
	state tmpState, min_distance_state;
	vector<vector<double>> city_parcel_table;
	int cityCnt, parcelCnt;
	vector<double> arrayList, emptyList;

public:

	void init_multi_threading(const vector<vector<double>>&, const int);
	void construct_distance_table(vector<CityParcel>);
	void init_all_variable(vector<CityParcel>, int, int);
	void init_empty_list();
	void start_tcp(boost::barrier&, const vector<vector<double>>&, int, int, int, int, const int, int);
	state tspC(const vector<vector<double>>&, int, int, int, int, vector<unordered_map< int, state >>&);
	state tspP_init(const vector<vector<double>>&, int, int, int, int, vector<unordered_map< int, state >>);
	state tspP(const vector<vector<double>>&, int, int, int, int, vector<unordered_map< int, state >>&);
	vector<double> getResult();
	dynamic_prgramming(vector<CityParcel>, int, int, const int);
};

