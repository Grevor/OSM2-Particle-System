/*
 * ListFunctions.h
 *
 *  Created on: 1 maj 2014
 *      Author: Erik
 */

#ifndef LISTFUNCTIONS_H_
#define LISTFUNCTIONS_H_

#include <list>
#include <iterator>

namespace Particle {

template<typename T> bool contains(std::list<T>* list, T element) {
	for(typename std::list<T>::iterator iter = list->begin(); iter != list->end(); iter++) {
		if(*iter == element) {
			return true;
		}
	}
	return false;
}

template<typename T> bool addToList(std::list<T>* list, T obj) {
		if(contains(list, obj)) {
			list->push_front(obj);
			return true;
		}
		return false;
	}

	template<typename T> bool removeFromList(std::list<T>* list, T obj) {
		for(typename std::list<T>::iterator iter = list->begin(); iter != list->end(); iter++) {
			if(*iter == obj) {
				list->erase(iter);
				return true;
			}
		}
		return false;
	}

	template<typename T> void deleteList(std::list<T*>* list) {
		for(typename std::list<T*>::iterator iter = list->begin(); iter != list->end(); iter++) {
			delete *iter;
		}
	}
}



#endif /* LISTFUNCTIONS_H_ */
