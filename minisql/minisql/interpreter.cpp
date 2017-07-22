#include "interpreter.h"

extern CatalogManage catalog;
extern RecordManager  record;

int  isint(string word);
int  isfloat(string word);
int trance(string word);

//initial  

void interpreter::initcol()
{
	if (column.size()>0){
		column.clear();
	}
}

void interpreter::initcond()
{
	if (conditions.size()>0){
		conditions.clear();
	}
}

void interpreter::initvalue()
{
	if (row.columns.size()>0){
		row.columns.clear();
	}
}

void interpreter::analysis(string input){                             //解析  input 
	int flag, i, findkey = 0, tempnum;
	int isprimary=0;
	string word;
	Attribute temp;
	Attribute tempcol;
	Condition tempcon;
	Col    temprow;

	flag = cutword(input, word);
	if (!flag){
		operation = EMPTY;
		return;
	}
	if (word == "quit"){                              //quit
		flag = cutword(input, word);
		if (!flag)
			operation = QUIT;
		return;
	}
	if (word == "create"){							
		flag = cutword(input, word);
		if (word == "table"){							//create table 
			operation = CREATETABLERR;
			flag = cutword(input, word);
			if (!flag){
				return;
			}
			tablename = word;
			if (catalog.isexisttable(word)){
				operation = TABLEEXIST;
				return;
			}
			tableinfo.name = word;
			flag = cutword(input, word);
			if (!flag)
				return;
			if (word != "(")
				return;
			flag = cutword(input, word);
			if (!flag)
				return;
			temp.name = word;
			flag = cutword(input, word);
			if (!flag)
				return;
			if (word == "int"){                          //判断类型
				temp.type = INT;
				temp.length = INTLENGTH;
				flag = cutword(input, word);
				if (!flag)
					return;
				if (word == "unique"){
					temp.isUnique= 1;
					flag = cutword(input, word);
					if (!flag)
						return;
				}
			}
			else if (word == "float"){
				temp.type = FLOAT;
				temp.length = FLOATLENGTH;
				flag = cutword(input, word);
				if (!flag)
					return;
				if (word == "unique"){
					temp.isUnique = 1;
					flag = cutword(input, word);
					if (!flag)
						return;
				}
			}
			else if (word == "char"){
				temp.type = CHAR;
				flag = cutword(input, word);
				if (!flag || word != "(")
					return;
				flag = cutword(input, word);
				if (!flag)
					return;
				if (!isint(word))
					return;
				temp.length = trance(word) + 1;
				if (temp.length < 2 || temp.length>256){
					operation = CHARERROR;
					return;
				}
				flag = cutword(input, word);
				if (!flag || word != ")")
					return;
				flag = cutword(input, word);
				if (!flag)
					return;
				if (word == "unique"){
					temp.isUnique = 1;
					flag = cutword(input, word);
					if (!flag)
						return;
				}
			}
			else
				return;
			tableinfo.attributes.push_back(temp);
			temp.isPrimeryKey = temp.isUnique = 0;
			while (word == ","){
				flag = cutword(input, word);
				if (!flag)
					return;
				if (word == "primary"){                       //判断primary key
					flag = cutword(input, word);
					if (!flag || word != "key")
						return;
					isprimary = 1;
					flag = cutword(input, word);
					if (!flag || word != "(")
						return;
					flag = cutword(input, word);
					if (!flag)
						return;
					for (i = 0; i < tableinfo.attributes.size(); i++){
						if (word == tableinfo.attributes[i].name){
							tableinfo.attributes[i].isPrimeryKey= 1;
							tableinfo.attributes[i].isUnique = 1;
							primarykeypos = i;
							break;
						}
					}
					if (i == tableinfo.attributes.size()){
						operation = NOKEY;
						return;
					}
					flag = cutword(input, word);
					if (!flag || word != ")")
						return;
					flag = cutword(input, word);
					if (!flag)
						return;
				}
				else{
					temp.name = word;
					flag = cutword(input, word);
					if (!flag)
						return;
					if (word == "int"){
						temp.type = INT;
						temp.length = INTLENGTH;
						flag = cutword(input, word);
						if (!flag)
							return;
						if (word == "unique"){
							temp.isUnique= 1;
							flag = cutword(input, word);
							if (!flag)
								return;
						}
					}
					else if (word == "float"){
						temp.type = FLOAT;
						temp.length = FLOATLENGTH;
						flag = cutword(input, word);
						if (!flag)
							return;
						if (word == "unique"){
							temp.isUnique = 1;
							flag = cutword(input, word);
							if (!flag)
								return;
						}
					}
					else if (word == "char"){
						temp.type = CHAR;
						flag = cutword(input, word);
						if (!flag || word != "(")
							return;
						flag = cutword(input, word);
						if (!flag)
							return;
						if (!isint(word))
							return;
						temp.length = trance(word) + 1;
						if (temp.length < 2 || temp.length>256){
							operation = CHARERROR;
							return;
						}
						flag = cutword(input, word);
						if (!flag || word != ")")
							return;
						flag = cutword(input, word);
						if (!flag)
							return;
						if (word == "unique"){
							temp.isUnique = 1;
							flag = cutword(input, word);
							if (!flag)
								return;
						}
					}
					else
						return;
					tableinfo.attributes.push_back(temp);
					temp.isPrimeryKey = temp.isUnique = 0;
				}


			}
			if (word != ")")
				return;
			flag = cutword(input, word);
			if (!flag){
				if (isprimary == 0){
					operation = NOPRIMARYKEY;
					return;
				}
				operation = CREATETABLE;
				return;
			}
		}
		else if (word == "index"){                          //create  index 
			operation = CREATEINDEXRR; 
			flag = cutword(input, word);
			if (!flag)
				return;
			indexname = word;
			indexinfo.index_name = word;
			if (catalog.existindex(word)){
				operation = EXISTINDEXNAME;
				return;
			}
			flag = cutword(input, word);
			if (!flag || word != "on")
				return;
			flag = cutword(input, word);
			if (!flag)
				return;
			tablename = word;
			indexinfo.table_name = word;
			if (!catalog.isexisttable(word)){
				operation = NOTABLE;
			}
			tableinfo = catalog.gettable(tablename);
			flag = cutword(input, word);
			if (!flag || word != "(")
				return;
			flag = cutword(input, word);
			if (!flag)
				return;
			int colunmnum;
			colname = word;
			colunmnum = catalog.getcolunmnum(tableinfo, word);
			if (colunmnum == -1){
				operation = COLUNMERROR;
				return;
			}
			if (catalog.existindex(tablename, colunmnum)){
				operation = EXISIINDEX;
				return;
			}
			indexinfo.columnNum = colunmnum;
			indexinfo.type = tableinfo.attributes[colunmnum].type;
			indexinfo.columnLength = tableinfo.attributes[colunmnum].length;
			flag = cutword(input, word);
			if (!flag || word != ")")
				return;
			flag = cutword(input, word);
			if (!flag){
				operation = CREATINDEX;
			}
		}
	}
	else if (word == "drop"){                              
		flag = cutword(input, word);
		if (!flag)
			return;
		if (word == "table"){										//drop  table 
			flag = cutword(input, word);
			if (!flag)
				return;
			tablename = word;
			if (!catalog.isexisttable(word)){
				operation = NOTABLE;
				return;
			}
			tableinfo = catalog.gettable(word);
			flag = cutword(input, word);
			if (!flag){
				operation = DROPTABLE;
				return;
			}
		}
		else if (word == "index"){                             //drop index  
			flag = cutword(input, word);
			if (!flag)
				return;
			indexname = word;
			if (!catalog.existindex(word)){
				operation = NOINDEX;
				return;
			}
			flag = cutword(input, word);
			if (!flag){
				operation = DROPINDEX;
				return;
			}
		}
	}
	else if (word == "select"){                                   //select 
		operation = SELECTRR;
		flag = cutword(input, word);
		if (!flag)
			return;
		tempcol.name = word;
		column.push_back(tempcol);
		flag = cutword(input, word);
		if (!flag)
			return;
		while (word == ","){
			flag = cutword(input, word);
			if (!flag)
				return;
			tempcol.name = word;
			column.push_back(tempcol);
			flag = cutword(input, word);
			if (!flag)
				return;
		}
		if (word != "from")
			return;
		flag = cutword(input, word);
		if (!flag)
			return;
		tablename = word;
		if (!catalog.isexisttable(word)){
			operation = NOTABLE;
			return;
		}
		tableinfo = catalog.gettable(word);
		flag = cutword(input, word);
		if (!flag){
			operation = SELECTALL;                                        //select  all 
			return;
		}
		else if (word != "where")
			return;
		else{
			flag = cutword(input, word);
			if (!flag)
				return;
			tempnum = catalog.getcolunmnum(tableinfo, word);
			colname = word;
			if (tempnum == -1){
				operation = NOCOLUMN;
				return;
			}
			tempcon.columnNum = tempnum;
			flag = cutword(input, word);
			if (!flag)
				return;
			if (word == "<")                                          //判断OP
				tempcon.op = LessThan;
			else if (word == "<=")
				tempcon.op = LessEqual;
			else if (word == "<>")
				tempcon.op = NotEqual;
			else if (word == "=")
				tempcon.op = Equal;
			else if (word == ">")
				tempcon.op = GreatThan;
			else if (word == ">=")
				tempcon.op = GreatEqual;
			else
				return;
			flag = cutword(input, word);
			if (!flag)
				return;
			if (word == "\'"){
				flag = cutstr(input, word);
				if (!flag)
					return;
					tempcon.value = word;
					flag = cutword(input, word);
					if (!flag || word != "\'")
						return;
				}
			else
				tempcon.value = word;
			conditions.push_back(tempcon);
			flag = cutword(input, word);
			if (!flag){
				operation = SELECT;                               //select  all 
				return;
			}
			while (word == "and"){
				flag = cutword(input, word);
				if (!flag)
					return;
				tempnum = catalog.getcolunmnum(tableinfo, word);
				colname = word;
				if (tempnum == -1){
					operation = NOCOLUMN;
					return;
				}
				tempcon.columnNum = tempnum;
				flag = cutword(input, word);
				if (!flag)
					return;
				if (word == "<")
					tempcon.op = LessThan;
				else if (word == "<=")
					tempcon.op = LessEqual;
				else if (word == "<>")
					tempcon.op = NotEqual;
				else if (word == "=")
					tempcon.op = Equal;
				else if (word == ">")
					tempcon.op = GreatThan;
				else if (word == ">=")
					tempcon.op = GreatEqual;
				else
					return;
				flag = cutword(input, word);
				if (!flag)
					return;
				if (word == "\'"){
					flag = cutstr(input, word);
					if (!flag)
						return;
					tempcon.value = word;
					flag = cutword(input, word);
					if (!flag || word != "\'")
						return;
				}
				else
					tempcon.value = word;
				conditions.push_back(tempcon);
				flag = cutword(input, word);
				if (!flag){
					operation = SELECT;
					return;
				}
			}

		}
	}
	else if (word == "insert"){                                        //insert  
		 
		operation = INSERTRR;
		flag = cutword(input, word);
		if (!flag || word != "into")
			return;
		flag = cutword(input, word);
		if (!flag)
			return;
		tablename = word;
		if (!catalog.isexisttable(word)){
			operation = NOTABLE;
			return;
		}
		tableinfo = catalog.gettable(word);
		flag = cutword(input, word);
		if (!flag || word != "values")
			return;
		flag = cutword(input, word);
		if (!flag || word != "(")
			return;
		flag = cutword(input, word);
		if (!flag)
			return;
		if (word == "\'"){
			flag = cutstr(input, word);
			if (!flag)
				return;
			temprow.Value = word;
			temprow.type = CHAR;
			flag = cutword(input, word);
			if (!flag || word != "\'")
				return;
		}
		else
		{
			temprow.Value = word;
			temprow.type = INT;
			if (isfloat(word)) {
				temprow.type = FLOAT;
			}
		}
		row.columns.push_back(temprow);
		flag = cutword(input, word);
		if (!flag)
			return;
		while (word == ","){
			flag = cutword(input, word);
			if (!flag)
				return;
			if (word == "\'"){
				flag = cutstr(input, word);
				if (!flag)
					return;
				temprow.Value = word;
				temprow.type = CHAR;
				flag = cutword(input, word);
				if (!flag || word != "\'")
					return;
			}
			else
			{
				temprow.Value = word;
				temprow.type = INT;
				if (isfloat(word)) {
					temprow.type = FLOAT;
				}
			}
			row.columns.push_back(temprow);
			flag = cutword(input, word);
			if (!flag)
				return;
		}
		if (word != ")")
			return;
		if (row.columns.size() != tableinfo.attributes.size()){
			operation = INSERTSIZERR;
			return;
		}
		for (int j = 0; j < tableinfo.attributes.size(); j++){
			if (tableinfo.attributes[j].type==FLOAT && row.columns[j].type==INT)
				continue;
			//cout << "table :" << tableinfo.attributes[j].type << "  row:" << row.columns[j].type << endl;
			if (tableinfo.attributes[j].type != row.columns[j].type){
				operation = INSERTTYPERR;
				wrongtype = j;
				return;
			}

		}
		for (int i = 0; i<tableinfo.attributes.size(); i++){
			if (tableinfo.attributes[i].isPrimeryKey){
				primarykeypos = i;
				tempcon.columnNum= i;
				tempcon.op = Equal;
				tempcon.value = row.columns[i].Value;
				conditions.push_back(tempcon);
			}
			if (tableinfo.attributes[i].isUnique != 1 && tableinfo.attributes[i].isUnique){
				uniquekeypos = i;
				tempcon.columnNum = i;
				tempcon.op = Equal;
				tempcon.value = row.columns[i].Value;
				uniqueconditions.push_back(tempcon);
			}
		}
		//cout << "!!!input : " << input << endl;
		flag = cutword(input, word);
		if (!flag){
			operation = INSERT;
			return;
		}
	}
	else if (word == "delete"){													//delete 
		operation = DELETERR; 
		flag = cutword(input, word);
		if (!flag || word != "from")
			return;
		flag = cutword(input, word);
		if (!flag)
			return;
		tablename = word;
		if (!catalog.isexisttable(word)){
			operation = NOTABLE;
			return;
		}
		tableinfo = catalog.gettable(word);
		flag = cutword(input, word);
		if (!flag){
			operation = DELETE;
			return;
		}
		if (word != "where")
			return;
		else{
			flag = cutword(input, word);
			if (!flag)
				return;
			tempnum = catalog.getcolunmnum(tableinfo, word);
			colname = word;
			if (tempnum == -1){
				operation = NOCOLUMN;
				return;
			}
			tempcon.columnNum = tempnum;
			flag = cutword(input, word);
			if (!flag)
				return;
			if (word == "<")
				tempcon.op = LessThan;
			else if (word == "<=")
				tempcon.op = LessEqual;
			else if (word == "<>")
				tempcon.op = NotEqual;
			else if (word == "=")
				tempcon.op = Equal;
			else if (word == ">")
				tempcon.op = GreatThan;
			else if (word == ">=")
				tempcon.op = GreatEqual;
			else
				return;
			flag = cutword(input, word);
			if (!flag)
				return;
			if (word == "\'"){
				flag = cutstr(input, word);
				if (!flag)
					return;
				tempcon.value = word;
				flag = cutword(input, word);
				if (!flag || word != "\'")
					return;
			}
			else
				tempcon.value = word;
			conditions.push_back(tempcon);
			flag = cutword(input, word);
			if (!flag){
				operation = DELETE;
				return;
			}
			while (word == "and"){
				flag = cutword(input, word);
				if (!flag)
					return;
				tempnum = catalog.getcolunmnum(tableinfo, word);
				colname = word;
				if (tempnum == -1){
					operation = NOCOLUMN;
					return;
				}
				tempcon.columnNum = tempnum;
				flag = cutword(input, word);
				if (!flag)
					return;
				if (word == "<")
					tempcon.op = LessThan;
				else if (word == "<=")
					tempcon.op = LessEqual;
				else if (word == "<>")
					tempcon.op = NotEqual;
				else if (word == "=")
					tempcon.op = Equal;
				else if (word == ">")
					tempcon.op = GreatThan;
				else if (word == ">=")
					tempcon.op = GreatEqual;
				else
					return;
				flag = cutword(input, word);
				if (!flag)
					return;
				if (word == "\'"){
					flag = cutstr(input, word);
					if (!flag)
						return;
					tempcon.value = word;
					flag = cutword(input, word);
					if (!flag || word != "\'")
						return;
				}
				else
					tempcon.value = word;
				conditions.push_back(tempcon);
				flag = cutword(input, word);
				if (!flag){
					operation = DELETE;
					return;
				}
			}
		}
	}
	else if (word == "execfile"){                              //执行文件操作
		flag = cutword(input, word);
		operation = EXECRR;
		if (!flag)
			return;
		filename = word;
		flag = cutword(input, word);
		if (!flag){
			operation = EXEC;
			return;
		}

	}
	return;
}


bool interpreter::cutword(string &input, string &word){                       //把input一个单词一个单词地分割
	word.clear();
	int i = 0;
	while (input[i] == ' ' || input[i] == '\t' || input[i] == 10 || input[i] == 13){
		i++;
		if (i == input.length())
			break;
	}
	if (i == input.length())
		return false;
	else{
		switch (input[i]){
		case ',':
		case '(':
		case ')':
		case '*':
		case '=':
		case '\'':
			word += input[i];
			input.erase(0, i + 1);
			break;
		case '<':
			word += input[i];
			i++;
			if (input[i] == '=' || input[i] == '>')
			{
				word += input[i];
				input.erase(0, i + 1);
			}
			else
			{
				input.erase(0, i);
			}
			break;
		case '>':
			word += input[i];
			i++;
			if (input[i] == '=')
			{
				word += input[i];
				input.erase(0, i + 1);
			}
			else
			{
				input.erase(0, i);
			}
			break;
		default:
			do{
				word += input[i];
				if (i < input.length())
					i++;
			} while (input[i] != '*' && input[i] != ',' && input[i] != '(' && input[i] != ')'
				&& input[i] != ' ' && input[i] != '\t' && input[i] != '=' && input[i] != '>'
				&& input[i] != '<' && input[i] != '\'' && input[i] != 10 && input[i] != 13);
			input.erase(0, i);
		}
		return true;
	}
}


int isint(string word){                                     //判断是不是INT类型
	int i;
	for (i = 0; i < word.length(); i++){
		if (word[i] != '0'&&word[i] != '1'&&word[i] != '2'&&word[i] != '3'&&word[i] != '4'&&word[i] != '5'&&word[i] != '6'&&word[i] != '7'&&word[i] != '8'&&word[i] != '9')
			return 0; 
	}
	return 1;
}

int trance(string word){                                    //把字符串转化为数字
	int sum=0;
	int i;
	for (i = 0; i < word.length(); i++){
		sum = sum * 10 + word[i] - '0';
	}
	return sum;
}

bool interpreter::cutstr(string &input, string &word){           //获取字符串 
	word.clear();
	int i = 0;
	if (input[0] == '\'') {
		word = "";
		return true;
	}
	else {
		while (input[i] != '\''&&i<input.length()){
			word += input[i];
			i++;
		}
		if (input[i] == '\''){
			input.erase(0, i);
			return true;
		}
		else
			return false;
}

}


int  isfloat(string word){                                   //判断是否为整型变量
	int i; 
	for (i = 0; i < word.length(); i++){
		if (word[i] == '.')
			return true;
	}
	return false;
}