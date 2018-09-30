#include "Thread.h"
#include "Lista.h"

void Thread::start(){
	myPCB->start();
}

Thread::Thread (StackSize stackSize, Time timeSlice){
	myPCB=allocatePCB(this,NULL,stackSize,timeSlice);
}

Thread::~Thread(){
	waitToComplete();
	delete myPCB;
}

void dispatch(){
	_dispatch();
}

void Thread::waitToComplete(){
	if(myPCB->stanje!=NEZAPOCETA && myPCB->stanje!=ZAVRSENA){
		LOCK
		running->stanje=BLOKIRANA;
		running->nitNaKojuCeka=myPCB;
		UNLOCK
	_dispatch();
	}
}

void Thread::sleep(Time timeToSleep){
	LOCK
	running->stanje=SPAVA;
	running->vremeZaSpavanje=timeToSleep;
	UNLOCK
	_dispatch();
}
