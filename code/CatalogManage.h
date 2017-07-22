#ifndef __CatalogManage_h__
#define __CatalogManage_h__

#include "Minisql.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

class CatalogManage{
private:
	vector <Table> tables;                  //table 信息
	int tablenum;
	vector <Index> indexs;                  //index 信息
	int indexnum;

	void initialtable();                    //读入信息
	void initialindex();
	void storetable();                      //存储信息
	void storeindex();
public:
	CatalogManage(){                         //构造函数
		initialtable();
		initialindex();
	}
	~CatalogManage(){                       //析构函数
		storetable();
		storeindex();
	}

	//成员函数

	void createtable(Table& tableinfo);                        
	bool isexisttable(string tablename);
	void createindex(Index& indexinfo);
	bool existindex(string indexname);
	bool existindex(string tablename, int colnum);
	Table gettable(string tablename);
	int  getcolunmnum(Table tableinfo, string word);
	void update(Index indexinfo);
	void update(Table tableinfo);
	void droptable(Table tableinfo);
	Index getindexinfo(string indexname);
	Index getindexinfo(Table tableinfo, int colnum);
	void dropindex(string indexname);

};

#endif