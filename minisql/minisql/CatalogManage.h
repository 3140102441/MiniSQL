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
	vector <Table> tables;                  //table ��Ϣ
	int tablenum;
	vector <Index> indexs;                  //index ��Ϣ
	int indexnum;

	void initialtable();                    //������Ϣ
	void initialindex();
	void storetable();                      //�洢��Ϣ
	void storeindex();
public:
	CatalogManage(){                         //���캯��
		initialtable();
		initialindex();
	}
	~CatalogManage(){                       //��������
		storetable();
		storeindex();
	}

	//��Ա����

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