#include "ConcurrentHashMap.hpp"

using namespace std;

/**************************************** Constructor *******************************************/
/* Crea la tabla de 26 entradas y almacena en cada entrada una nueva Lista de Elem*/
ConcurrentHashMap::ConcurrentHashMap(){
	for (size_t i = 0; i < 26; i++) {
		tabla[i] = new Lista<Elem>();
		// Inicializa los mutex
		pthread_mutex_init(&(_locks[i]), NULL);
	}
}

/**************************************** Destructor ******************************************/
/**** volamos el destructor porque nos cagaba la copia ****/
//ConcurrentHashMap::~ConcurrentHashMap(){
//	for (size_t i = 0; i < 26; i++) {
//		//delete tabla[i];
//		pthread_mutex_destroy(&_locks[i]);
//	}
//}

/**************************************** addAndInc *******************************************/
void ConcurrentHashMap::addAndInc(string key){
	int h = hash(key);
	/* Agregamos la clave a la lista si no esta */
    // Lockeo a nivel de entrada
    pthread_mutex_lock(& _locks[h]);
    if(!member(key)) {
        tabla[h]->push_front(make_pair(key,0));
    }
    pthread_mutex_unlock(& _locks[h]);
    // Desbloqueo a nivel de entrada

	/* Incrementamos su valor */
	for (auto it = tabla[h]->CrearIt(); it.HaySiguiente(); it.Avanzar()) {
		if (it.Siguiente().first == key) {
			// Lockeo a nivel de entrada
			pthread_mutex_lock(& _locks[h]);
			it.Siguiente().second++;
			pthread_mutex_unlock(& _locks[h]);
			// Desbloqueo a nivel de entrada
			break;
		}
	}
}

/**************************************** member *********************************************/
bool ConcurrentHashMap::member(string key){
	for (auto it = tabla[hash(key)]->CrearIt(); it.HaySiguiente(); it.Avanzar()) {
		if (it.Siguiente().first == key) return true;
	}
	return false;
}

/**********************************************************************************************/
/* max_thread es la funcion que usa cada thread para calcular el máximo de un hashmap*/
void * max_thread(void * args){
	infoThread* inf = (infoThread*) args;
	ConcurrentHashMap *h = inf->context;
	int next;
	next = atomic_fetch_add(inf->siguiente,1);
	// Ejecutamos mientras no hayamos calculado el maximo de cada entrada de la tabla
	while(next < 26){
		/* Vamos a la proxima entrada de la tabla para recorrer. */
		for (auto it = h->tabla[next]->CrearIt(); it.HaySiguiente(); it.Avanzar()) {
			// Recorremos todos los elementos de la lista
			while( (* (inf->max)).load() == NULL || it.Siguiente().second > ((*inf->max).load())->second ){
				// Mientras no haya maximo o el maximo del elemento analizando es mayor al maximo actual
				Elem* m = (* (inf->max)).load();
				atomic_compare_exchange_weak(inf->max, &m , &it.Siguiente());
			}
		}
		next = atomic_fetch_add(inf->siguiente,1);
	}
	/* Si no quedan mas entradas de la tabla para recorrer terminamos. */
	return NULL;
}

/************************************ maximum *************************************************/
pair<string,unsigned int> ConcurrentHashMap::maximum(unsigned int nt){
	atomic<int> siguiente(0);
	atomic<Elem *> maximo(nullptr);
	// Creo nt threads
	pthread_t threads[nt]; 
	int tid;
	infoThread vars[nt];
	/* Pedimos los mutex de cada entrada de la tabla */
	for (size_t i = 0; i < 26; i++) pthread_mutex_lock(& _locks[i]);
	/* Inizializacion de la estructura que van a usar los threads */
	for (size_t tid = 0; tid < nt; tid++) {
		vars[tid].siguiente = &siguiente;
		vars[tid].max = &maximo;
		vars[tid].context = this;
		pthread_create(&threads[tid],NULL,max_thread, & (vars[tid]));
	}
	/* Esperamos que todos los threads terminen */
	for (size_t tid = 0; tid < nt; tid++) pthread_join(threads[tid],NULL);
	// Nos creamos el par con el string y el entero que representan el mayor valor
	pair<string,unsigned int> res = make_pair(maximo.load()->first,maximo.load()->second);
	/* Desbloqueamos los mutex de cada entrada de la tabla */
	for (size_t i = 0; i < 26; i++) pthread_mutex_unlock(& _locks[i]);
	return res;
}

void count_words(string archivo,ConcurrentHashMap* h){

	string word;
	char space_delimiter = ' ';
	// Abrimos el archivo
	//cout << "antes de abrir el archivo " << archivo << endl;
	ifstream file(archivo);
	//cout << "despues de abrir el archivo " << archivo << endl;
	if (file.is_open()) {
		//cout << "antes del while" << endl;
		while (getline(file,word,space_delimiter)) {
			//cout << "en el while con la palabra " << word << endl;
	 		// Agregamos cada palabra al hashsmap
	 		h->addAndInc(word);
	 	}
	 	file.close();
 	}
}

/*********** Toma un archivo de texto y devuelve el ConcurrentHashMap cargado con las palabras del archivo. *****/
ConcurrentHashMap count_words(string archivo){

	ConcurrentHashMap h;
	count_words(archivo,&h);
    return h;
}

/*********** Función que utilizan los threads para contar las palabras*****/
void * count_words_threads(void * args){

	pair<string*,ConcurrentHashMap*> input = *((pair<string*,ConcurrentHashMap*> *) args);
	ConcurrentHashMap *h = input.second;
	string inputFile = *(input.first);
	count_words(inputFile,h);
	return NULL;
}

/************************ Toma como parámetro una lista de archivos de texto y devuelva el ConcurrentHashMap cargado con esas palabras ***************************/
ConcurrentHashMap count_words(list<string>files){
  	ConcurrentHashMap h;

  	// Se crea un thread por cada archivo.
  	pthread_t threads[files.size()];
  	pair<string*,ConcurrentHashMap*> vars_for_thread[files.size()];

  	int tid = 0;
  	// List no es accesible con [], por eso el iterador.
  	for (auto it = files.begin(); it != files.end(); it++){
  		vars_for_thread[tid].first = &(*it);
  		vars_for_thread[tid].second = &h;
  		pthread_create(&threads[tid],NULL, count_words_threads, & vars_for_thread[tid]); //
  		tid++;
  	}

  	for (tid = 0; tid < files.size(); tid++) {
    	pthread_join(threads[tid],NULL);
  	}

  	return h;
}


/********* Función que utilizarán los n threads para la versión concurrente count_words****/
void * count_words_nthreads(void * args){

	infoFile inf = *(infoFile*) args;

	int next;
	int last = inf.files->size();

	ConcurrentHashMap *h = inf.context;
	next = atomic_fetch_add(inf.siguiente,1);
	// Mientras no haya visto todos los archivos de texto
	while(next < last){
    	string file = (*inf.files)[next];
    	count_words(file,h);
    	next = atomic_fetch_add(inf.siguiente,1);
	}

	return NULL;
}


/**********************Utilizando n threads calcula la cantidad de palabras de los archivos de texto****************/
ConcurrentHashMap count_words(unsigned int n, list<string> files_list){

  ConcurrentHashMap h;

  // Creao n threads
  pthread_t threads[n];
  int tid;
  // Creo n estructuras de infoFile que van a utilzar los threads
  infoFile vars[n];

  // Me guardo en un vector los nombres de los archivos
  vector<string> files;
  for (auto it = files_list.begin(); it != files_list.end(); it++){
  	string s = *it;
  	files.push_back(s);
  }
  atomic<int> siguiente(0);


  for(tid = 0; tid < n; tid++){
  	vars[tid].siguiente = &siguiente;
  	vars[tid].files = &files;
  	vars[tid].context = &h;
  	pthread_create(&threads[tid], NULL, count_words_nthreads, & (vars[tid]));
  }

  // Esperamos que todos los threads terminen
  for(tid = 0; tid < n; tid++){
  	pthread_join(threads[tid], NULL);
  }

  return h;

}


/**** Función que utilizarán los p_archivos threads para leer los archivos**********/
void * count_words_nthreads_2(void * args){

	infoFileVector inf = *(infoFileVector*) args;

	int next;
	int last = inf.files->size();

	vector<ConcurrentHashMap> *h = inf.hashMaps;
	next = atomic_fetch_add(inf.siguiente,1);
	// Mientras no hayamos contado las palabras de todos los archivos
	while(next < last){
    	string archivo = (*inf.files)[next];
    	ifstream file(archivo);
		string word;
		char space_delimiter = ' ';
		if (file) {
			while(getline(file,word,space_delimiter)){
				(*h)[next].addAndInc(word);
			}
		}
		file.close();
		next = atomic_fetch_add(inf.siguiente,1);
	}

	return NULL;
}


/**** Función que utilizarán los p_máximos threads para calcular el máximo **********/
void * count_row(void * args){
	infoFileFind inf = *(infoFileFind*) args;

	int next;
	vector<ConcurrentHashMap> *h = inf.hashMaps;
	ConcurrentHashMap *hGral = inf.hashMapGral;
	next = atomic_fetch_add(inf.row, 1);
	while(next < 26){
		int i;
		for (i = 0; i < h->size(); i++){
			auto it = (*h)[i].tabla[next]->CrearIt();
			for(auto it = (*h)[i].tabla[next]->CrearIt(); it.HaySiguiente(); it.Avanzar()){
				string key = it.Siguiente().first;
				int j;
				for (j = 0; j < it.Siguiente().second; j++) (*hGral).addAndInc(key);
			}
		}
		next = atomic_fetch_add(inf.row, 1);
	}

	return NULL;
}


/*************************Calcula el maximo usando p_archivos threads para leer los archivos y p_maximos threads para calcular el maximo ************************/

pair<string, unsigned int> maximum(unsigned int p_archivos,unsigned int p_maximos, list<string>archs){


	// Creo un vector con un hashsmap por cada archivo
	int n = archs.size();
	vector<ConcurrentHashMap> hashMaps(n);

	// Creo p_archivos threads
	pthread_t threads[p_archivos];
	int tid;
	// Creo p_archivos estructuras que van a usar los threads
	infoFileVector vars[p_archivos];

	// Creo un vector con los nombres de los archivos
	vector<string> files;
	for (auto it = archs.begin(); it != archs.end(); it++){
		string s = *it;
		files.push_back(s);
	}

	atomic<int> siguiente(0);

	for(tid = 0; tid < p_archivos ; tid++){
		vars[tid].siguiente = &siguiente;
		vars[tid].files = &files;
		vars[tid].hashMaps = &hashMaps;
		pthread_create(&threads[tid], NULL, count_words_nthreads_2, & (vars[tid]));
	}

	// Esperamos que los p_threads terminen
	for(tid = 0; tid < p_archivos; tid++){
		pthread_join(threads[tid], NULL);
	}



	// Creo p_maximos threads en el que cada uno irá construyendo de a una fila un hashmap general
	p_maximos = (p_maximos > 26)? 26 : p_maximos;
	pthread_t threads_find[p_maximos];
	infoFileFind vars2[p_maximos];
	atomic<int> row(0);
	ConcurrentHashMap hashMapGral;

	for(tid = 0; tid < p_maximos; tid++){
		vars2[tid].row =&row;
		vars2[tid].hashMaps = &hashMaps;
		vars2[tid].hashMapGral = &hashMapGral;
		pthread_create(&threads_find[tid], NULL, count_row, & (vars2[tid]));
	}

	// Esperamos que los p_maximos threads terminen
	for(tid = 0; tid < p_maximos; tid++){
		pthread_join(threads_find[tid], NULL);
	}

	// Calculamos el maximo del hashMapGral usando p_maximos threads

	pair<string, unsigned int> max = hashMapGral.maximum(p_maximos);

	return max;

}

/*************************Calcula el maximo usando la version concurrente de countwords ************************/

pair<string, unsigned int>maximum_concurrent(unsigned int p_archivos,unsigned int p_maximos, list<string>archs){
	ConcurrentHashMap h = count_words(p_archivos, archs);
	pair<string, unsigned int> max = h.maximum(p_maximos);
	return max;	
}