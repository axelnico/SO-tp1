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

class ConcurrentHashMap {

	private:

		/* Info que le pasamos a un thread para calcular el maximo */
	struct infoThread {
		infoThread(): siguiente(nullptr),max(nullptr),context(nullptr) {}
		atomic<int>* siguiente;
		atomic<Elem* > * max;
		ConcurrentHashMap* context;
	};

	pthread_mutex_t _locks[26];

	/***********************************************/
	static void* maxThrWrapper(void * args);

	void * maxThr(void * args);

	

		int hash(string s){
			return s[0];
		}

	public: 

		
	/****************** maximum ********************/
	pair<string,unsigned int> maximum(unsigned int nt);
};

#endif /* CONCURRENT_HASH_MAP_H__ */