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