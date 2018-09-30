#include "Semaphor.h"
#include "PCB.h"
void putToSch(PCB* pcb);

Semaphore::Semaphore(int init){
	LOCK
	myImpl=new KernelSem(init);
	UNLOCK
}

Semaphore::~Semaphore(){
	delete myImpl;
}

void Semaphore::wait(){
	LOCK // da se running ne promeni
	myImpl->val--;
	if(myImpl->val<0){
		running->stanje=SEMBLOK; //da li brojati niti na semaforima?
		myImpl->red.dodajNaKraj(running);
		_dispatch();
	}
	UNLOCK
}

void Semaphore::signal(){
	LOCK
	if(myImpl->val<0){
		PCB* e=myImpl->red.izbaci(myImpl->red.prvi);
		e->stanje=RADI;
		putToSch(e);
	}
	myImpl->val++;
	UNLOCK
}

int Semaphore::val() const{
	return myImpl->val;
}
