#include "IVTE.h"

IVTEntry* IVTEntry::ulazi[256]={NULL};

IVTEntry::IVTEntry(IVTNo br,int pozvatiStaru, PrekidnaRutina pr){
	LOCK
	if(ulazi[br]) {cout<<"Ulaz "<<br<<" je zauzet!\n"; return;}
	brojUlaza=br;
	this->pozvatiStaru=pozvatiStaru;
	novaRutina=pr;
	dogadjaj=NULL;
#ifndef BCC_BLOCK_IGNORE
	staraRutina=getvect(br);
	if(novaRutina) setvect(brojUlaza,novaRutina);
#endif
	ulazi[brojUlaza]=this;
	UNLOCK
}

IVTEntry::~IVTEntry(){
#ifndef BCC_BLOCK_IGNORE
	LOCK
	setvect(brojUlaza,staraRutina);
	UNLOCK
#endif
	ulazi[brojUlaza]=NULL;
}

void IVTEntry::dodajPrekidnuRutinu(PrekidnaRutina nova){
	novaRutina=nova;
#ifndef BCC_BLOCK_IGNORE
	LOCK
	setvect(brojUlaza,novaRutina);
	UNLOCK
#endif
}
