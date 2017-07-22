#include "API.h"

using namespace std;

RecordManager record;
IndexManager index;
CatalogManage catalog;

void output(Data datas, Table tableinfo, vector <Attribute> columns);        //打印查询结果

void API::execute(interpreter interfaces){                                   //API执行解析后的查询语句
	int i,eqpos=-1,j;
	Data datas;
	Index indexinfo;
	Table tableinfo;
	vector<int> num;
	vector<int> finalnum;
	switch (interfaces.operation){
	case UNKNOW:                                                             //unknown  query
		cout << "Unkonw query ,please chech your input !" << endl;
		break;
	case EMPTY:																 //empty  query
		cout << "Empty query,please check your input !" << endl;
		break;
	case QUIT:                                                               //quit
		isdone = true;
		cout << "Good bye,have a good day!" << endl;
		break;
	case CREATETABLE:                                                        //create table 
		interfaces.tableinfo.attriNum = interfaces.tableinfo.attributes.size();
		catalog.createtable(interfaces.tableinfo);
		record.createTable(interfaces.tableinfo);

		tableinfo = catalog.gettable(interfaces.tableinfo.name);                            //为primary key自动建立索引
		indexinfo.columnLength = tableinfo.attributes[interfaces.primarykeypos].length; 
		indexinfo.columnNum = interfaces.primarykeypos;
		indexinfo.index_name = "primary_index";
		indexinfo.table_name = tableinfo.name;
		indexinfo.type = tableinfo.attributes[interfaces.primarykeypos].type;
		catalog.createindex(indexinfo);
		index.createIndex(tableinfo, indexinfo);
		catalog.update(indexinfo);

		cout << "Table " << interfaces.tableinfo.name << " has been created successfully!" << endl;
		break;
	case CREATETABLERR:                                                                                 //create table 语法错误
		cout << "Incorrect usage of \"create table \" query,please check your input !" << endl;
		break;
	case CREATEINDEXRR:																					//create index 语法错误
		cout << "Incorrect usage of \"create index \" query,please check your input !" << endl;
		break;
	case TABLEEXIST:																					//table 已经存在
		cout << "The table " << interfaces.tablename << " has been created,please check the database!";
		break;
	case CHARERROR:																						//字符串长度错误
		cout << "Error: only 1~255 charactors is allowed! Please check your input!" << endl;
		break;
	case NOKEY:																							//primary KEY 不存在
		cout << "Error : invalid primary key!Please check your input!" << endl;
		break;
	case NOPRIMARYKEY:																					//primary KEY 没定义
		cout << "Error : No primary key is defined in the table."<< endl;
		break;
	case EXISIINDEX:																					//index 已经存在
		cout << "Error : The index " <<interfaces.colname << " has already been created!" << endl;
		break;
	case EXISTINDEXNAME:																				//index name已经存在
		cout << "Error : The indexname " << interfaces.indexname << " has already existed!" << endl;
		break;
	case NOTABLE:																						//table 不存在
		cout << "Table " << interfaces.tablename << " is not exist,please check your input!" << endl;
		break;
	case COLUNMERROR:																					//属性不存在 
		cout << interfaces.colname << "is not exist,please check the table!" << endl;
		break;
	case CREATINDEX:                                                                                    //create index
		if (!interfaces.tableinfo.attributes[interfaces.indexinfo.columnNum].isUnique){
			cout << "Column " << interfaces.tableinfo.attributes[interfaces.indexinfo.columnNum].name << "  is not unique." << endl;
			break;
		}
		catalog.createindex(interfaces.indexinfo);
		index.createIndex(interfaces.tableinfo, interfaces.indexinfo);
		catalog.update(interfaces.indexinfo);
		cout << "The index " << interfaces.indexinfo.index_name << "has been created successfully." << endl;
		break;
	case DROPTABLE:																					//DROP table
		record.dropTable(interfaces.tableinfo);														//删除记录
		catalog.droptable(interfaces.tableinfo);													//删除 catalog
		for (i = 0; i < interfaces.tableinfo.attriNum; i++){
			indexinfo = catalog.getindexinfo(interfaces.tableinfo, i);
			if (indexinfo.index_name != "")
				index.dropIndex(indexinfo);
		}
		cout << "Table " << interfaces.tableinfo.name << " has been dropped successfully." << endl;
		break;
	case NOINDEX:																					//NO index
		cout << "Index " << interfaces.indexname << " is not exist,please check the database." << endl;
		break;
	case DROPINDEX:																					//drop index
		indexinfo = catalog.getindexinfo(interfaces.indexname);
		if (indexinfo.index_name == ""){
			cout << "Index" << interfaces.indexname << "does not exist!" << endl;
		}
		index.dropIndex(indexinfo);																	
		catalog.dropindex(interfaces.indexname);
		cout << "The index has been dropped successfully" << endl;
		break; 
	case SELECTRR:																					//select 语法错误
		cout << "Incorrect usage of \"select \" query,please check your input !" << endl;
		break;
	case SELECTALL:																					//select all
		datas = record.select(interfaces.tableinfo);
		if (datas.rows.size() == 0){
			cout << "No data is found!!!" << endl;
		}
		else
			output(datas, interfaces.tableinfo,interfaces.column);
		break;
	case NOCOLUMN:                                                                              //不存在属性
		cout << "The column " << interfaces.colname << "is not exist,please check the database!" << endl;
		break;
	case SELECT:                                                                                //select
		for (i = 0; i < interfaces.conditions.size(); i++){										
			if (catalog.existindex(interfaces.tablename, interfaces.conditions[i].columnNum)){
				if (interfaces.conditions[i].op != NotEqual){
					num.push_back(i);
					indexinfo = catalog.getindexinfo(interfaces.tableinfo, interfaces.conditions[i].columnNum);
					break;
				}
			}
		}
		if (i < interfaces.conditions.size()){
			for ( j = i + 1; j < interfaces.conditions.size(); j++){
				if (catalog.getindexinfo(interfaces.tableinfo, interfaces.conditions[j].columnNum).index_name == indexinfo.index_name&&interfaces.conditions[j].op != NotEqual)
					num.push_back(j);
			}
			for (j = 0; j < num.size(); j++){
				if (interfaces.conditions[num[j]].op == Equal){
					finalnum.push_back(num[j]);
					eqpos = 1;
				}
			}
			if (eqpos == -1){
				for (j = 0; j < num.size(); j++){
					if (interfaces.conditions[num[j]].op == GreatThan || interfaces.conditions[num[j]].op == GreatEqual)
						finalnum.push_back(num[j]);
				}
				for (j = 0; j < num.size(); j++){
					if (interfaces.conditions[num[j]].op == LessThan || interfaces.conditions[num[j]].op == LessEqual)
						finalnum.push_back(num[j]);
				}
			}
			datas = index.select(interfaces.tableinfo, indexinfo, finalnum, interfaces.conditions);    //select use index
		}
		else{
			datas = record.select(interfaces.tableinfo, interfaces.conditions);                      //select  without index
		}
		if (datas.rows.size() != 0)
			output(datas, interfaces.tableinfo, interfaces.column);                                 //output
		else{
			cout << "No data is found!!!" << endl;
		}
		break;
	case INSERTRR:																					//inset  语法错误
		cout << "Incorrect usage of \"insert\" query,please check your input !" << endl;
		break;
	case INSERTSIZERR:                                                                           //insert的属性数目错误
		cout << "The number of insert values is not equal to the number of attributes of the table,please check the table!" << endl;
		break;
	case INSERT:																				//insert
		tableinfo = interfaces.tableinfo;
		datas = record.select(tableinfo, interfaces.conditions);
		if (datas.rows.size() > 0){
			cout << "Primary Key Redundancy occurs, thus insertion failed" << endl;             //判断是否冗余
			break;
		}
		
		if (interfaces.uniquekeypos != -1){
			datas = record.select(tableinfo, interfaces.uniqueconditions);
			if (datas.rows.size() > 0){
				cout << "Unique Value Redundancy occurs, thus insertion failed" << endl;
				break;
			}
		}
		for (i = 0; i < interfaces.row.columns.size();i++){                                        //update index
			if (catalog.existindex(tableinfo.name, i)){
				index.creatValue(tableinfo, catalog.getindexinfo(tableinfo, i)) ;
			}
		}
		
		record.insertRecord(tableinfo, interfaces.row);
		catalog.update(tableinfo);
		cout << "One record has been inserted successfully" << endl;
		break;
	case INSERTTYPERR:																			//insert的属性类型错误
		cout << "The type of the value " << interfaces.row.columns[interfaces.wrongtype].Value << " is not correct,please check the table!";
		break;
	case DELETERR:                                                                             //delete 语法错误
		cout << "Incorrect usage of \"delete\" query,please check your input !" << endl;
		break;
	case DELETE:																				//delete
	{
		int rownumber = record.deleteRecords(interfaces.tableinfo, interfaces.conditions);
		cout << rownumber << "  rows have been deleted." << endl;
	}
	break;
	case EXECRR:                                                                                //执行文件操作
		cout << "Incorrect usage of \"execfile\" query,please check your input !" << endl;
		break;
		
	}
}


void output(Data datas, Table tableinfo, vector <Attribute> columns){                          //output，支持 select attribute
	if (columns[0].name == "*"){
		cout << endl << "+";
		for (int i = 0; i < tableinfo.attriNum; i++){
			for (int j = 0; j < tableinfo.attributes[i].length + 1; j++){
				cout << "-";
			}
			cout << "+";
		}
		cout << endl;
		cout << "| ";
		for (int i = 0; i < tableinfo.attriNum; i++){
			cout << tableinfo.attributes[i].name;
			int lengthLeft = tableinfo.attributes[i].length - tableinfo.attributes[i].name.length();
			for (int j = 0; j < lengthLeft; j++){
				cout << ' ';
			}
			cout << "| ";
		}
		cout << endl;
		cout << "+";
		for (int i = 0; i < tableinfo.attriNum; i++){
			for (int j = 0; j < tableinfo.attributes[i].length + 1; j++){
				cout << "-";
			}
			cout << "+";
		}
		cout << endl;


		for (int i = 0; i < datas.rows.size(); i++){
			cout << "| ";
			for (int j = 0; j < tableinfo.attriNum; j++){
				int lengthLeft = tableinfo.attributes[j].length;
				for (int k = 0; k < datas.rows[i].columns[j].Value.length(); k++){
					if (datas.rows[i].columns[j].Value.c_str()[k] == NIL) break;
					else{
						cout << datas.rows[i].columns[j].Value.c_str()[k];
						lengthLeft--;
					}
				}
				for (int k = 0; k < lengthLeft; k++) cout << " ";
				cout << "| ";
			}
			cout << endl;
		}

		cout << "+";
		for (int i = 0; i < tableinfo.attriNum; i++){
			for (int j = 0; j < tableinfo.attributes[i].length + 1; j++){
				cout << "-";
			}
			cout << "+";
		}
		cout << endl;
	}
	else{
		cout << endl << "+";
		for (int i = 0; i < columns.size(); i++){
			int col;
			for (col = 0; col < tableinfo.attributes.size(); col++){
				if (tableinfo.attributes[col].name == columns[i].name) break;
			}
			for (int j = 0; j < tableinfo.attributes[col].length + 1; j++){
				cout << "-";
			}
			cout << "+";
		}
		cout << endl;
		cout << "| ";
		for (int i = 0; i < columns.size(); i++){
			int col;
			for (col = 0; col < tableinfo.attributes.size(); col++){
				if (tableinfo.attributes[col].name == columns[i].name) break;
			}
			cout << tableinfo.attributes[col].name;
			int lengthLeft = tableinfo.attributes[col].length - tableinfo.attributes[col].name.length();
			for (int j = 0; j < lengthLeft; j++){
				cout << ' ';
			}
			cout << "| ";
		}
		cout << endl;
		cout << "+";
		for (int i = 0; i < columns.size(); i++){
			int col;
			for (col = 0; col < tableinfo.attributes.size(); col++){
				if (tableinfo.attributes[col].name == columns[i].name) break;
			}
			for (int j = 0; j < tableinfo.attributes[col].length + 1; j++){
				cout << "-";
			}
			cout << "+";
		}
		cout << endl;


		for (int i = 0; i < datas.rows.size(); i++){
			cout << "| ";
			for (int j = 0; j < columns.size(); j++){
				int col;
				for (col = 0; col < tableinfo.attributes.size(); col++){
					if (tableinfo.attributes[col].name == columns[j].name) break;
				}
				int lengthLeft = tableinfo.attributes[col].length;
				for (int k = 0; k < datas.rows[i].columns[col].Value.length(); k++){
					if (datas.rows[i].columns[col].Value.c_str()[k] == EMPTY) break;
					else{
						cout << datas.rows[i].columns[col].Value.c_str()[k];
						lengthLeft--;
					}
				}
				for (int k = 0; k < lengthLeft; k++) cout << " ";
				cout << "| ";
			}
			cout << endl;
		}

		cout << "+";
		for (int i = 0; i < columns.size(); i++){
			int col;
			for (col = 0; col < tableinfo.attributes.size(); col++){
				if (tableinfo.attributes[col].name == columns[i].name) break;
			}
			for (int j = 0; j < tableinfo.attributes[col].length + 1; j++){
				cout << "-";
			}
			cout << "+";
		}
		cout << endl;
	}
	cout << datas.rows.size() << " rows have been found." << endl;
}
