/*
 * KEvent.h
 *
 *  Created on: Aug 30, 2015
 *      Author: OS1
 */

#ifndef _KEVENT_H_
#define _KEVENT_H_
#include "Event.h"
#include "IVTE.h"
#include "PCB.h"

class KernelEv{
	friend class Event;
private:
	int val;
	IVTNo brojUlaza;
	PCB* nit;
	Event* event;
public:
	KernelEv(IVTNo ivtNo, Event* e){
		brojUlaza=ivtNo;
		val=0;
		nit=running;
		event=e;
	}
	void signal(){val=1;}
};

#endif /* _KEVENT_H_ */
