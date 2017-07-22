#ifndef _INDEX_H_
#define _INDEX_H_
#include "BufferManage.h"
#include "Minisql.h"
#include <list>
#define POINTERLENGTH 4
extern BufferManager buf;

class IndexLeaf	//One pane in the IndexLeaf
{
public:
	string key;
	int type;
	int File_Offset;
	int Block_Offset;
	IndexLeaf():
		key(""), File_Offset(0), Block_Offset(0){}
	IndexLeaf(string k, int t, int FileOff, int BlockOff) :
		key(k), type(t), File_Offset(FileOff), Block_Offset(BlockOff) {}
};

class IndexBranch	//One pane in the Branch
{
public:
	string key;
	int type;
	int ptrChild;	//block pointer,the blockNum of the Child
	IndexBranch():
		key(""), ptrChild(0){}
	IndexBranch(string k, int t, int ptrC) :
		key(k), type(t), ptrChild(ptrC) {}
};

class BPlusTree	//base class of B+ Tree
{
public:
	bool isRoot;
	int bufferNum;		//Number of buffer --> the position in the buffer
	int ptrFather;		//block pointer, root -> useless
	int recordNum;		//One block has the Number of the record(IndexBranch/IndexLeaf)
	int columnLength;	//the Length of Index_Attribution 
	BPlusTree(){}
	BPlusTree(int NbufNum): bufferNum(NbufNum), recordNum(0){}
	int GetPtr(int pos);
	int GetRecordNum();
};


class Branch: public BPlusTree	//Branch in B+ Tree
{
public:
	list<IndexBranch> nodelist;
	Branch(){}
	Branch(int NbufNum): BPlusTree(NbufNum){}//this is for new added brach
	Branch(int NbufNum, const Index& indexinfor); 
	~Branch();
	void insert(IndexBranch node);
	IndexBranch pop()
	{
		recordNum--;
		IndexBranch tmpt = nodelist.back();
		nodelist.pop_back();
		return tmpt;
	}
	IndexBranch getfront()
	{
		return nodelist.front();
	}
};

class Leaf: public BPlusTree
{
public:	
	int nextSibling;	//block pointer
	int lastSibling;	//block pointer
	list<IndexLeaf> nodelist;
	Leaf(int NbufNum){	//this kind of leaf is added when old leaf is needed to be splited
		bufferNum = NbufNum;
		recordNum = 0;
		nextSibling = lastSibling = 0;
	}
	Leaf(int NbufNum, const Index& indexinfor);
	~Leaf();

	void insert(IndexLeaf node); 
	IndexLeaf pop()
	{
		recordNum--;
		IndexLeaf tmpt = nodelist.back();
		nodelist.pop_back();
		return tmpt;
	}
	IndexLeaf getfront()
	{
		return nodelist.front();
	}
};

class IndexManager
{
public:
	void createIndex(const Table& tableinfor, Index& indexinfor);	//Create index
	void creatValue(const Table& tableinfor, Index& indexinfor);	//Create a IndexLeaf and insert it
	IndexBranch insertValue(Index& indexinfor, IndexLeaf node, int blockOffset = 0);	//Insert one index_record into index
	Data select(const Table& tableinfor, const Index& indexinfor, vector<int> num, vector<Condition> op, int  blockOffset = 0);
	void dropIndex(Index& indexinfor)
	{
		string filename = indexinfor.index_name + ".index";
		if (remove(filename.c_str()) != 0)
			perror("Error deleting file");
		else
			buf.deleteFile(filename);
	}
	void deleteValue() {}
private:
	Row splitRow(Table tableinfor, string row);	//Get the row which index finding
	string getColumnValue(const Table& tableinfor, const Index& indexinfor, string row);
};

#endif
