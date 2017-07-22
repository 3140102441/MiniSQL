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
	bool isdone;                                             //判断操作是否为 quit
	void execute(interpreter interfaces);                    //执行解析后的指令

	API(){
		isdone = false;
	}
};

#endif