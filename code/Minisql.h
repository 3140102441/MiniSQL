#pragma once
#include <string>
#include <iostream>
#include <vector>
#define UNKNOW        0 
#define EMPTY		  1
#define QUIT		  2
#define CREATETABLE   3
#define CREATETABLERR 4
#define CREATEINDEXRR 44
#define TABLEEXIST    5
#define INT           6
#define CHAR          7
#define FLOAT         8
#define INTLENGTH     11
#define FLOATLENGTH   20    
#define CHARERROR     9
#define NOKEY        10
#define EXISIINDEX    12
#define NOTABLE       13
#define COLUNMERROR   14
#define CREATINDEX    15
#define DROPTABLE     16
#define NOINDEX       17
#define DROPINDEX     18
#define SELECTRR      19
#define SELECTALL     20
#define NOCOLUMN      21
#define SELECT        22
#define INSERTRR      23
#define INSERTSIZERR  24
#define INSERT        25
#define INSERTTYPERR  26
#define DELETERR      27
#define DELETE        29
#define NOPRIMARYKEY  30
#define EXISTINDEXNAME 31
#define EXECRR        32
#define EXEC          33

#define MAXBLOCKNUM 1024
#define MAXVALUELEN 1000
#define BLOCKSIZE 4096
#define NOTFOUND -1
#define SUCCESS 1
#define UNSUCCESS 0
#define NIL '@'
#define NOTNIL '$'
#define INT 0
#define FLOAT 1
#define CHAR 2

using namespace std;
class Attribute
{
public:
	string name;
	int type;
	int length;
	bool isPrimeryKey;
	bool isUnique;
	Attribute()
	{
	 isPrimeryKey=false;
	 isUnique=false;
	}
	Attribute(string n, int t, int l, bool isP, bool isU)
		:name(n), type(t), length(l), isPrimeryKey(isP), isUnique(isU){}
};

class Table
{
public:
	string name;   //all the datas is store in file name.table
	int blockNum;	//number of block the datas of the table occupied in the file name.table
	//int recordNum;	//number of record in name.table
	int attriNum;	//the number of attributes in the tables
	int totalLength;	//total length of one record, should be equal to sum(attributes[i].length)
	vector<Attribute> attributes;
	Table(): blockNum(0), attriNum(0), totalLength(0){}
};

class Index
{
public:
	string index_name;	//all the datas is store in file index_name.index
	string table_name;	//the name of the table on which the index is create
	int columnNum;		//on which column the index is created
	int columnLength;
	int blockNum;		//number of block the datas of the index occupied in the file index_name.table
	int type;
	Index(): columnNum(0), blockNum(0){}
};

class Col {
public:
	string Value;
	int type;
	Col() {
		Value = "";
	}
};
class Row
{
public:
	vector<Col> columns;
	void Initial()
	{
		this->columns.clear();
	}
};
class Data//这样就把Data搞成了一个二维数组
{
public:
	vector<Row> rows;
};

enum Comparison { LessThan, LessEqual, GreatThan, GreatEqual, Equal, NotEqual };
//stants for less than, less equal, greater than, greater equal, equal, not equal respectivly
class Condition{
public:
	Comparison op;
	int columnNum;
	string value;
};

class Block {
private:
public:
	bool inUse;
	int LRUValue;
	string FileName;
	int BlockOffset;
	char Values[BLOCKSIZE + 1];

	Block() {
		initialize();
	}

	void initialize() {
		inUse = 0;
		FileName = "NULL";
		memset(Values, NIL, sizeof(Values));
		Values[BLOCKSIZE] = '\0';
	}

	string getValues(const int startpos, const int length) {
		char temp[MAXVALUELEN];
		memcpy(temp, Values + startpos, sizeof(char)*length);
		temp[length] = '\0';
		string ans = string(temp);
		return ans;
	}

	char getValue(const int pos) {
		return Values[pos];
	}

	void setValue(const int pos, const char ch) {
		Values[pos] = ch;
	}

	void setValues(const int pos, const string str) {
		strncpy(Values + pos, str.c_str(),str.length());
	}

};
