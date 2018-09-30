/*
 * PCB.cpp
 *
 *  Created on: May 3, 2015
 *      Author: OS1
 */

#include "PCB.h"
#include "Lista.h"

PCB *running;
ID PCB::autoid=0;
volatile int lockFlag=0; // 0 - otkljucano
volatile long brojNitiUSch=0;
volatile long brojBlokiranihNiti=0;
volatile long brojUspavanihNiti=0;
FunkcijaBezArg fba;
FunVoidVoid fvv=PCB::IzvrsavacNiti;
/* privremene promenljive za dispatch() */
volatile unsigned tax,tbx,tcx,tdx,tsi,tdi,tcs,tds,tes,tss,tsp,tbp,tpc,tpsw;
#ifdef ISPIS
volatile unsigned timerCount=0;
volatile unsigned broj_promena=0;
#endif
volatile int u_rutini=0;
volatile int zahtevana_promena_konteksta=0;
volatile int vreme_isteklo=0;
volatile Time kvant=defaultTimeSlice;
void tick();

PCB::~PCB() {
	waitToComplete();
#ifdef ISPIS
	LOCK
	cout<<"Nit "<<ime<<" se unistava\n";
	UNLOCK
#endif
	delete []stack;
	delete []ime;
}

PCB::PCB(Thread* t, char *name, StackSize stackSize, Time timeSlice) {
	this->stackSize=stackSize>0?stackSize:defaultStackSize;
	LOCK
	stack=new unsigned[this->stackSize];
	UNLOCK
	this->timeSlice=timeSlice;
	thread=t;
	id=++autoid;
	ax=bx=cx=dx=ds=es=si=di=0;
	stanje=NEZAPOCETA;
	vremeZaSpavanje=0;
	nitNaKojuCeka=0;
	procesZakljucan=0;
	psw=0x200;
#ifndef BCC_BLOCK_IGNORE
	sp=bp=FP_OFF(stack+stackSize);
	ss=FP_SEG(stack+stackSize);
	pc=FP_OFF(fvv);
	cs=FP_SEG(fvv);
#endif
	if(name!=NULL){
		LOCK
		ime=new char[strlen(name)+1];
		strcpy(ime,name);
		UNLOCK
	}
	else {
		LOCK
		ime=new char[11]; // "Thread-" + 4 cifre
		sprintf(ime,"Thread-%d",id);
		UNLOCK
	}
#ifdef ISPIS
	LOCK
	cout<<"Nit "<<ime<<" kreirana\n";
	UNLOCK
#endif
}

PCB* allocatePCB(Thread* t, char *name, StackSize stackSize, Time timeSlice){
	LOCK
	PCB* pcb=new PCB(t,name,stackSize,timeSlice);
	UNLOCK
	return pcb;
}

PCB& PCB::setIme(const char* name){
	if(ime!=NULL) delete []ime;
	LOCK
	ime=new char[strlen(name)+1];
	strcpy(ime,name);
	UNLOCK
	return *this;
}

void putToSch(PCB* pcb){
	//LOCK
	Scheduler::put(pcb);
	brojNitiUSch++;
	//UNLOCK
}

PCB* getFromSch(){
	//LOCK
	PCB* out=Scheduler::get();
	brojNitiUSch--;
	//UNLOCK
	return out;
}

// Svaki PCB ima kao pocetnu adresu 1. instr. ove funkcije
void PCB::IzvrsavacNiti(){
	//running->stanje=RADI;
	running->thread->run();
	running->exitThread();
}

void PCB::start(){
	if(stanje!=NEZAPOCETA){
		LOCK
		cout<<"Nit "<<ime<<" vec zapoceta!\n";
		UNLOCK
	}
	else{
		stanje=RADI;
		putToSch(this);
#ifdef ISPIS
		LOCK
		cout<<"Nit "<<ime<<" startovana\n";
		UNLOCK
#endif
	}
}

void PCB::exitThread(){
	stanje=ZAVRSENA;
	LOCK
#ifdef ISPIS
	cout<<"Nit "<<ime<<" zavrsena\n";
#endif
	UNLOCK
	listaBlokiranih.izbaciKojeCekaju(this);
	_dispatch();
	//UNLOCK
}

void PCB::waitToComplete(){
	if(stanje==NEZAPOCETA) return;
	while(!(stanje==ZAVRSENA)) _dispatch();
}

void _dispatch(){
	//LOCK
	zahtevana_promena_konteksta=1;
#ifndef BCC_BLOCK_IGNORE
	asm int 8h //timer
#endif
	//UNLOCK
}

void interrupt timer(...){
	if(u_rutini) return;
	u_rutini=1;
	//if(zahtevana_promena_konteksta) uredjivanjeUspavanih();
	if(!zahtevana_promena_konteksta){
		 uredjivanjeUspavanih();
#ifdef ISPIS
		timerCount++;
#endif
#ifndef BCC_BLOCK_IGNORE
		asm int 80h //oldTimer
#endif
		tick();
		if(running->timeSlice!=0){
			kvant--;
			//if(kvant<0) kvant=0; //Unreachable code
			if(kvant==0) vreme_isteklo=1;
		}
	}
	if(zahtevana_promena_konteksta || (vreme_isteklo && lockFlag==0)){
			zahtevana_promena_konteksta=0;
			/*if(running->stanje==RADI){*/
/*Na steku se redom nalaze psw,cs,ip,ax,bx,cx,dx,es,ds,si,di,bp; ss i sp prepisati*/
	#ifndef BCC_BLOCK_IGNORE
		asm{
			pop tbp
			pop tdi
			pop tsi
			pop tds
			pop tes
			pop tdx
			pop tcx
			pop tbx
			pop tax
			pop tpc
			pop tcs
			pop tpsw
			mov tss,ss
			mov tsp,sp
		}
	#endif
			running->ax=tax;
			running->bp=tbp;
			running->bx=tbx;
			running->cs=tcs;
			running->cx=tcx;
			running->di=tdi;
			running->ds=tds;
			running->dx=tdx;
			running->es=tes;
			running->pc=tpc;
			running->si=tsi;
			running->sp=tsp;
			running->ss=tss;
			running->psw=tpsw;
			running->procesZakljucan=lockFlag;
			switch(running->stanje){
			case RADI: putToSch(running); break;
			case SPAVA: dodajUUspavane(running); break;
			case BLOKIRANA: dodajUBlokirane(running); break;
			case ZAVRSENA: case SEMBLOK: break;
			}
			running=getFromSch();
			kvant=running->timeSlice;
			vreme_isteklo=0;
			lockFlag=running->procesZakljucan;
			tax=running->ax;
			tbx=running->bx;
			tcx=running->cx;
			tdx=running->dx;
			tsi=running->si;
			tdi=running->di;
			tcs=running->cs;
			tds=running->ds;
			tes=running->es;
			tss=running->ss;
			tbp=running->bp;
			tsp=running->sp;
			tpc=running->pc;
			tpsw=running->psw;
#ifndef BCC_BLOCK_IGNORE
	asm{
		mov sp,tsp
		mov ss,tss
		push tpsw
		push tcs
		push tpc
		push tax
		push tbx
		push tcx
		push tdx
		push tes
		push tds
		push tsi
		push tdi
		push tbp
	}
#endif
#ifdef ISPIS
			broj_promena++;
			//cout<<"//Promena konteksta "<<broj_promena<<". put\\\\\n";
#endif
		} // promena konteksta

/*	 else{
		 uredjivanjeUspavanih();
#ifdef ISPIS
			timerCount++;
#endif
#ifndef BCC_BLOCK_IGNORE
	asm int 80h //oldTimer
#endif
	 }*/
	//tick();
	u_rutini=0;
}
