#include "ConcurrentHashmap.hpp"

using namespace std;


ConcurrentHashMap::ConcurrentHashMap(){
	/* Inizializamos la tabla y los mutex */
	for (size_t i = 0; i < 26; i++) {
		tabla[i] = new Lista<Elem>();
		pthread_mutex_init(&(_locks[i]), NULL);
	}
}

/**************************************** addAndInc *******************************************/
void ConcurrentHashMap::addAndInc(string key){
	int h = hash(key);
	/* Agregarmos la clave a la lista si no esta */
	while (!member(key)){
		pthread_mutex_lock(& _locks[h]);
		if(!member(key)) {
			tabla[h]->push_front(make_pair(key,0));
		}
		pthread_mutex_unlock(& _locks[h]);
	}
	/* Incrementamos su valor */
	for (auto it = tabla[h]->CrearIt(); it.HaySiguiente(); it.Avanzar()) {
		if (it.Siguiente().first == key) {
			pthread_mutex_lock(& _locks[h]);
			it.Siguiente().second++;
			pthread_mutex_unlock(& _locks[h]);
			break;
		}
	}
}

/**************************************** memeber *********************************************/
bool ConcurrentHashMap::member(string key){
	for (auto it = tabla[hash(key)]->CrearIt(); it.HaySiguiente(); it.Avanzar()) {
		if (it.Siguiente().first == key) return true;
	}
	return false;
}

/**********************************************************************************************/
/* maxThrWrapper y maxThr funciones que usa cada threada para calcular el máximo de un hashmap*/
void* ConcurrentHashMap::maxThrWrapper(void * args){
	infoThread * info = (infoThread *) args ;
	return info->context->maxThr(info);
}
void * ConcurrentHashMap::maxThr(void * args){
	infoThread* inf = (infoThread*) args;
	int next;
	while(next = atomic_fetch_add(inf->siguiente,1), next  < 26 ){
		/* Vamos a la proxima entrada de la tabla para recorrer. */
		for (auto it = tabla[next]->CrearIt(); it.HaySiguiente(); it.Avanzar()) {
			while( (* (inf->max)).load() == NULL || it.Siguiente().second > ((*inf->max).load())->second ){
				Elem* m = (* (inf->max)).load();
				atomic_compare_exchange_weak(inf->max, &m , &it.Siguiente());
			}
		}
	}
	/* Si no quedan mas entradas de la tabla para reccorer terminamos. */
	return NULL;
}

/************************************ maximum *************************************************/
pair<string,unsigned int> ConcurrentHashMap::maximum(unsigned int nt){
	atomic<int> siguiente(0);
	atomic<Elem *> maximo(nullptr);
	pthread_t threads[nt]; int tid;
	infoThread vars[nt];
	/* Pedimos los mutex de cada entrada de la tabla */
	for (size_t i = 0; i < 26; i++) pthread_mutex_lock(& _locks[i]);
	/* Inizializamos los threads */
	for (size_t tid = 0; tid < nt; tid++) {
		vars[tid].siguiente = &siguiente;
		vars[tid].max = &maximo;
		vars[tid].context = this;
		pthread_create(&threads[tid],NULL,&ConcurrentHashMap::maxThrWrapper,& (vars[tid]) );
	}
	/* Joineamos los threads */
	for (size_t tid = 0; tid < nt; tid++) pthread_join(threads[tid],NULL);
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

ConcurrentHashMap count_words(string archivo){

	ConcurrentHashMap h;
	count_words(archivo,&h);
	return h;
}

/*********** función que utilizarán los threads para la versión concurrent count_words 2.3*****/
void * count_words_threads(void * args){

	pair<string*,ConcurrentHashMap*> input = *((pair<string*,ConcurrentHashMap*> *) args);
	ConcurrentHashMap *h = input.second;
	string inputFile = *(input.first);
	count_words(inputFile,h);
	return NULL;
}

/************************EJ 2.3 versión concurrente del count_words ***************************/
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


/********* función que utilizarán los n threads para la versión concurrente count_words 2.4****/
void * count_words_nthreads(void * args){

	infoFile inf = *(infoFile*) args;

	int next;
	int last = inf.words->size();

	ConcurrentHashMap *h = inf.context;
	next = atomic_fetch_add(inf.siguiente,1);
	while(next < last){
    	string file = (*inf.words)[next];
    	count_words(file,h);
    	next = atomic_fetch_add(inf.siguiente,1);
	}

	return NULL;
}


/**********************EJ 2.4 versión concurrente del count_words con n threads****************/
ConcurrentHashMap count_words(unsigned int n, list<string>files){

  ConcurrentHashMap h;

  pthread_t threads[n];
  int tid;
  infoFile vars[n];

  vector<string> words;
  for (auto it = files.begin(); it != files.end(); it++){
  	string s = *it;
  	words.push_back(s);
  }
  atomic<int> siguiente(0);


  for(tid = 0; tid < n; tid++){
  	vars[tid].siguiente = &siguiente;
  	vars[tid].words = &words;
  	vars[tid].context = &h;
  	pthread_create(&threads[tid], NULL, count_words_nthreads, & (vars[tid]));
  }

  for(tid = 0; tid < n; tid++){
  	pthread_join(threads[tid], NULL);
  }

  return h;

}