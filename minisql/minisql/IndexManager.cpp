#include "IndexManager.h"
/*
*function	: compare object A and B
*parm		: string	A;		(Object A)
*			  string	B;		(Object B)
*			  int		type;	(The type of A and B --> char / int / float )
*			  Comparison op;	(opration > / >= / < / <= / == / !=)
*return		: bool true/false of relation between A and B
*/
bool Compare(string A, string B, int type, Comparison op)
{
	int Ai, Bi;
	float Af, Bf;
	switch (type)
	{
	case CHAR:
		switch (op)
		{
		case LessThan:return A < B;
			break;
		case LessEqual:return A <= B;
			break;
		case GreatThan:return A > B;
			break;
		case GreatEqual:return A >= B;
			break;
		case Equal:return A == B;
			break;
		case NotEqual:return A != B;
			break;
		default:cout << "Wrong operation of compare!" << endl; return 0;
			break;
		}
		break;
	case INT:
		Ai = atoi(A.c_str());
		Bi = atoi(B.c_str());
		switch (op)
		{
		case LessThan:return Ai < Bi;
			break;
		case LessEqual:return Ai <= Bi;
			break;
		case GreatThan:return Ai > Bi;
			break;
		case GreatEqual:return Ai >= Bi;
			break;
		case Equal:return Ai == Bi;
			break;
		case NotEqual:return Ai != Bi;
			break;
		default:cout << "Wrong operation of compare!" << endl; return 0;
			break;
		}
		break;
	case FLOAT:
		Af = atof(A.c_str());
		Bf = atof(B.c_str());
		switch (op)
		{
		case LessThan:return Af < Bf;
			break;
		case LessEqual:return Af <= Bf;
			break;
		case GreatThan:return Af > Bf;
			break;
		case GreatEqual:return Af >= Bf;
			break;
		case Equal:return Af == Bf;
			break;
		case NotEqual:return Af != Bf;
			break;
		default:cout << "Wrong operation of compare!" << endl; return 0;
			break;
		}
		break;
	default:cout << "Wrong type of attribution!" << "TYPE:" << type << endl; return 0;
		break;
	}
	return 0;
}
/*
*function	: compare Condition in record except indexAttribution
*parm		: Table				tableinfo
*			  Row				Object	(record)
*			  vector<int>		num		(The number of indexAttribution)
*			  vector<Condition> op
*return		: bool true/false of record object satisfy the op(relation in where)
*/
bool selectOther(Table tableinfo, Row object, vector<int> num, vector<Condition> op)
{
	for (int i = 0; i < op.size(); i++)
	{
		if (i!=num[0] && i!=num[num.size()-1])//num.size() bewteen 1 and 2
		{
			if (!Compare(object.columns[op[i].columnNum].Value, op[i].value, tableinfo.attributes[op[i].columnNum].type, op[i].op))
				return false;
		}
	}
	return true;
}
/*
*function	: translate object from int to string
*parm		: int object
*			  int length	(the length after translate)
*return		: string Out	(object in string with length)
*/
string IntToString(int object, int length)
{
	char* tmpt = new char(length+1);
	_itoa(object, tmpt, 10);
	string Out = tmpt;
	while (Out.length() < length)
		Out = '0' + Out;
	return Out;
}
/*****************************************************************************************************/
/*----------------------------------------------B+ Tree----------------------------------------------*/
/*****************************************************************************************************/
/*
*function	: get a pointer in BPlusTree
*parm		: int pos		(the position in block)
*return		: int pointer
*/
int BPlusTree::GetPtr(int pos)
{
	int ptr = 0;
	for (int i = pos; i<pos + POINTERLENGTH; i++) {
		ptr = 10 * ptr + buf.bufferBlock[bufferNum].Values[i] - '0';
	}
	return ptr;
}
/*
*function	: get the RecordNum in BPlusTree
*return		: the recordNum
*/
int BPlusTree::GetRecordNum()
{
	int recordNum = 0;
	for (int i = 2; i<6; i++) {
		if (buf.bufferBlock[bufferNum].Values[i] == NIL) break;
		recordNum = 10 * recordNum + buf.bufferBlock[bufferNum].Values[i] - '0';
	}
	return recordNum;
}
/******************************************************************************************************/
/*---------------------------------------------- Branch ----------------------------------------------*/
/******************************************************************************************************/
/*
*function	: constructed function
*parm		: int			NbufNum		(copy buf[NbufNum] into Branch)
*			  const Index&	indexinfor	(information of Index)
*/
Branch::Branch(int NbufNum, const Index & indexinfor)
{
	bufferNum = NbufNum;
	isRoot = (buf.bufferBlock[bufferNum].Values[0] == 'R');
	int recordMax = GetRecordNum();
	int position = 6 + POINTERLENGTH;

	recordNum = 0;				//one insert --> recordNum++  -->(until) recordMax
	ptrFather = GetPtr(6);		
	columnLength = indexinfor.columnLength;
	for (int i = 0; i < recordMax; i++)
	{
		string key = "";
		for (int i = position; i < position + columnLength; i++) 
		{
			if (buf.bufferBlock[bufferNum].Values[i] == NIL) 
				break;
			else 
				key += buf.bufferBlock[bufferNum].Values[i];
		}
		position += columnLength;
		int ptrChild = GetPtr(position);
		position += POINTERLENGTH;
		IndexBranch node(key, indexinfor.type, ptrChild);
		insert(node);
	}
}
/*
*function	: deconstructed function,save Branch into Buffer
*/
Branch::~Branch()
{
	//isRoot
	if (isRoot) 
		buf.bufferBlock[bufferNum].Values[0] = 'R';
	else 
		buf.bufferBlock[bufferNum].Values[0] = '_';
	//is not a Leaf
	buf.bufferBlock[bufferNum].Values[1] = '_';
	//recordNum
	string strRecordNum = IntToString(recordNum, 4);
	strncpy(buf.bufferBlock[bufferNum].Values + 2, strRecordNum.c_str(), 4);
	//FP
	int position = 6;				// 0-->R/_  1-->L/_  (2,5)-->recordNum (6,9)-->FP
	string strptrFather = IntToString(ptrFather, POINTERLENGTH);
	strncpy(buf.bufferBlock[bufferNum].Values + position, strptrFather.c_str(), POINTERLENGTH);
	position += POINTERLENGTH;
	//nodelist
	if (nodelist.size() == 0) {
		cout << "NodeList.size = 0. Excuse me?" << endl;
		exit(0);
	}
	list<IndexBranch>::iterator i;
	for (i = nodelist.begin(); i != nodelist.end(); i++)
	{
		string key = (*i).key;
		while (key.length() <columnLength)
			key += NIL;
		strncpy(buf.bufferBlock[bufferNum].Values + position, key.c_str(), columnLength);//key
		position += columnLength;

		string ptrChild = IntToString((*i).ptrChild,POINTERLENGTH);
		strncpy(buf.bufferBlock[bufferNum].Values + position, ptrChild.c_str(), POINTERLENGTH);//ptr
		position += POINTERLENGTH;
	}
}
/*
*function	: insert one node into Branch
*parm		: IndexBranch node		
*/
void Branch::insert(IndexBranch node)
{
	recordNum++;
	list<IndexBranch>::iterator i = nodelist.begin();
	if (nodelist.size() == 0)
		nodelist.insert(i, node);
	else 
	{
		for (i = nodelist.begin(); i != nodelist.end(); i++)
			if (Compare((*i).key, node.key, node.type, GreatThan))
				break;//get position
		nodelist.insert(i, node);//insert
	}
}
/******************************************************************************************************/
/*------------------------------------------------Leaf------------------------------------------------*/
/******************************************************************************************************/
/*
*function	: constructed function
*parm		: int			NbufNum		(copy buf[NbufNum] into Leaf)
*			  const Index&	indexinfor	(information of Index)
*/
Leaf::Leaf(int NbufNum, const Index & indexinfor)
{
	bufferNum = NbufNum;
	isRoot = (buf.bufferBlock[bufferNum].Values[0] == 'R');
	int recordMax = GetRecordNum();
	recordNum = 0;
	ptrFather = GetPtr(6);
	lastSibling = GetPtr(6 + POINTERLENGTH);
	nextSibling = GetPtr(6 + POINTERLENGTH * 2);
	columnLength = indexinfor.columnLength;	//Save it to use in deconstructed function
	int position = 6 + POINTERLENGTH * 3;	//R L recordNum FP LP RP position

	for (int i = 0; i < recordMax; i++)
	{
		string key = "";
		for (int i = position; i < position + columnLength; i++)
		{
			if (buf.bufferBlock[bufferNum].Values[i] == NIL) break;
			else key += buf.bufferBlock[bufferNum].Values[i];
		}
		position += columnLength;
		int File_Offset = GetPtr(position);
		position += POINTERLENGTH;
		int Block_Offset = GetPtr(position);
		position += POINTERLENGTH;
		IndexLeaf node(key, indexinfor.type, File_Offset, Block_Offset);//Create IndexLeaf
		insert(node);
	}
}
/*
*function	: deconstructed function,save Leaf into Buffer
*/
Leaf::~Leaf()
{
	//isRoot
	if (isRoot) buf.bufferBlock[bufferNum].Values[0] = 'R';
	else buf.bufferBlock[bufferNum].Values[0] = '_';
	//isLeaf
	buf.bufferBlock[bufferNum].Values[1] = 'L';
	//recordNum
	string strRecordNum = IntToString(recordNum,4);
	strncpy(buf.bufferBlock[bufferNum].Values + 2, strRecordNum.c_str(), 4);
	int position = 6;
	//FP
	string strptrFather = IntToString(ptrFather, POINTERLENGTH);
	strncpy(buf.bufferBlock[bufferNum].Values + position, strptrFather.c_str(), POINTERLENGTH);
	position += POINTERLENGTH;
	//LP
	string strLastSibling = IntToString(lastSibling, POINTERLENGTH);;
	strncpy(buf.bufferBlock[bufferNum].Values + position, strLastSibling.c_str(), POINTERLENGTH);
	position += POINTERLENGTH;
	//RP
	string strNextSibling = IntToString(nextSibling, POINTERLENGTH);;
	strncpy(buf.bufferBlock[bufferNum].Values + position, strNextSibling.c_str(), POINTERLENGTH);
	position += POINTERLENGTH;

	//nodelist
	if (nodelist.size() == 0) {
		cout << "NodeList.size = 0. Excuse me?" << endl;
		exit(0);
	}

	list<IndexLeaf>::iterator i;
	for (i = nodelist.begin(); i != nodelist.end(); i++)
	{
		string key = (*i).key;
		while (key.length() <columnLength)
			key += NIL;
		strncpy(buf.bufferBlock[bufferNum].Values + position, key.c_str(), columnLength);//key
		position += columnLength;

		string File_Offset = IntToString((*i).File_Offset,POINTERLENGTH);
		strncpy(buf.bufferBlock[bufferNum].Values + position, File_Offset.c_str(), POINTERLENGTH);//FileOffset
		position += POINTERLENGTH;

		string Block_Offset = IntToString((*i).Block_Offset, POINTERLENGTH);;
		strncpy(buf.bufferBlock[bufferNum].Values + position, Block_Offset.c_str(), POINTERLENGTH);//BufferOffset
		position += POINTERLENGTH;
	}
}
/*
*function	: insert one node into Leaf
*parm		: IndexLeaf node
*/
void Leaf::insert(IndexLeaf node)
{
	recordNum++;
	list<IndexLeaf>::iterator i = nodelist.begin();
	if (nodelist.size() == 0)
		nodelist.insert(i, node);
	else 
	{
		for (i = nodelist.begin(); i != nodelist.end(); i++)
			if (Compare((*i).key, node.key, node.type, GreatThan))
				break;//get position
		nodelist.insert(i, node);//insert
	}
}
/******************************************************************************************************/
/*--------------------------------------------IndexManager--------------------------------------------*/
/******************************************************************************************************/
/*
*function	: Create a BPlusTree with indexAttribution in table
*parm		: const Table&	tableinfor	(tableinformation)
*			  Index&		indexinfor	(indexinformation)
*/
void IndexManager::createIndex(const Table & tableinfor, Index & indexinfor)
{
	//create a new file
	string filename = indexinfor.index_name + ".index";
	fstream  fout(filename.c_str(), ios::out);
	fout.close();
	//create a root for the tree
	int blockNum = buf.getEmptyBlock();
	buf.bufferBlock[blockNum].FileName = filename;
	buf.bufferBlock[blockNum].BlockOffset = 0;
	buf.bufferBlock[blockNum].inUse = 1;
	buf.bufferBlock[blockNum].Values[0] = 'R';//0->R/_   R-->Root  _-->Not Root
	buf.bufferBlock[blockNum].Values[1] = 'L';//1->L/_	 L-->Leaf  _-->Not Leaf
	memset(buf.bufferBlock[blockNum].Values + 2, '0', 4);//(2,5)-->RecordNum  (0000~9999)
														 //Next 3*LENGTHBlockPtr FP , LP , RP
	for (int i = 0; i < 3; i++)
		memset(buf.bufferBlock[blockNum].Values + 6 + POINTERLENGTH*i, '0', POINTERLENGTH);
	indexinfor.blockNum++;

	//retrieve datas of the table and form a B+ Tree
	filename = tableinfor.name + ".table";
	string stringrow;
	string key;

	int length = tableinfor.totalLength + 1;			//add one byte to judge the record delete or not
	const int recordNum = BLOCKSIZE / length;

	//read datas from the record and sort it into a B+ Tree and store it
	for (int blockOffset = 0; blockOffset < tableinfor.blockNum; blockOffset++) 
	{
		int bufferNum = buf.getBlockNum(filename, blockOffset);
		for (int offset = 0; offset < recordNum; offset++) 
		{
			int position = offset * length;
			stringrow = buf.bufferBlock[bufferNum].getValues(position, length);
			if (stringrow.c_str()[0] == NIL) 
				continue;								//inticate that this row of record have been deleted
			stringrow.erase(stringrow.begin());			//erase the first byte
			key = getColumnValue(tableinfor, indexinfor, stringrow);
			IndexLeaf node(key, indexinfor.type, blockOffset, offset);
			insertValue(indexinfor, node);
		}
	}
}
/*
*function	: Create a IndexLeaf node and insert it into BPlusTree
*parm		: const Table&	tableinfor	(tableinformation)
*			  Index&		indexinfor	(indexinformation)
*/
void IndexManager::creatValue(const Table & tableinfor, Index & indexinfor)
{
	string stringrow, key;
	string filename = tableinfor.name + ".table";
	int blockOffset = tableinfor.blockNum - 1;
	int bufferNum = buf.getBlockNum(filename, blockOffset);
	int length = tableinfor.totalLength + 1;		//add one byte to judge the record delete or not
	const int recordNum = BLOCKSIZE / length;
	
	for (int offset = recordNum - 1; offset >= 0; offset--)
	{
		int position = offset * length;
		stringrow = buf.bufferBlock[bufferNum].getValues(position, length);
		if (stringrow.c_str()[0] == NIL)
			continue;								//inticate that this row of record have been deleted
		stringrow.erase(stringrow.begin());			//erase the first byte
		key = getColumnValue(tableinfor, indexinfor, stringrow);
		IndexLeaf node(key, indexinfor.type, blockOffset, offset);
		insertValue(indexinfor, node);
		return ;	//find the latest record and then return
	}
}
/*
*function	: Insert a IndexLeaf node into BPlusTree
*parm		: Index&		indexinfor	(indexinformation)
*			  IndexLeaf		node		(the node to be inserted)
*			  int			blockOffset (the block offset in the BPlusTree)
*return		: IndexBranch	reBranch	(NULL	-->insert into BPlusTree without split)
*										(NOTNULL-->the new IndexBranch after split)
*/
IndexBranch IndexManager::insertValue(Index & indexinfor, IndexLeaf node, int blockOffset)
{
	IndexBranch reBranch;		//for return, intial to be NIL
	string filename = indexinfor.index_name + ".index";
	int bufferNum = buf.getBlockNum(filename, blockOffset);
	bool isLeaf = (buf.bufferBlock[bufferNum].Values[1] == 'L');// L for leaf
	
	if (isLeaf) {	//Leaf
		Leaf leaf(bufferNum, indexinfor);
		leaf.insert(node);

		const int RecordLength = indexinfor.columnLength + POINTERLENGTH * 2;
		const int MaxrecordNum = (BLOCKSIZE - 6 - POINTERLENGTH * 3) / RecordLength;
		if (leaf.recordNum > MaxrecordNum)	//record number is too great, need to split
		{
			if (leaf.isRoot)	//this leaf is a root
			{
				int rbufferNum = leaf.bufferNum;	// buffer number for new root
				leaf.bufferNum = buf.addBlockInFile(indexinfor);	//find a new place for old leaf
				int sbufferNum = buf.addBlockInFile(indexinfor);	// buffer number for sibling 
				Branch branchRoot(rbufferNum);	//new root, which is branch indeed
				Leaf leafadd(sbufferNum);	//sibling
											//is root
				branchRoot.isRoot = 1;
				leafadd.isRoot = 0;
				leaf.isRoot = 0;

				branchRoot.ptrFather = leafadd.ptrFather = leaf.ptrFather = 0;
				branchRoot.columnLength = leafadd.columnLength = leaf.columnLength;
				//link the newly added leaf block in the link list of leaf  |******| + | | --> |***| + |****|
				leafadd.lastSibling = buf.bufferBlock[leaf.bufferNum].BlockOffset;
				leaf.nextSibling = buf.bufferBlock[leafadd.bufferNum].BlockOffset;
				while (leafadd.nodelist.size() < leaf.nodelist.size()) 
				{
					IndexLeaf tnode = leaf.pop();
					leafadd.insert(tnode);
				}
				//create the new IndexBranch
				IndexBranch tmptNode;
				tmptNode.key = leafadd.getfront().key;
				tmptNode.type = indexinfor.type;
				tmptNode.ptrChild = buf.bufferBlock[leafadd.bufferNum].BlockOffset;
				branchRoot.insert(tmptNode);
				tmptNode.key = leaf.getfront().key;
				tmptNode.ptrChild = buf.bufferBlock[leaf.bufferNum].BlockOffset;
				branchRoot.insert(tmptNode);
				return reBranch;
			}
			else //this leaf is not a root, we have to cascade up
			{
				int bufferNum = buf.addBlockInFile(indexinfor);
				Leaf leafadd(bufferNum);
				leafadd.isRoot = 0;
				leafadd.ptrFather = leaf.ptrFather;
				leafadd.columnLength = leaf.columnLength;

				//link the newly added leaf block in the link list of leaf
				leafadd.nextSibling = leaf.nextSibling;
				leafadd.lastSibling = buf.bufferBlock[leaf.bufferNum].BlockOffset;
				leaf.nextSibling = buf.bufferBlock[leafadd.bufferNum].BlockOffset;
				if (leafadd.nextSibling != 0) 
				{
					int bufferNum = buf.getBlockNum(filename, leafadd.nextSibling);
					Leaf leafnext(bufferNum, indexinfor);
					leafnext.lastSibling = buf.bufferBlock[leafadd.bufferNum].BlockOffset;
				}
				while (leafadd.nodelist.size() < leaf.nodelist.size()) 
				{
					IndexLeaf tnode = leaf.pop();
					leafadd.insert(tnode);
				}
				reBranch.key = leafadd.getfront().key;
				reBranch.ptrChild = leaf.nextSibling;
				reBranch.type = indexinfor.type;
				return reBranch;
			}//if (leaf.isRoot)
		}//if (leaf.recordNum > MaxrecordNum)
		else {
			return reBranch;
		}
	}//if (isLeaf)
	else 
	{
		Branch branch(bufferNum, indexinfor);
		list<IndexBranch>::iterator i = branch.nodelist.begin();
		if (Compare((*i).key, node.key, indexinfor.type, GreatThan))//new value to be insert is small than first value in the Branch
		{
			(*i).key = node.key;	//Update the first value in the Branch
		}
		else {
			for (i = branch.nodelist.begin(); i != branch.nodelist.end(); i++)
				if (Compare((*i).key, node.key, indexinfor.type, GreatThan))
					break;
			i--;//Get the correct position
		}

		IndexBranch bnode = insertValue(indexinfor, node, (*i).ptrChild);//go down

		if (bnode.key == "") 
		{
			return reBranch;
		}
		else //bnode.key != "", split has been happened in the Children
		{
			branch.insert(bnode);
			const int RecordLength = indexinfor.columnLength + POINTERLENGTH;
			const int MaxrecordNum = (BLOCKSIZE - 6 - POINTERLENGTH) / RecordLength;
			if (branch.recordNum > MaxrecordNum) //need to split up
			{
				if (branch.isRoot) 
				{
					int rbufferNum = branch.bufferNum;	// buffer number for new root
					branch.bufferNum = buf.addBlockInFile(indexinfor);	//find a new place for old branch
					int sbufferNum = buf.addBlockInFile(indexinfor);	// buffer number for sibling 
					Branch branchRoot(rbufferNum);	//new root
					Branch branchadd(sbufferNum);	//sibling

													//is root
					branchRoot.isRoot = 1;
					branchadd.isRoot = 0;
					branch.isRoot = 0;

					branchRoot.ptrFather = branchadd.ptrFather = branch.ptrFather = 0;
					branchRoot.columnLength = branchadd.columnLength = branch.columnLength;

					while (branchadd.nodelist.size() < branch.nodelist.size()) 
					{
						IndexBranch tnode = branch.pop();
						branchadd.insert(tnode);
					}
					//create the new IndexBranch
					IndexBranch tmptNode;
					tmptNode.key = branchadd.getfront().key;
					tmptNode.type = indexinfor.type;
					tmptNode.ptrChild = buf.bufferBlock[branchadd.bufferNum].BlockOffset;
					branchRoot.insert(tmptNode);
					tmptNode.key = branch.getfront().key;
					tmptNode.ptrChild = buf.bufferBlock[branch.bufferNum].BlockOffset;
					branchRoot.insert(tmptNode);
					return reBranch;//here the function must have already returned to the top lay
				}//if (branch.isRoot)
				else 
				{
					int bufferNum = buf.addBlockInFile(indexinfor);
					Branch branchadd(bufferNum);
					branchadd.isRoot = 0;
					branchadd.ptrFather = branch.ptrFather;
					branchadd.columnLength = branch.columnLength;

					while (branchadd.nodelist.size() < branch.nodelist.size()) 
					{
						IndexBranch tnode = branch.pop();
						branchadd.insert(tnode);
					}
					reBranch.key = branchadd.getfront().key;
					reBranch.ptrChild = buf.bufferBlock[bufferNum].BlockOffset;
					return reBranch;
				}
			}//if (branch.recordNum > MaxrecordNum)
			else 
			{
				return reBranch;
			}
		}//if (bnode.key == "")
	}////if (isLeaf)
	return reBranch;//here is just for safe
}
/*
*function	: finish the where-clause with index
*parm		: const Table&		tableinfor	(tableinformation)
*			  Index&			indexinfor	(indexinformation)
*			  vector<int>		num			(The number of indexAttribution in the op)
*			  vector<Conditon>	op			(the operation in the where-clause)
*return		: Data				datas		(tempory table that satisfy the where-clause)
*/
Data IndexManager::select(const Table& tableinfor, const Index& indexinfor, vector<int> num, vector<Condition> op, int  blockOffset)
{
	Data datas;
	string filename = indexinfor.index_name + ".index";
	int bufferNum = buf.getBlockNum(filename, blockOffset);
	bool isLeaf = (buf.bufferBlock[bufferNum].Values[1] == 'L');// L for leaf
	if (isLeaf) 
	{
		while (1)
		{
			Leaf leaf(bufferNum, indexinfor);
			list<IndexLeaf>::iterator i;
			for (i = leaf.nodelist.begin(); i != leaf.nodelist.end(); i++) 
			{
				//cout << (*i).key << ' ' << op[num[0]].value << endl;
				if (Compare((*i).key, op[num[0]].value, indexinfor.type, op[num[0]].op))
				{	//if num.size() ==2 ---> interval select so it is overstep [x,y] when (*i).key > y
					if ((num.size() == 2) && (!Compare((*i).key, op[num[1]].value, indexinfor.type, op[num[1]].op)))
					{
						return datas;
					}
					filename = indexinfor.table_name + ".table";
					int recordBufferNum = buf.getBlockNum(filename, (*i).File_Offset);//save record to Buffer
					int position = (tableinfor.totalLength + 1)* ((*i).Block_Offset);
					//get the new record
					string stringrow = buf.bufferBlock[recordBufferNum].getValues(position, tableinfor.totalLength);
					if (stringrow.c_str()[0] != NIL)
					{
						stringrow.erase(stringrow.begin());
						Row splitedRow = splitRow(tableinfor, stringrow);
						if (selectOther(tableinfor, splitedRow, num, op))//other condition compare
							datas.rows.push_back(splitedRow);
					}
					if (num.size() == 1 && op[num[0]].op == Equal)//equal must only one record
					{
						return datas;
					}
				}
			}
			if (leaf.nextSibling != 0) //next block
			{
				filename = indexinfor.index_name + ".index";
				bufferNum = buf.getBlockNum(filename, leaf.nextSibling);
			}
			else return datas;
		}
	}
	else {//not leaf, go down to the leaf
		Branch branch(bufferNum, indexinfor);
		list<IndexBranch>::iterator i = branch.nodelist.begin();
		if ((!Compare((*i).key, op[num[0]].value, indexinfor.type, LessThan)))
		{
			datas = select(tableinfor, indexinfor, num, op, (*i).ptrChild);
			return datas;
		}
		else
		{
			for (i = branch.nodelist.begin(); i != branch.nodelist.end(); i++) 
			{
				if (!Compare((*i).key, op[num[0]].value, indexinfor.type, LessThan))
					break;
			}
			i--;//get the correct position
			datas = select(tableinfor, indexinfor, num, op, (*i).ptrChild);
			return datas;
		}
	}
	return datas;
}
/*
*function	: get the split Row from table(string to Row)
*parm		: Table&	tableinfor	(tableinformation)
*			  string	row			(the row information)
*return		: Row		splitedRow	(after translating the row information from string to Row)
*/
Row IndexManager::splitRow(Table tableinfor, string row)
{
	Row splitedRow;
	int s_pos = 0, f_pos = 0;//start position & finish position
	for (int i = 0; i < tableinfor.attriNum; i++)
	{
		s_pos = f_pos;
		f_pos += tableinfor.attributes[i].length;
		Col col;
		for (int j = s_pos; j < f_pos; j++)
		{
			if (row[j] == NIL)
				break;
			col.Value += row[j];
		}
		splitedRow.columns.push_back(col);
	}
	return splitedRow;
}
/*
*function	: get the Column value in the table which is indexAttribution
*parm		: const Table&	tableinfor	(tableinformation)
*			  const Index&	indexinfor	(indexinformation)
*			  string		row			(the row information)
*return		: string		colValue	(column value)
*/
string IndexManager::getColumnValue(const Table & tableinfor, const Index & indexinfor, string row)
{
	string colValue;
	int s_pos = 0, f_pos = 0;	//start position & finish position
	for (int i = 0; i <= indexinfor.columnNum; i++)
	{
		s_pos = f_pos;
		f_pos += tableinfor.attributes[i].length;
	}
	for (int j = s_pos; j < f_pos && row[j] != NIL; j++)
		colValue += row[j];
	return colValue;
}
