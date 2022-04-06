/*
 * Write a program that takes a large list of doubles and returns its average.
 *
 *  Created on: April 5, 2022
 *      Author: Eyup Agirtmis
 */
#include <iostream>
#include <thread>
#include <random>
#include <ctime>
#include <mutex>
using namespace std;

mutex mtx;

/**
 * Function to calcualte average iteratively 
 */
double getAvgIteratively(int first, int last, double *double_arr) {
	double avg = 0.0;
	double cnt = 1.0;
	for (int i = first; i <= last; i++) {
		double x = double_arr[i];
		avg += (x - avg) / cnt;
		cnt++;
	}
	return avg;
}

/**
 * Thread function of computing average of a block of the array
 */
void getAvg(int size, double *double_arr, double *g_avg, int threadID, int numOfThreads) {
	if (double_arr == NULL) return;

	// Determine the block for each thread
	int block_size = (size + numOfThreads - 1) / numOfThreads;

	// Calculate thread-specific start index
	int my_first = block_size * threadID;

	// Calculate thread-specific end index
	int my_last = my_first + block_size - 1 > size ? size - 1 : my_first + block_size - 1;

	// Calcualte average of block
	double avg = getAvgIteratively(my_first, my_last, double_arr);

	// Calcualte weight of local average
	double actual_block_size = my_last - my_first + 1;
	double weight = actual_block_size/(double)size;

	// Lock mutex and add average x weight to the global average
	mtx.lock();
	*g_avg += avg * weight;	
	mtx.unlock();
}

int main(){
	int arr_size, numOfThreads = 0;
	double average = 0; 

	// Decide how many threads for calculation
	cout << "How many threads for calculation? Please type an integer number: " << flush;
	scanf("%d", &numOfThreads);

	// Decide the size of array
	cout << "Please type the size of the array: " << flush;
	scanf("%d", &arr_size);

	/**
	 * Catch edge cases:
	 * numOfThreads is 0
	 * array size is 0
	 * array size is less than thread size
	 */
	if (numOfThreads == 0) {
		cout << "Number of threads cannot be 0" << endl;
		return 0;
	}

	if (arr_size == 0) {
		cout << "The size of the array cannot be 0" << endl;
		return 0;
	}

	if (arr_size < numOfThreads) {
		cout << "Number of threads cannot be greater than the size of the array" << endl;
		return 0;
	}

	double* averages= (double*) calloc(numOfThreads, sizeof(double));;
	double* double_arr = (double*) calloc(arr_size, sizeof(double));

	/* Populate array with dummy numbers */
	for(int i = 0; i < arr_size; i++) {
		double_arr[i] = i + 1;
	}

	time_t start = clock();

	vector<thread> calc_thread_pool;
	// Calculate average in parallel
	for(int i = 0; i < numOfThreads; i++) {
		thread t(getAvg, arr_size, double_arr, &average, i, numOfThreads);
		calc_thread_pool.push_back(move(t));
	}

	for(thread &t : calc_thread_pool) {
		if(t.joinable())
			t.join();
	}
 
	cout << "Average: " << average << endl;

	time_t end = clock();
	cout << "It takes " << ((end-start) * 1000 / (CLOCKS_PER_SEC + 0.0)) << " milliseconds" << endl;

	return 0;
}
