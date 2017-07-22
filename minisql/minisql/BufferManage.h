//
//  main.cpp
//  BufferManger
//
//  Created by Mac on 6/11/16.
//  Copyright (c) 2016 Zhejiang University. All rights reserved.
//
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Minisql.h"

using namespace std;
class InsertPos {
public:
	int BlockNum;   // the position in bufferBlock
	int Position;   // the position in Values
};

class BufferManager {
private:
public:
	Block bufferBlock[MAXBLOCKNUM];
	friend class RecordManager;
	friend class IndexManager;

	BufferManager() {
		for (int i = 0; i < MAXBLOCKNUM; i++)
			bufferBlock[i].initialize();
	}

	~BufferManager() {
		for (int i = 0; i < MAXBLOCKNUM; i++)
			writeBack(i);
	}

	int writeBack(const int blockNum) {
		int k;
		if (!bufferBlock[blockNum].inUse) return NOTFOUND;
		//showBlock(blockNum);
		fstream fout(bufferBlock[blockNum].FileName.c_str(), ios::in|ios::out);
		//cout << "OFFSet:" << BLOCKSIZE * bufferBlock[blockNum].BlockOffset << endl;
		fout.seekp(BLOCKSIZE * bufferBlock[blockNum].BlockOffset, ios::beg);
		//fout.write(bufferBlock[blockNum].Values, BLOCKSIZE);
		fout << bufferBlock[blockNum].Values;
		fout.close();
		bufferBlock[blockNum].inUse = 0;
		return SUCCESS;
	}   // write back to files

	int getBlockNum(const string fileName, const int blockOffset) {
		int blockNum = getBlockNumInBuffer(fileName, blockOffset);
		if (blockNum == NOTFOUND) {
			blockNum = getEmptyBlock();
			readBlock(fileName, blockOffset, blockNum);
		}
		useBlock(blockNum);
		return blockNum;
	}

	int getBlockNumInBuffer(const string fileName, const int blockOffset) {
		for (int i = 0; i < MAXBLOCKNUM; i++)
			if (bufferBlock[i].FileName == fileName && bufferBlock[i].BlockOffset == blockOffset &&
				bufferBlock[i].inUse)
				return i;
		return NOTFOUND;
	}   // Get blockNum if the block is in the buffer

	int getEmptyBlock(const string fileName = "", const int blockOffset = NOTFOUND) {
		for (int i = 0; i < MAXBLOCKNUM; i++)
			if (!bufferBlock[i].inUse) return i;

		int maxLRU = 0, blockNum = 0;
		for (int i = 0; i < MAXBLOCKNUM; i++)
			if (bufferBlock[i].LRUValue > maxLRU && bufferBlock[i].FileName != fileName &&
				bufferBlock[i].BlockOffset != blockOffset) {
				blockNum = i;
				maxLRU = bufferBlock[i].LRUValue;
			}
		writeBack(blockNum);
		bufferBlock[blockNum].inUse = 0;
		return blockNum;
	}   // If the block we use is not in the buffer, get an idle blockNum for it

	int readBlock(const string fileName, const int blockOffset, int blockNum) {
		if (bufferBlock[blockNum].inUse)
			return UNSUCCESS;
		bufferBlock[blockNum].initialize();
		bufferBlock[blockNum].inUse = 1;
		bufferBlock[blockNum].FileName = fileName;
		bufferBlock[blockNum].BlockOffset = blockOffset;
		fstream fin(fileName.c_str(), ios::in);
		fin.seekg(BLOCKSIZE * blockOffset, ios::beg);
		fin.read(bufferBlock[blockNum].Values, BLOCKSIZE);
		fin.close();
		return SUCCESS;
	}   // read a block onto buffer

	void deleteBlock(const string fileName, const int blockOffset) {
		for (int i = 0; i < MAXBLOCKNUM; i++)
			if (bufferBlock[i].FileName == fileName && bufferBlock[i].BlockOffset == blockOffset)
				bufferBlock[i].inUse = 0;
	}   // delete a block on buffer

	void deleteFile(const string fileName) {
		for (int i = 0; i < MAXBLOCKNUM; i++)
			if (bufferBlock[i].FileName == fileName)
				bufferBlock[i].inUse = 0;
	}   // delete all blocks of this file

	int addBlockInFile(Table & table) {
		int blockNum = getEmptyBlock();
		bufferBlock[blockNum].initialize();
		bufferBlock[blockNum].FileName = table.name + ".table";
		bufferBlock[blockNum].inUse = 1;
		bufferBlock[blockNum].BlockOffset = table.blockNum++;
		return blockNum;
	}   // add one more block in the buffer for the table

	int addBlockInFile(Index & index) {
		int blockNum = getEmptyBlock();
		bufferBlock[blockNum].initialize();
		bufferBlock[blockNum].FileName = index.index_name + ".index";
		bufferBlock[blockNum].inUse = 1;
		bufferBlock[blockNum].BlockOffset = ++index.blockNum;
		return blockNum;
	}

	InsertPos getInsertPosition(Table & table) {
		InsertPos ipos;
		if (!table.blockNum) {
			ipos.BlockNum = addBlockInFile(table);
			ipos.Position = 0;
			return ipos;
		}
		string fileName = table.name + ".table";
		int memsize = table.totalLength + 1;    // the memory size to store a record
		int blockOffset = table.blockNum - 1;   // check where is the last block of the table
		int blockNum = getBlockNum(fileName, blockOffset);
		const int recordNum = BLOCKSIZE / memsize;
		for (int offset = 0; offset < recordNum; offset++) {
			int position = offset * memsize;
			if (bufferBlock[blockNum].Values[position] == NIL) {  // get an empty position for inserting
				ipos.BlockNum = blockNum;
				ipos.Position = position;
				return ipos;
			}
		}
		//  There is no empty position in the last block of the table, we need one more block.
		ipos.BlockNum = addBlockInFile(table);
		ipos.Position = 0;
		return ipos;
	}   // To insert a row to table, find the position in buffer

	void useBlock(int blockNum) {
		for (int i = 0; i < MAXBLOCKNUM; i++)
			bufferBlock[i].LRUValue++;
		bufferBlock[blockNum].LRUValue = 0;
	}

	//  for test ********************
	void showBlock(const int start, const int num) {
		const int MAXN = 30;
		if (0 <= start && start + num <= MAXBLOCKNUM) {
			if (num > MAXN) cout << "Can't show more than " << MAXN << " blocks." << endl;
			else for (int i = start; i < start + num; i++) showBlock(i);
		}
		else cout << "Out of Border." << endl;
	}

	void showBlock(const int blockNum) {
		cout << "BlockNum: " << blockNum << endl;
		cout << "InUse: " << bufferBlock[blockNum].inUse << endl;
		cout << "LRUValue: " << bufferBlock[blockNum].LRUValue << endl;
		cout << "Filename: " << bufferBlock[blockNum].FileName << endl;
		cout << "blockOffset: " << bufferBlock[blockNum].BlockOffset << endl;
		cout << bufferBlock[blockNum].Values << " \t***********" << endl;
	}

};
