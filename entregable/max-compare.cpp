#include <iostream>
#include <cstdlib>
#include "ConcurrentHashMap.hpp"
#include <time.h>

#define BILLION  1000000000L;
#define mil 1000

#define MEDIR_TIEMPO(var)                                   \
{                                                           \
    __asm__ __volatile__ (                                  \
        "xor %%rdx, %%rdx\n\t"                              \
        "xor %%rax, %%rax\n\t"                              \
        "lfence\n\t"                                        \
        "rdtsc\n\t"                                         \
        "sal $32, %%rdx\n\t"                                \
        "or %%rdx, %%rax\n\t"                               \
        "movq %%rax, %0\n\t"                                \
        : "=r" (var)                                        \
        : /* no input */                                    \
        : "%rax", "%rdx"                                    \
    );                                                      \
}

#define MEDIR_TIEMPO_START(start)                           \
{                                                           \
    /* warm up ... */                                       \
    MEDIR_TIEMPO(start);                                    \
    MEDIR_TIEMPO(start);                                    \
    MEDIR_TIEMPO(start);                                    \
}

#define MEDIR_TIEMPO_STOP(end)                              \
{                                                           \
    MEDIR_TIEMPO(end);                                      \
}

using namespace std;

int main(int argc, char **argv) {
    pair<string, unsigned int> p, threaded_p;
    list<string> l = { "corpus-0", "corpus-1", "corpus-2", "corpus-3", "corpus-4" };
    unsigned long start1, stop1, start2, stop2;

    if (argc != 3) {
        cerr << "uso: " << argv[0] << " #tarchivos #tmaximum" << endl;
        return 1;
    }
    MEDIR_TIEMPO_START(start1);
    p = maximum(atoi(argv[1]), atoi(argv[2]), l);
    MEDIR_TIEMPO_STOP(stop1);

    MEDIR_TIEMPO_START(start2);
    threaded_p = maximum_concurrent(atoi(argv[1]), atoi(argv[2]), l);
    MEDIR_TIEMPO_STOP(stop2);

    if (p == threaded_p) {
        cout << ( stop1 - start1 );
        cout << " " << ( stop2 - start2 ) << endl;
    }


    return 0;
}

