#include "SharedArray.hpp"


template<typename T>
void SharedArray<T>::take(bool write) {
	if (write) this->mtx.lock();
	else this->mtx.lock_shared();
}

template<typename T>
void SharedArray<T>::release(bool write) {
	if (write) this->mtx.unlock();
	else this->mtx.unlock_shared();
}
