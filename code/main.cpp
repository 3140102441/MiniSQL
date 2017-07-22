#include "API.h"
#include "CatalogManage.h"
#include "Minisql.h"
#include <iostream>
#include <fstream>
#include <time.h>
using namespace std;
string getinput();

interpreter interfaces;
extern CatalogManage  catalog;
API  api;

int main()
{
	clock_t Time_begin,Time_stop;
	double duration, insert_time = 0, del_time = 0, find_time = 0;
	string input,temp;
	int i = 0,f=20;
	int cnt = -1;
	int k;
	cout << "welcome to our MiniSql" << endl;                                //初始化
	while (1){
		cout << "MiniSql -> ";
		input.clear();
		input = getinput();
		interfaces.MakeInitilate();
		//cout << "input :" << input << endl;
		interfaces.analysis(input);												//调用interpreter
		if (interfaces.operation != EXEC){                            
			Time_begin = clock();
			api.execute(interfaces);                                                //调用API
			Time_stop = clock();
			duration = ((Time_stop-Time_begin)) / CLK_TCK;//测执行时间
			printf("Time cost:	%e\n",duration);
			if (api.isdone == true)
				break;
		}
		else{
			fstream infile(interfaces.filename, ios::in);                            //exec file 操作
			if (!infile){
				cout << "The file " << interfaces.filename << " is not exist.";
			}
			else{
				while (1){
					input.clear();
					interfaces.MakeInitilate();
					getline(infile,temp);

					while (temp.length() == 0)
						getline(cin, temp);
					input = "";
					while (1){
						for (i = temp.length() - 1; i >= 0 && (temp[i] == '\t' || temp[i] == ' '); i--){
						}
						if (temp[i] == ';'){
							temp[i] = ' ';
							input = input + temp;
							break;
						}
						else{
							input = input + temp;
						    input += " " ;
							getline(infile,temp);
						}
					}
					cout << "cnt = " << cnt++ << endl;
					interfaces.analysis(input);
					api.execute(interfaces);
					if (api.isdone == true)
						break;
				}
			}
			}
		}
	system("pause");
	return 0;
}

string getinput(){                                                   //得到‘；‘ 前的所有字符
	int i;
	string temp, input;
	getline(cin, temp);
	//cout << "temp:" <<hex<< temp << "end of temp" << endl;
	while (temp.length()==0)
		getline(cin, temp);
	input = "";
	while (1){
		for (i = temp.length() - 1; i >= 0 && (temp[i] == '\t' || temp[i] == ' '); i--){
		}
		if (temp[i] == ';'){
			temp[i] = ' ';
			input = input + temp;
			return input;
		}
		else{
			input = input + temp;
			input += " ";
			getline(cin, temp);
		}
	}
}
