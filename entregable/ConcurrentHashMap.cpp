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