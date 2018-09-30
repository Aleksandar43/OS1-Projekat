#include <iostream.h>
#include "PCB.h"
#include "Lista.h"
#include "Thread.h"
#include <DOS.H>
#include "Event.h"

int userMain(int argc,char* argv[]);
void interrupt timer(...);
void interrupt (*oldTimer)(...);
extern unsigned timerCount;
extern unsigned broj_promena;
int izlaz; //povratna vrednost za main

class UserMainThread:public Thread{
private:
	int argc;
	char **argv;
public:
	UserMainThread (int argc, char* argv[], StackSize stackSize = defaultStackSize,
					Time timeSlice = defaultTimeSlice) :Thread(stackSize,timeSlice){
		this->argc=argc;
		this->argv=argv;
		//myPCB->setIme("UserMainThread");
	}
	void run(){
		izlaz=userMain(argc,argv);
	}
};

class IdleThread:public Thread{
public:
	void run(){
		// prilikom provere kraja, samo mainThread treba da ostane blokirana
		while(brojBlokiranihNiti>1 || brojNitiUSch>0 || brojUspavanihNiti>0) _dispatch();
	}
};

int main(int argc,char* argv[]){
	LOCK
#ifndef BCC_BLOCK_IGNORE
	oldTimer=getvect(8);
	setvect(8,&timer);
	setvect(0x80,oldTimer);
#endif
	PCB mainPCB(NULL,"mainPCB");
	mainPCB.stanje=RADI;
	running=&mainPCB;
	UserMainThread umt(argc,argv);
	IdleThread idleThread;
	idleThread.start();
	umt.start();
	UNLOCK
	idleThread.waitToComplete();
#ifdef ISPIS
	LOCK
	cout<<"Kontekst promenjen "<<broj_promena<<" puta\n";
	cout<<"timer je otkucao "<<timerCount<<" puta\n";
	cout<<"izlaz="<<izlaz<<"\n";
	cout<<"--------Happy end----------\n";
	UNLOCK
#endif
	mainPCB.stanje=ZAVRSENA;
	LOCK
#ifndef BCC_BLOCK_IGNORE
	setvect(8,oldTimer);
#endif
	UNLOCK
	return izlaz;
}
