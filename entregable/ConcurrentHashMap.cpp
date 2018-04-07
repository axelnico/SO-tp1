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