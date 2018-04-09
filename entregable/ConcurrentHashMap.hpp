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
		// Siguiente se usa para llevar registro de las entradas de la tabla analizadas
		atomic<int>* siguiente;
		// Max es el elemento maximo encontrado
		atomic<Elem* > * max;
		// Context es el HashMap compartido que van a usar los threads para analizar
		ConcurrentHashMap* context;
	};

	pthread_mutex_t _locks[26];

	void count_words(string archivo,*ConcurrentHashMap);

	void * count_words_threads(void * args);

	void * max_thread(void * args);

	
    // Función de hash que devuelve el primer caracter del string
	int hash(string s){
		return s[0];
	}

	public: 

		
	/****************** maximum ********************/
	pair<string,unsigned int> maximum(unsigned int nt);
};

/********************* Estructura y funciones para Ej 2.4 ***************************/
struct infoFile {
	infoFile(): siguiente(nullptr), files(nullptr),context(nullptr) {}
	atomic<int>* siguiente;
	vector<string> *files;
	ConcurrentHashMap* context;
};

	ConcurrentHashMap count_words(string archivo);

	ConcurrentHashMap count_words(list<string>archs);

	ConcurrentHashMap count_words(unsigned int n, list<string>files);


#endif /* CONCURRENT_HASH_MAP_H__ */