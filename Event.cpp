#include "Event.h"
#include "PCB.h"

Event::Event(IVTNo ivtNo){
	LOCK
	myImpl=new KernelEv(ivtNo,this);
	IVTEntry::dohvatiUlaz(ivtNo)->dogadjaj=myImpl;
	UNLOCK
}

Event::~Event(){
	delete myImpl;
}

void Event::wait(){
	if(myImpl->nit==running){
		myImpl->val=0;
		while(myImpl->val==0) _dispatch();
	}
}

void Event::signal(){
	myImpl->val=1;
}
