#include <iostream>
#include <cstdlib>
#include "ConcurrentHashMap.hpp"
#include <time.h>

#define BILLION  1000000000L;
#define mil 1000

using namespace std;

int main(int argc, char **argv) {
    pair<string, unsigned int> p, threaded_p;
    list<string> l = { "corpus-0", "corpus-1", "corpus-2", "corpus-3", "corpus-4" };
    struct timespec start1, stop1, start2, stop2;

    if (argc != 3) {
        cerr << "uso: " << argv[0] << " #tarchivos #tmaximum" << endl;
        return 1;
    }
    clock_gettime( CLOCK_REALTIME, &start1);
    p = maximum(atoi(argv[1]), atoi(argv[2]), l);
    clock_gettime( CLOCK_REALTIME, &stop1);

    clock_gettime( CLOCK_REALTIME, &start2);
    threaded_p = maximum_concurrent(atoi(argv[1]), atoi(argv[2]), l);
    clock_gettime( CLOCK_REALTIME, &stop2);

    if (p == threaded_p) {
        cout << ( stop1.tv_nsec - start1.tv_nsec ) / mil;
        cout << " " << ( stop2.tv_nsec - start2.tv_nsec ) / mil << endl;
    }


    return 0;
}

