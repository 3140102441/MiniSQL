#include "CatalogManage.h"


void CatalogManage::initialtable(){                                           //initial table
	int i,j;
	Table temptable;
	const string filename = "catalog.table";
	fstream  infile(filename.c_str(), ios::in);
	infile >> tablenum;
	Attribute temp;
	for (i = 0; i < tablenum; i++){
		infile >> temptable.name;
		infile >> temptable.attriNum;
		infile >> temptable.blockNum;
		for (j = 0; j < temptable.attriNum; j++){
			infile >> temp.name;
			infile >> temp.isPrimeryKey;
			infile >> temp.isUnique;
			infile >> temp.length;
			infile >> temp.type;
			temptable.attributes.push_back(temp);
			temptable.totalLength += temptable.attributes[j].length;
		}
			tables.push_back(temptable);
	}
	infile.close();
}


void CatalogManage::initialindex(){                                              //initial  index 
	Index tempindex;
	const string filename = "catalog.index";
	fstream  infile(filename, ios::in);
	infile >> indexnum;
	for (int i = 0; i < indexnum; i++){
		infile >> tempindex.table_name;
		infile >> tempindex.index_name;
		infile >> tempindex.blockNum;
		infile >> tempindex.columnNum;
		infile >> tempindex.columnLength;
		infile >> tempindex.type;
		indexs.push_back(tempindex);
	}
	infile.close();
}
 
void CatalogManage::storetable(){                                                     //store  table
	const string filename = "catalog.table";
	fstream  outfile(filename, ios::out);
	outfile << tablenum << endl;
	for (int i=0; i < tablenum; i++){
		outfile << tables[i].name<<" ";
		outfile << tables[i].attriNum<<" ";
		outfile << tables[i].blockNum<<endl ;
		for (int j = 0; j < tables[i].attributes.size(); j++){
			outfile << tables[i].attributes[j].name <<" ";
			outfile << tables[i].attributes[j].isPrimeryKey <<" ";
			outfile << tables[i].attributes[j].isUnique <<" ";
			outfile << tables[i].attributes[j].length <<" ";
			outfile << tables[i].attributes[j].type << endl;
		}
	}
	outfile.close();
}



void CatalogManage::storeindex(){                                                    //store  index
	const string filename = "catalog.index";
	fstream  outfile(filename, ios::out);
	outfile << indexnum << endl;
	for (int i = 0; i < indexnum; i++){
		outfile << indexs[i].table_name << " ";
		outfile << indexs[i].index_name << " ";
		outfile << indexs[i].blockNum << " ";
		outfile << indexs[i].columnNum << " ";
		outfile << indexs[i].columnLength << " ";
		outfile << indexs[i].type << endl;
	}
	outfile.close();
}


void CatalogManage::createtable(Table& tableinfo){                            //create  table 
	tablenum++;
	for (int i = 0; i < tableinfo.attributes.size(); i++){
		tableinfo.totalLength += tableinfo.attributes[i].length;
	}
	tables.push_back(tableinfo);
}

bool CatalogManage::isexisttable(string tablename){                           //根据table name判断是否存在table
	for (int i = 0; i < tables.size(); i++){
		if (tables[i].name == tablename)
			return true;
	}
	return false;
}

void CatalogManage::createindex(Index& indexinfo){                            //create  index
	indexnum++;
	indexs.push_back(indexinfo);
}


bool  CatalogManage::existindex(string indexname){                            //根据index name 判断是否存在index
	for (int i = 0; i < indexs.size(); i++){
		if (indexs[i].index_name == indexname)
			return true;
	}
	return false;
}

bool  CatalogManage::existindex(string tablename, int colnum){                //根据table name和所在列数判断是否存在index
	for (int i = 0; i < indexs.size(); i++){
		if (indexs[i].table_name == tablename && indexs[i].columnNum== colnum)
			return true;
	}
	return false;
}

Table CatalogManage::gettable(string tablename){                             //get table
	Table temp;
	for (int i = 0; i < tables.size(); i++){
		if (tables[i].name == tablename)
			return tables[i];
	}
	return temp;
}

int  CatalogManage::getcolunmnum(Table tableinfo, string word){             //获取列数
	for (int i = 0; i < tableinfo.attributes.size(); i++){
		if (tableinfo.attributes[i].name == word)
			return i;
	}
	return -1;
}

void CatalogManage::update(Index indexinfo){                               //更新index 
	for (int i = 0; i < indexs.size(); i++){
		if (indexs[i].index_name == indexinfo.index_name){
			indexs[i] = indexinfo;
		}
	}
}

void  CatalogManage::droptable(Table tableinfo){                            //drop  table 
	int i;
	for (int i = 0; i < tables.size(); i++){
		if (tables[i].name == tableinfo.name){
			tables.erase(tables.begin()+i);
			tablenum--;
		}
	}
	for (i = 0; i < indexs.size(); i++){  
		if (indexs[i].table_name == tableinfo.name){
			indexs.erase(indexs.begin() + i);
			indexnum--;
		}

	}

}


Index CatalogManage::getindexinfo(string indexname){                          //get index
	Index temp;
	for (int i = 0; i < indexs.size(); i++){
		if (indexs[i].index_name == indexname)
			return indexs[i];
	}
	return temp;
}

void CatalogManage::dropindex(string indexname){                              //drop  index 
	for (int i = 0; i < indexs.size(); i++){
		if (indexs[i].index_name == indexname){
			indexs.erase(indexs.begin() + i);
		}
	}
}

void CatalogManage::update(Table tableinfo){                                  //更新 table
	for (int i = 0; i < tables.size(); i++){
		if (tables[i].name == tableinfo.name){
			tables[i] = tableinfo;
		}
	}
}


Index  CatalogManage::getindexinfo(Table tableinfo, int colnum){             //get index 
	Index temp;
	for (int i = 0; i < indexs.size(); i++){
		if (indexs[i].table_name == tableinfo.name &&indexs[i].columnNum == colnum)
			return indexs[i];
	}
	return temp;
}

