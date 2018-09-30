/*
 * PCB.h
 *
 *  Created on: May 3, 2015
 *      Author: OS1
 */

#ifndef _PCB_H_
#define _PCB_H_
#include <DOS.H>
#include <IOSTREAM.H>
#include <STRING.H>
#include <STDIO.H>
#include "Thread.h"

#define LOCK lockFlag++;
#define UNLOCK if(lockFlag>0) lockFlag--;

#define ISPIS

class Thread;

typedef void (Thread::*FunkcijaBezArg)();
extern FunkcijaBezArg fba;
typedef void (*FunVoidVoid)(void);
extern FunVoidVoid fvv;
extern volatile long brojBlokiranihNiti;
extern volatile long brojUspavanihNiti;
enum StanjeNiti{NEZAPOCETA, RADI, BLOKIRANA, SEMBLOK, SPAVA, ZAVRSENA};

class PCB {
	friend class Thread;
private:
	//PCB(const PCB&){}
	//void operator=(PCB& p){}
	static ID autoid;
public:
	unsigned sp;
	unsigned bp;
	unsigned pc;
	unsigned ax;
	unsigned bx;
	unsigned cx;
	unsigned dx;
	unsigned cs;
	unsigned ds;
	unsigned es;
	unsigned ss;
	unsigned si;
	unsigned di;
	unsigned psw;
	StackSize stackSize;
	Time timeSlice; //kvant
	Thread* thread;
	unsigned *stack;
	ID id;
	StanjeNiti stanje;
	char *ime;
	int procesZakljucan;
	Time vremeZaSpavanje;
	PCB* nitNaKojuCeka;
	virtual ~PCB();
	PCB(Thread* t, char *name=NULL, StackSize stackSize=defaultStackSize,Time timeSlice=defaultTimeSlice);
	PCB& setIme(const char* name);
	static void IzvrsavacNiti();
	void start();
	void exitThread();
	void waitToComplete();
};

extern PCB *running;
extern volatile int lockFlag;
extern volatile long brojNitiUSch;
extern volatile long brojBlokiranihNiti;
extern volatile long brojUspavanihNiti;
void putToSch(PCB*);
PCB* getFromSch();
void _dispatch();
PCB* allocatePCB(Thread* t, char *name=NULL, StackSize stackSize=defaultStackSize,Time timeSlice=defaultTimeSlice);
/*void inic();
void restore();*/

#endif /* _PCB_H_ */
