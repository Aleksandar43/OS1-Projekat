/*
 * IVTE.h
 *
 *  Created on: Sep 2, 2015
 *      Author: OS1
 */

#ifndef _IVTE_H_
#define _IVTE_H_
#define NULL 0L
#include <IOSTREAM.H>
#include <DOS.H>
#include "Event.h"
#include "PCB.h"
class Event;

#define PREPAREENTRY(brul,pozvatiStaru)\
	void interrupt rutina##brul(...){\
		IVTEntry *ulaz=IVTEntry::dohvatiUlaz(brul);\
		ulaz->getDogadjaj()->signal();\
		if(pozvatiStaru) ulaz->staraRutina();\
	}\
	IVTEntry entry##brul(brul,pozvatiStaru,&rutina##brul);

typedef void interrupt (*PrekidnaRutina)(...);

class IVTEntry{
	friend class KernelEv;
	friend class Event;
private:
	IVTNo brojUlaza;

	int pozvatiStaru;
	KernelEv *dogadjaj;
	static IVTEntry* ulazi[256];
public:
	IVTEntry(IVTNo br,int pozvatiStaru=0, PrekidnaRutina pr=NULL);
	~IVTEntry();
	static IVTEntry* dohvatiUlaz(IVTNo brul){return ulazi[brul];}
	void dodajPrekidnuRutinu(PrekidnaRutina nova);
	KernelEv* getDogadjaj(){return dogadjaj;}
	PrekidnaRutina staraRutina,novaRutina;
};

#endif /* _IVTE_H_ */
