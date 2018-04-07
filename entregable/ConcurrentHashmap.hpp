#ifndef CONCURRENT_HASH_MAP_H__
#define CONCURRENT_HASH_MAP_H__


#include <atomic>
#include "ListaAtomica.hpp"
#include "pthread.h"
#include <fstream>
#include <list>
#include <vector>
#include <iostream>

using namespace std;

typedef pair<string,unsigned int> Elem;

class ConcurrentHashMap {}