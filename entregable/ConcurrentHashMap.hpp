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

		pthread_mutex_t _locks[26];

		void count_words(string archivo,ConcurrentHashMap* map);

		void * count_words_threads(void * args);
		
	    // Función de hash que devuelve el primer caracter del string
		int hash(string s){
			return ((int) s[0] % 26);
		}

	public:

		Lista<Elem >* tabla[26];

		/***************** Constructor *****************/
		ConcurrentHashMap();

		/*************** Destructor ********************/
		//~ConcurrentHashMap();

		/****************** addAndInc ******************/
		void addAndInc(string key);

		/****************** memeber ********************/
		bool member(string key);
			
		/****************** maximum ********************/
		pair<string,unsigned int> maximum(unsigned int nt);
};

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

void * max_thread(void * args); // Funcion usada para maximum de ConcurrentHashMap

/********************* Estructuras usadas por los threads ***************************/
struct infoFile {
	infoFile(): siguiente(nullptr), files(nullptr),context(nullptr) {}
	atomic<int>* siguiente;
	vector<string> *files;
	ConcurrentHashMap* context;
};

struct infoFileVector {
	infoFileVector(): siguiente(nullptr), files(nullptr),hashMaps(nullptr) {}
	atomic<int>* siguiente;
	vector<string> *files;
	vector<ConcurrentHashMap>* hashMaps;
};
struct infoFileFind {
	infoFileFind(): row(nullptr), hashMaps(nullptr), hashMapGral(nullptr) {}
	atomic<int>* row;
	vector<ConcurrentHashMap>* hashMaps;
	ConcurrentHashMap* hashMapGral;
};

ConcurrentHashMap count_words(string archivo);

ConcurrentHashMap count_words(list<string>archs);

ConcurrentHashMap count_words(unsigned int n, list<string> files_list);

void * count_row(void * args);

pair<string, unsigned int>maximum(unsigned int p_archivos,unsigned int p_maximos, list<string>archs);

pair<string, unsigned int>maximum_concurrent(unsigned int p_archivos,unsigned int p_maximos, list<string>archs);

#endif /* CONCURRENT_HASH_MAP_H__ */