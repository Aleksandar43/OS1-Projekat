/*
 * Lista.cpp
 *
 *  Created on: Jul 18, 2015
 *      Author: OS1
 */

#include "Lista.h"

Lista listaBlokiranih,listaUspavanih;

void Lista::kopiraj(Lista& lista){
	for(Elem *tek=lista.prvi; tek!=NULL; tek=tek->sled){
		dodajNaKraj(tek->x);
	}
}

void Lista::premesti(Lista& lista){
	prvi=lista.prvi;
	posl=lista.posl;
	lista.posl=lista.prvi=NULL;
}

void Lista::brisi(){
	Elem *tek=prvi;
	while(tek!=NULL){
		prvi=prvi->sled;
		delete tek;
		tek=prvi;
	}
	prvi=posl=NULL;
}

Lista& Lista::dodajNaPocetak(PCB* novi){
	Elem *e=new Elem();
	e->x=novi;
	e->sled=prvi;
	prvi=e;
	if(posl==NULL) posl=prvi;
	return *this;
}

Lista& Lista::dodajNaKraj(PCB* novi){
	Elem *e=new Elem();
	e->x=novi;
	e->sled=NULL;
	if(prvi!=NULL) posl->sled=e;
	else prvi=e;
	posl=e;
	return *this;
}

PCB* Lista::izbaci(Elem* e){
	PCB* vrati=NULL;
	if(prvi==NULL) return NULL;
	Elem *pret=NULL,*tek=prvi;
	if(e==prvi){
		vrati=prvi->x;
		prvi=prvi->sled;
		delete tek;
		if(posl==e) posl=pret;
	}
	else{
		if(prvi->sled==NULL) return NULL;
		pret=prvi;
		tek=prvi->sled;
		while(tek && tek!=e){
			pret=tek;
			tek=tek->sled;
		}
		if(tek){
			vrati=tek->x;
			pret->sled=tek->sled;
			delete tek;
			if(posl==e) posl=pret;
		}
	}
	return vrati;
}

PCB* Lista::izbaciPoID(ID vrednost){//izbacuje 1. na koji naidje
	Elem *tek=prvi;
	PCB* vrati=NULL;
	while(tek && tek->x->id!=vrednost) tek=tek->sled;
	if(tek){
		vrati=tek->x;
		izbaci(tek);
	}
	return vrati;
}

void Lista::izbaciKojeCekaju(PCB* gotova){
	Elem *tek=prvi;
	while(tek){
		if(tek->x->nitNaKojuCeka==gotova){
			tek->x->stanje=RADI;
			putToSch(tek->x);
			Elem *out=tek;
			tek=tek->sled;
			brojBlokiranihNiti--;
			izbaci(out);
		}
		else tek=tek->sled;
	}
}

Lista& Lista::dodajIza(PCB* novi, Elem* pret){
	Elem *e=new Elem();
	e->x=novi;
	e->sled=pret->sled;
	pret->sled=e;
	if(pret==posl) posl=e;
	return *this;
}

void dodajUUspavane(PCB* novi){
	if(listaUspavanih.prvi==NULL) listaUspavanih.dodajNaKraj(novi);
	else{
		Elem *tek=listaUspavanih.prvi, *pret=NULL;
		while(tek && novi->vremeZaSpavanje > tek->x->vremeZaSpavanje){
			novi->vremeZaSpavanje-=tek->x->vremeZaSpavanje;
			pret=tek;
			tek=tek->sled;
		}
		if(tek==listaUspavanih.prvi) listaUspavanih.dodajNaPocetak(novi);
		else listaUspavanih.dodajIza(novi,pret);
	}
	brojUspavanihNiti++;
}

void uredjivanjeUspavanih(){
	if(listaUspavanih.prvi==NULL) return;
	listaUspavanih.prvi->x->vremeZaSpavanje--;
	while(listaUspavanih.prvi && listaUspavanih.prvi->x->vremeZaSpavanje==0){
		PCB* e=listaUspavanih.izbaci(listaUspavanih.prvi);
		e->stanje=RADI;
		putToSch(e);
		brojUspavanihNiti--;
	}
}

void dodajUBlokirane(PCB* novi){
	listaBlokiranih.dodajNaKraj(novi);
	brojBlokiranihNiti++;
}
