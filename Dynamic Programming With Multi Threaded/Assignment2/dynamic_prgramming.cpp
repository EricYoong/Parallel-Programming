#include "dynamic_prgramming.h"

boost::mutex mtx;

dynamic_prgramming::dynamic_prgramming(vector<CityParcel> tmp, int cityCnt1, int parcelCnt1, const int num_core) {
	vector<int> prev_Point;
	cityCnt = cityCnt1;
	parcelCnt = parcelCnt1;
	init_all_variable(tmp, cityCnt1, parcelCnt1);
	//tspC(city_parcel_table, 0, 0, 0, 1, matrix);
	init_multi_threading(city_parcel_table, num_core);
}

void dynamic_prgramming::init_all_variable(vector<CityParcel> tmp, int cityCnt, int parcelCnt) {

	//init the shortest distance table
	thread t1(&dynamic_prgramming::construct_distance_table, this, tmp);
	//init empty list for array list and matrix
	thread t3(&dynamic_prgramming::init_empty_list, this);

	t1.join();
	t3.join();
}

void dynamic_prgramming::construct_distance_table(vector<CityParcel> tmp) {
	vector<vector<double>> emptyArray;
	vector<SDList> tmpArray;
	vector<double> emptyList2;
	city_parcel_table.reserve(tmp.size());

	//init empty list for empty array
	//use empty array to start a new city parcel table

	for (int j = 0; j < tmp.size(); j++)
		emptyList2.push_back(0);

	for (int i = 0; i < tmp.size(); i++)
		emptyArray.push_back(emptyList2);

	city_parcel_table = emptyArray;

	//Contruct a shortest distance table
	for (int i = 0; i < tmp.size(); i++) {
		tmpArray = tmp[i].getShortestDistanceList();
		for (int j = 0; j < tmpArray.size(); j++)
			city_parcel_table[i][j] = tmpArray[j].getDistance();
	}

	tmpArray.clear();
	emptyArray.clear();
	emptyList2.clear();
}


void dynamic_prgramming::init_empty_list() {
	for (int j = 0; j < cityCnt * 2; j++)
		emptyList.push_back(0);

	matrix[1].distance = INT_MAX;
}

void dynamic_prgramming::init_multi_threading(const vector<vector<double>>& cities, const int num_core) {
	vector<boost::thread> threadpool_state;
	boost::barrier br(parcelCnt);
	int counter = 0;

	for (int tid = 0; tid < num_core; tid++)
		for (int i = ((tid * 2) / 2) + cityCnt; i < cities.size(); i = i + num_core) 
			threadpool_state.push_back(boost::thread(boost::bind(&dynamic_prgramming::start_tcp, this, boost::ref(br), cities, i, 0, 1, 1, num_core, tid)));

	for (int i = 0; i < threadpool_state.size(); i++)
		threadpool_state[i].join();
}

void dynamic_prgramming::start_tcp(boost::barrier& cur_barier, const vector<vector<double>>& cities, int pPos, int cPos, int pos, int visited, const int num_core, int tid) {
	unordered_map< int, state> tmp_unordered_map1;
	vector<unordered_map< int, state>> tmpGroupState;
	vector<double> tmpList;

	//init a group state for the thread to have their own state table
	tmp_unordered_map1[0].distance = INT_MAX;

	for (int i = 0; i < cities.size(); i++) {
		tmpGroupState.push_back(tmp_unordered_map1);
	}

	tmp_unordered_map1.clear();

	int nextVisit = visited | (1 << pPos);
	int test = ((tid * 2) / 2) + cityCnt;
	state state_private = tspP_init(cities, pPos, cPos, pos, nextVisit, tmpGroupState);

	//compare the value that thread get and get the min distance
	double distance = cities[cPos][pPos] + state_private.distance;

	if (tmpGroupState[cPos].find(visited) != tmpGroupState[cPos].end()) {
		if (distance < tmpGroupState[cPos][visited].distance) {
			tmpList = state_private.distance_paths;
			tmpList[pos] = pPos;
			tmpGroupState[cPos][visited].distance_paths = tmpList;
			tmpGroupState[cPos][visited].distance = distance;
		}
	}
	else {
		tmpList = state_private.distance_paths;
		tmpList[pos] = pPos;
		tmpGroupState[cPos][visited].distance_paths = tmpList;
		tmpGroupState[cPos][visited].distance = distance;
	}

	cur_barier.wait();
	//boost::lock_guard<boost::mutex> locker(io_mutex);
	mtx.lock();
	if (tmpGroupState[cPos][visited].distance < matrix[visited].distance)
		matrix[visited] = tmpGroupState[cPos][visited];
	mtx.unlock();
}

state dynamic_prgramming::tspP_init(const vector<vector<double>>& cities, int pPos, int cPos, int pos, int visited, vector<unordered_map< int, state >> state2) {
	int curPos = pos;
	state tmpState;
	vector<double> tmpList;
	pos++;

	//Use city size
	for (int i = 0; i < cityCnt; ++i)
	{
		// can't visit ourselves unless we're ending & skip if already visited
		if (i == cPos || (visited & (1 << i)))
			continue;

		int nextVisit = visited | (1 << i);
		tmpState = tspC(cities, pPos, i, pos, nextVisit, state2);
		double distance = cities[pPos][i] + tmpState.distance;

		if (state2[pPos].find(visited) != state2[pPos].end()) {
			if (distance < state2[pPos][visited].distance) {
				tmpList = emptyList;
				if (state2[i].find(nextVisit) != state2[i].end())
					tmpList = state2[i][nextVisit].distance_paths;
				tmpList[pos] = i;
				state2[pPos][visited].distance_paths = tmpList;
				state2[pPos][visited].distance = distance;
			}
		}
		else {
			tmpList = emptyList;
			if (state2[i].find(nextVisit) != state2[i].end())
				tmpList = state2[i][nextVisit].distance_paths;
			tmpList[pos] = i;
			state2[pPos][visited].distance_paths = tmpList;
			state2[pPos][visited].distance = distance;
		}
	}

	
	tmpState = state2[pPos][visited];
	state2.clear();
	return tmpState;
}

state dynamic_prgramming::tspP(const vector<vector<double>>& cities, int pPos, int cPos, int pos, int visited, vector<unordered_map< int, state >>& state1) {
	int curPos = pos;
	state tmpState;
	vector<double> tmpList;

	pos++;

	//Use city size
	for (int i = 0; i < cityCnt; ++i)
	{
		// can't visit ourselves unless we're ending & skip if already visited
		if (i == cPos || (visited & (1 << i)))
			continue;

		int nextVisit = visited | (1 << i);
		tmpState = tspC(cities, pPos, i, pos, nextVisit, state1);
		double distance = cities[pPos][i] + tmpState.distance;

		if (state1[pPos].find(visited) != state1[pPos].end()) {
			if (distance < state1[pPos][visited].distance) {
				tmpList = emptyList;
				if (state1[i].find(nextVisit) != state1[i].end())
					tmpList = state1[i][nextVisit].distance_paths;
				tmpList[pos] = i;
				state1[pPos][visited].distance_paths = tmpList;
				state1[pPos][visited].distance = distance;
			}
		}
		else {
			tmpList = emptyList;
			if (state1[i].find(nextVisit) != state1[i].end())
				tmpList = state1[i][nextVisit].distance_paths;
			tmpList[pos] = i;
			state1[pPos][visited].distance_paths = tmpList;
			state1[pPos][visited].distance = distance;
		}
	}
	return state1[pPos][visited];
}

state dynamic_prgramming::tspC(const vector<vector<double>>& cities, int pPos, int cPos, int pos, int visited, vector<unordered_map< int, state >>& state1) {
	int curPos = pos;
	state tmpState;
	vector<double> tmpList;

	//Reached the bottom of the tree
	if (curPos == (cityCnt * 2) - 2) {
		state1[cPos][visited].distance_paths = emptyList;
		state1[cPos][visited].distance_paths[curPos] = cPos;
		state1[cPos][visited].distance = cities[cPos][0];
		return state1[cPos][visited]; // return to starting city
	}

	pos++;

	for (int i = cityCnt; i < cities.size(); i++)
	{
		// can't visit ourselves unless we're ending & skip if already visited
		if (i == pPos || (visited & (1 << i)))
			continue;
		
		int nextVisit = visited | (1 << i);

		tmpState = tspP(cities, i, cPos, pos, nextVisit, state1);
		double distance = cities[cPos][i] + tmpState.distance;
		
		if (state1[cPos].find(visited) != state1[cPos].end()) {
			if (distance < state1[cPos][visited].distance) {
				if (state1[cPos][visited].distance != 0) {
					tmpList = emptyList;
					if(state1[i].find(nextVisit) != state1[i].end())
						tmpList = state1[i][nextVisit].distance_paths;
					tmpList[pos] = i;
					state1[cPos][visited].distance_paths = tmpList;
					state1[cPos][visited].distance = distance;
				}
			}
		}
		else {
			tmpList = emptyList;
			if (state1[i].find(nextVisit) != state1[i].end())
				tmpList = state1[i][nextVisit].distance_paths;
			tmpList[pos] = i;
			state1[cPos][visited].distance_paths = tmpList;
			state1[cPos][visited].distance = distance;
		}
	}
	return state1[cPos][visited];
}

vector<double> dynamic_prgramming::getResult() {
	return matrix[1].distance_paths;
}