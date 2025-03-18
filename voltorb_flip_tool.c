#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "mt_tool.h"

static unsigned int mtIdx;

typedef struct {
	int voltorbs;
	int twos;
	int threes;
	int maxFreeRow;
	int maxFreeTotal;
} boardParam;

static const boardParam level_1_params[10] = {
	{ 6, 3, 1, 3, 3 },
	{ 6, 0, 3, 2, 2 },
	{ 6, 5, 0, 3, 4 },
	{ 6, 2, 2, 3, 3 },
	{ 6, 4, 1, 3, 4 },
	{ 6, 3, 1, 3, 3 },
	{ 6, 0, 3, 2, 2 },
	{ 6, 5, 0, 3, 4 },
	{ 6, 2, 2, 3, 3 },
	{ 6, 4, 1, 3, 4 }
};



static void setMtIdx(unsigned int inIdx) {
	mtIdx = inIdx;
}

static unsigned int getMtIdx() {
	return mtIdx;
}

static int checkMtIdxEnd() {
	return mtIdx >= mtLen;
}

static uint32_t readMt(uint32_t* mt) {
	uint32_t ret = mt[mtIdx];
	mtIdx++;
	return mtTemper(ret);
}


static void placeTilesInBoard(char* board, char tile, int numTiles, uint32_t* mt) {
	int attempts = 0;
	int tilesPlaced = 0;
	while (tilesPlaced != numTiles) {
		uint32_t pos = readMt(mt);
		if (checkMtIdxEnd()) return;
		
		pos %= 25;
		if (board[pos] != '1') {
			attempts++;
			if (attempts == 100) return;
		} else {
			board[pos] = tile;
			tilesPlaced++;
		}
	}
}

static int checkBoard(char* board, int maxFreeRow, int maxFreeTotal) {
	int freeTotal = 0;
	int freeRow[5] = { 0, 0, 0, 0, 0 };
	int freeCol[5] = { 0, 0, 0, 0, 0 };
	
	int riskyRow[5] = { 0, 0, 0, 0, 0 };
	int riskyCol[5] = { 0, 0, 0, 0, 0 };
	
	for (int i = 0; i < 25; i++) {
		if (board[i] == 'V') {
			int row = i / 5;
			int col = i % 5;
			riskyRow[row] = 1;
			riskyCol[col] = 1;
		}
	}
	
	for (int i = 0; i < 25; i++) {
		if (board[i] == '2' || board[i] == '3') {
			int row = i / 5;
			int col = i % 5;
			
			if (!(riskyRow[row] && riskyCol[col])) {
				freeRow[row]++;
				if (freeRow[row] == maxFreeRow) {
					return 0;
				}
				
				freeCol[col]++;
				if (freeCol[col] == maxFreeRow) {
					return 0;
				}
				
				freeTotal++;
				if (freeTotal == maxFreeTotal) {
					return 0;
				}
			}
		}
	}
	
	return 1;
}


int vfGetNext(unsigned int inIdx) {
	uint32_t* mt = mtGetBuf();
	setMtIdx(inIdx);
	
	// Board param
	uint32_t n = readMt(mt);
	if (checkMtIdxEnd()) return -1;
	
	n %= 100;
	n = (10 - (n % 10)) + n;
	n = (n / 10) - 1;
	const boardParam* param = &level_1_params[n];
	
	
	// Generate board
	int attempts = 0;
	char board[25];
	while (1) {
		memset(board, '1', 25);
		placeTilesInBoard(board, 'V', param->voltorbs, mt);  if (checkMtIdxEnd()) return -1;
		placeTilesInBoard(board, '2', param->twos,     mt);  if (checkMtIdxEnd()) return -1;
		placeTilesInBoard(board, '3', param->threes,   mt);  if (checkMtIdxEnd()) return -1;
		
		if (checkBoard(board, param->maxFreeRow, param->maxFreeTotal)) {
			break;
		}
		
		if (attempts == 1000) {
			break;
		}
		
		attempts++;
	}
	
	return getMtIdx();
}
