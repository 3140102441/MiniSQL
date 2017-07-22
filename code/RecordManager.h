#pragma once
#include "Minisql.h"
#include "BufferManage.h"
extern BufferManager buf;
class RecordManager {
private:
	Row getSplitRow(Table table, string stringRow) {
		int startPos = 0;
		string attribute;
		Row splitRow;
		//cout << "stringRow = " << stringRow;
		for (int i = 0; i < table.attriNum; i++) {
			attribute = stringRow.substr(startPos, table.attributes[i].length);
			Col col;
			col.Value = attribute;
			splitRow.columns.push_back(col);
			startPos += table.attributes[i].length;
		}
		return splitRow;
	}   // For a row, split it into column of attrributes.

	string connectRow(Table table, Row splitRow) {
		string stringRow, tempRow;
		int attributeNum = (int)splitRow.columns.size();
		for (int i = 0; i < attributeNum; i++) {
			tempRow = splitRow.columns[i].Value;
			for (int j = (int)tempRow.length(); j < table.attributes[i].length; j++)
				tempRow += NIL;
			stringRow += tempRow;
		}
		return stringRow;
	}   // Given split rows, get the whole string row.

public:
	bool cmp(const Table table, const Row splitRow, const vector<Condition> conditions) {
		for (int i = 0; i < conditions.size(); i++) {
			int colNum = conditions[i].columnNum;
			string value1 = splitRow.columns[colNum].Value;
			string value2 = conditions[i].value;
			int pos;
			int maxlen1 = (int)splitRow.columns[colNum].Value.length();
			for (pos = 0; pos < maxlen1; pos++)
				if (value1[pos] == NIL) break;
			value1.erase(pos, maxlen1 - pos);
			switch (table.attributes[colNum].type) {
			case INT: {
				int int1 = atoi(value1.c_str());
				int int2 = atoi(value2.c_str());
				switch (conditions[i].op) {
				case LessThan:
					if (int1 >= int2) return 0;
					break;
				case LessEqual:
					if (int1 > int2) return 0;
					break;
				case GreatThan:
					if (int1 <= int2) return 0;
					break;
				case GreatEqual:
					if (int1 < int2) return 0;
					break;
				case Equal:
					if (int1 != int2) return 0;
					break;
				case NotEqual:
					if (int1 == int2) return 0;
					break;
				default:
					break;
				}
			}
					  break;
			case FLOAT: {
				float float1 = atof(value1.c_str());
				float float2 = atof(value2.c_str());
				switch (conditions[i].op) {
				case LessThan:
					if (float1 >= float2) return 0;
					break;
				case LessEqual:
					if (float1 > float2) return 0;
					break;
				case GreatThan:
					if (float1 <= float2) return 0;
					break;
				case GreatEqual:
					if (float1 < float2) return 0;
					break;
				case Equal:
					if (float1 != float2) return 0;
					break;
				case NotEqual:
					if (float1 == float2) return 0;
					break;
				default:
					break;
				}
			}
						break;
			case CHAR:
				switch (conditions[i].op) {
				case LessThan:
					if (value1 >= value2) return 0;
					break;
				case LessEqual:
					if (value1 > value2) return 0;
					break;
				case GreatThan:
					if (value1 <= value2) return 0;
					break;
				case GreatEqual:
					if (value1 < value2) return 0;
					break;
				case Equal:
					if (value1 != value2) return 0;
					break;
				case NotEqual:
					if (value1 == value2) return 0;
					break;
				default:
					break;
				}
				break;
			default:
				return 0;
				break;
			}
		}
		return 1;
	}

	Data select(const Table table) {
		string fileName = table.name + ".table";
		string stringRow;
		Row splitRow;
		Data datas;
		const int memsize = table.totalLength + 1;
		const int recordNum = BLOCKSIZE / memsize;

		for (int blockOffset = 0; blockOffset < table.blockNum; blockOffset++) {
			int blockNum = buf.getBlockNum(fileName, blockOffset);
			for (int recordOffset = 0; recordOffset < recordNum; recordOffset++) {
				int position = recordOffset * memsize;
				stringRow = buf.bufferBlock[blockNum].getValues(position, memsize);
				if (stringRow[0] == NIL) continue;
				stringRow.erase(stringRow.begin());
				splitRow = getSplitRow(table, stringRow);
				datas.rows.push_back(splitRow);
			}
		}
		return datas;
	}   // Select all the data.

	Data select(const Table table, const vector<Condition> conditions) {
		string fileName = table.name + ".table";
		string stringRow;
		Row splitRow;
		Data datas;
		const int memsize = table.totalLength + 1;
		const int recordNum = BLOCKSIZE / memsize;

		for (int blockOffset = 0; blockOffset < table.blockNum; blockOffset++) {
			int blockNum = buf.getBlockNum(fileName, blockOffset);
			for (int recordOffset = 0; recordOffset < recordNum; recordOffset++) {
				int position = recordOffset * memsize;
				stringRow = buf.bufferBlock[blockNum].getValues(position, memsize);
				//cout << "stringRow ! = " << stringRow << endl;
				if (stringRow[0] == NIL) continue;
				stringRow.erase(stringRow.begin());
				splitRow = getSplitRow(table, stringRow);
				if (cmp(table, splitRow, conditions))
					datas.rows.push_back(splitRow);
			}
		}
		return datas;
	}   // Select all the data by conditions.

	void insertRecord(Table & table, Row & splitRow) {
		string stringRow = connectRow(table, splitRow);
		string record = NOTNIL + stringRow;
		InsertPos ipos = buf.getInsertPosition(table);
		buf.bufferBlock[ipos.BlockNum].setValues(ipos.Position, record);
	}   // Insert a record to the table

	int deleteRecords(Table table) {
		string fileName = table.name + ".table";
		int cnt = 0;
		const int memsize = table.totalLength + 1;
		const int recordNum = BLOCKSIZE / memsize;

		for (int blockOffset = 0; blockOffset < table.blockNum; blockOffset++) {
			int blockNum = buf.getBlockNum(fileName, blockOffset);
			for (int recordOffset = 0; recordOffset < recordNum; recordOffset++) {
				int position = recordOffset * memsize;
				if (buf.bufferBlock[blockNum].getValue(position) == NOTNIL) {
					buf.bufferBlock[blockNum].setValue(position, NIL);
					cnt++;
				}
			}
		}
		return cnt;
	}   // Delete all records in the table and return the number of records.

	int deleteRecords(Table table, vector<Condition> conditions) {
		string fileName = table.name + ".table";
		int cnt = 0;
		const int memsize = table.totalLength + 1;
		const int recordNum = BLOCKSIZE / memsize;

		for (int blockOffset = 0; blockOffset < table.blockNum; blockOffset++) {
			int blockNum = buf.getBlockNum(fileName, blockOffset);
			for (int recordOffset = 0; recordOffset < recordNum; recordOffset++) {
				int position = recordOffset * memsize;
				if (buf.bufferBlock[blockNum].getValue(position) == NOTNIL) {
					string stringRow = buf.bufferBlock[blockNum].getValues(position, memsize);
					stringRow.erase(stringRow.begin());
					Row splitRow = getSplitRow(table, stringRow);
					if (cmp(table, splitRow, conditions)) {
						buf.bufferBlock[blockNum].setValue(position, NIL);
						cnt++;
					}
				}
			}
		}
		return cnt;
	}   // Delete the records satisfy the conditions in the table and return the number of records.

	void dropTable(Table table) {
		string fileName = table.name + ".table";
		if (remove(fileName.c_str()) != 0)
			perror("Error in deleting file\n");
		else buf.deleteFile(fileName);
	}   // delete the file and the blocks in buffer

	void createTable(Table table) {
		string fileName = table.name + ".table";
		ofstream fout(fileName.c_str());
		fout.close();
	}   // new a file

	void showDatas(const Data & datas) {
		if (datas.rows.size() == 0) {
			cout << "No datas" << endl;
			return;
		}
		for (int i = 0; i < datas.rows.size(); i++) {
			cout << i + 1 << " ";
			for (int j = 0; j < datas.rows[i].columns.size(); j++) {
				for (int k = 0; k < datas.rows[i].columns[j].Value.length(); k++)
					if (datas.rows[i].columns[j].Value[k] == NIL) break;
					else cout << datas.rows[i].columns[j].Value[k];
					cout << "\t";
			}
			cout << endl;
		}
	}
};
