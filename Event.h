// File: event.h
#ifndef _event_h_
#define _event_h_

typedef unsigned char IVTNo;
#include "IVTE.h"
#include "KEvent.h"
class KernelEv;

class Event {
public:
	Event (IVTNo ivtNo);
	~Event ();
	void wait ();
protected :
	friend class KernelEv;
	void signal(); // can call KernelEv
private:
	KernelEv* myImpl;
};
#endif
