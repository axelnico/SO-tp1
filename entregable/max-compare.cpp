#include <iostream>
#include <cstdlib>
#include "ConcurrentHashMap.hpp"
#include <time.h>
#include <chrono>


using namespace std;

static chrono::time_point<chrono::high_resolution_clock> start_time;

void start_timer() {
    start_time = chrono::high_resolution_clock::now();
}

double stop_timer() {
    chrono::time_point<chrono::high_resolution_clock> end_time = chrono::high_resolution_clock::now();
    return double(chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count());
}

int main(int argc, char **argv) {
    pair<string, unsigned int> p, threaded_p;
    list<string> l = { "corpus-0", "corpus-1", "corpus-2", "corpus-3", "corpus-4" };

    if (argc != 3) {
        cerr << "uso: " << argv[0] << " #tarchivos #tmaximum" << endl;
        return 1;
    }
    start_timer();
    p = maximum(atoi(argv[1]), atoi(argv[2]), l);
    auto time_single_thread = stop_timer();

    start_timer();
    threaded_p = maximum_concurrent(atoi(argv[1]), atoi(argv[2]), l);
    auto time_multi_thread = stop_timer();

    if (p == threaded_p) {
        cout << time_single_thread;
        cout << " " << time_multi_thread << endl;
    } else {
        cout << "Algo anda mal" << endl;
    }


    return 0;
}

