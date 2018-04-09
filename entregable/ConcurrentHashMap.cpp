#include "ConcurrentHashmap.hpp"

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

/**************************************** addAndInc *******************************************/
void ConcurrentHashMap::addAndInc(string key){
	int h = hash(key);
	/* Agregamos la clave a la lista si no esta */
	while (!member(key)){
		// Lockeo a nivel de entrada
		pthread_mutex_lock(& _locks[h]);
		if(!member(key)) {
			tabla[h]->push_front(make_pair(key,0));
		}
		pthread_mutex_unlock(& _locks[h]);
		// Desbloqueo a nivel de entrada
	}
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
void * ConcurrentHashMap::max_thread(void * args){
	infoThread* inf = (infoThread*) args;
	int next;
	next = atomic_fetch_add(inf->siguiente,1);
	// Ejecutamos mientras no hayamos calculado el maximo de cada entrada de la tabla
	while(next  < 26 ){
		/* Vamos a la proxima entrada de la tabla para recorrer. */
		for (auto it = tabla[next]->CrearIt(); it.HaySiguiente(); it.Avanzar()) {
			// Recorremos todos los elementos de la lista
			while( (* (inf->max)).load() == NULL || it.Siguiente().second > ((*inf->max).load())->second ){
				// Mientras no haya maximo o el maximo del elemento analizando es mayor al maximo actual
				Elem* m = (* (inf->max)).load();
				atomic_compare_exchange_weak(inf->max, &m , &it.Siguiente());
			}
		}
		next = atomic_fetch_add(inf->siguiente,1);
	}
	/* Si no quedan mas entradas de la tabla para reccorer terminamos. */
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
		pthread_create(&threads[tid],NULL,&ConcurrentHashMap::max_thread,& (vars[tid]) );
	}
	/* Esperamos que todos los threads terminen */
	for (size_t tid = 0; tid < nt; tid++) pthread_join(threads[tid],NULL);
	// Nos creamos el par con el string y el entero que representan el mayor valor
	pair<string,unsigned int> res = make_pair(maximo.load()->first,maximo.load()->second);
	/* Desbloqueamos los mutex de cada entrada de la tabla */
	for (size_t i = 0; i < 26; i++) pthread_mutex_unlock(& _locks[i]);
	return res;
}

void count_words(string archivo,*ConcurrentHashMap h){

	string word;
	char space_delimiter = " ";
	// Abrimos el archivo
	ifstream file(archivo);
	if (file) {
	 while (getline(file,word,space_delimiter)) {
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
	int last = inf.words->size();

	ConcurrentHashMap *h = inf.context;
	next = atomic_fetch_add(inf.siguiente,1);
	// Mientras no haya visto todos los archivos de texto
	while(next < last){
    	string file = (*inf.words)[next];
    	count_words(file,h);
    	next = atomic_fetch_add(inf.siguiente,1);
	}

	return NULL;
}


/**********************Utilizando n threads calcula la cantidad de palabras de los archivos de texto****************/
ConcurrentHashMap count_words(unsigned int n, list<string>files){

  ConcurrentHashMap h;

  // Creao n threads
  pthread_t threads[n];
  int tid;
  // Creo n estructuras de infoFile que van a utilzar los threads
  infoFile vars[n];

  // Me guardo en un vector los nombres de los archivos
  vector<string> files;
  for (auto it = files.begin(); it != files.end(); it++){
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