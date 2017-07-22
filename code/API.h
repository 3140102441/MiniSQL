#ifndef _API_H
#define _API_H

#include "Minisql.h"
#include "CatalogManage.h"
#include "interpreter.h"
#include "BufferManage.h"
#include "IndexManager.h"
#include "RecordManager.h"
#include <iostream>

using namespace std;

class API{
public:
	bool isdone;                                             //�жϲ����Ƿ�Ϊ quit
	void execute(interpreter interfaces);                    //ִ�н������ָ��

	API(){
		isdone = false;
	}
};

#endif