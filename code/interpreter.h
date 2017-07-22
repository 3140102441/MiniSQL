#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_
#include  "Minisql.h"
#include "BufferManage.h"
#include  "CatalogManage.h"
#include  <iostream>

class interpreter{
public:
	int  operation;                                     //����
	string tablename;                                   //Ҫ���в�����table name
	string indexname;									//Ҫ���в�����index name
	string colname;
	vector <Condition> conditions;                      //����
	vector <Attribute> column;
	Table  tableinfo;
	Index  indexinfo;
	Row    row;
	int   primarykeypos;
	int   uniquekeypos;
	int   wrongtype;
	vector <Condition> uniqueconditions;
	string filename;

	interpreter(){                                       //���캯��
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
	void analysis(string input);                                    //����
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