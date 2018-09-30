/*
 * KSem.h
 *
 *  Created on: Aug 11, 2015
 *      Author: OS1
 */

#ifndef _KSEM_H_
#define _KSEM_H_
#include "Lista.h"

class KernelSem{
	friend class Semaphore;
private:
	int val;
	Lista red;
public:
	KernelSem(int init):red(){
		val=init;
	}
	int getVal(){return val;}
	void setVal(int newval){val=newval;}
};

#endif /* _KSEM_H_ */
