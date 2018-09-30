/*
 * Lista.h
 *
 *  Created on: Jul 18, 2015
 *      Author: OS1
 */
/*Nijedna od funkcija liste ne omogucava medjusobno iskljucivanje niti*/
#ifndef _LISTA_H_
#define _LISTA_H_
#define NULL 0L
#include "PCB.h"

typedef struct elem{
	PCB* x;
	struct elem *sled;
} Elem;

class Lista{
private:
	void kopiraj(Lista& lista);
	void premesti(Lista& lista);
	void brisi();
public:
	Elem *prvi, *posl;
	Lista(){prvi=posl=NULL;}
	~Lista(){brisi();}
	Lista(Lista& lista){kopiraj(lista);}
	//Lista(Lista&& lista){premesti(lista);}
	Lista& operator=(Lista& lista){if(this!=&lista) {brisi(); kopiraj(lista);} return *this;}
	//Lista& operator=(Lista&& lista){if(this!=&lista) {brisi(); premesti(lista);} return *this;}
	Lista& dodajNaPocetak(PCB* novi);
	Lista& dodajNaKraj(PCB* novi);
	PCB* izbaci(Elem* e);
	PCB* izbaciPoID(ID vrednost);
	void izbaciKojeCekaju(PCB* gotova);
	Lista& dodajIza(PCB* novi, Elem* pret);
};

extern Lista listaBlokiranih,listaUspavanih;
void dodajUUspavane(PCB* novi);
void uredjivanjeUspavanih();
void dodajUBlokirane(PCB* novi);
#endif /* _LISTA_H_ */
