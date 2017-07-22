#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_
#include  "Minisql.h"
#include "BufferManage.h"
#include  "CatalogManage.h"
#include  <iostream>

class interpreter{
public:
	int  operation;                                     //操作
	string tablename;                                   //要进行操作的table name
	string indexname;									//要进行操作的index name
	string colname;
	vector <Condition> conditions;                      //条件
	vector <Attribute> column;
	Table  tableinfo;
	Index  indexinfo;
	Row    row;
	int   primarykeypos;
	int   uniquekeypos;
	int   wrongtype;
	vector <Condition> uniqueconditions;
	string filename;

	interpreter(){                                       //构造函数
		operation = UNKNOW;
		tablename = indexname = colname = "";
		colname = "";
		primarykeypos = uniquekeypos = -1;
		wrongtype = -1;

	}
	void initcol();                                          //initial
	void initcond();
	void initvalue();
	void inittable(){
		tableinfo.name = "";
		tableinfo.attriNum= tableinfo.blockNum= tableinfo.totalLength = 0;
		tableinfo.attributes.clear();
	}
	void initindex(){
		indexinfo.blockNum = indexinfo.columnNum= -1;
		indexinfo.index_name = filename = "";
	}
	void analysis(string input);                                    //解析
    bool cutword(string &input, string &word);
	bool cutstr(string &input, string &word);
	void MakeInitilate(){
		operation = UNKNOW;
		tablename = "";
		indexname = "";
		initcol();
		initcond();
		initvalue();
		inittable();
		initindex();
	}
};




#endif 