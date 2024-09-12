/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

For a C++ project simply rename the file to .cpp and re-run the build script 

-- Copyright (c) 2020-2024 Jeffery Myers
--
--This software is provided "as-is", without any express or implied warranty. In no event 
--will the authors be held liable for any damages arising from the use of this software.

--Permission is granted to anyone to use this software for any purpose, including commercial 
--applications, and to alter it and redistribute it freely, subject to the following restrictions:

--  1. The origin of this software must not be misrepresented; you must not claim that you 
--  wrote the original software. If you use this software in a product, an acknowledgment 
--  in the product documentation would be appreciated but is not required.
--
--  2. Altered source versions must be plainly marked as such, and must not be misrepresented
--  as being the original software.
--
--  3. This notice may not be removed or altered from any source distribution.

*/

#include "raylib.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct cell {
	int num; // label in an event that its index in an array does not match its position on the board
	int* choices;
	int length;
};
// construct a cell
struct cell cell(int num) {
	struct cell cell;
	cell.choices = malloc(9 * sizeof(int));
	cell.length = 9;
	cell.num = num;
	for (int i = 0; i < 9; i++) cell.choices[i] = i;
	return cell;
}
// return a random number between 0 and limit inclusive (https://stackoverflow.com/questions/2999075/generate-a-random-number-within-range)
int rand_lim(int limit) {
	int divisor = RAND_MAX / (limit + 1);
	int retval;
	do {
		retval = rand() / divisor;
	} while (retval > limit);
	return retval;
}

void elim(struct cell board[], int cellNum, int choice);
void prop(struct cell board[], int cellNum, int choice);
// eliminate the possibility that a cell can contain this number
void elim(struct cell board[], int cellNum, int choice) {
	if (board[cellNum].length == 1)
		return;
	int found = 0;
	for (int i = 0; i < board[cellNum].length; i++) {
		if (board[cellNum].choices[i] == choice) {
			found = 1;
			board[cellNum].length--;
		}
		if (found)
			board[cellNum].choices[i] = board[cellNum].choices[i + 1];
	}
	if (board[cellNum].length == 1)
		prop(board, cellNum, board[cellNum].choices[0]);
}
// spread the news that all cells in the same row, column, and box as the collapsed cell can no longer contain its number 
void prop(struct cell board[], int cellNum, int choice) {
	int col = cellNum % 9;
	int row = cellNum / 9;
	int yBox = col - (col % 3);
	int xBox = row - (row % 3);
	for (int i = 0; i < 9; i++) {
		elim(board, row * 9 + i, choice);
		elim(board, i * 9 + col, choice);
		elim(board, (xBox + (i / 3)) * 9 + yBox + (i % 3), choice);
	}
}
// draw the sudoku board
void show(struct cell board[]) {
	for (int i = 0; i < 4; i++) DrawRectangle(i * 300, 0, 4, 900, WHITE);
	for (int i = 0; i < 4; i++) DrawRectangle(0, i * 300, 900, 4, WHITE);
	for (int i = 1; i < 9; i++) DrawLine(i * 100 + 2, 0, i * 100 + 2, 900, WHITE);
	for (int i = 1; i < 9; i++) DrawLine(0, i * 100 + 2, 900, i * 100 + 2, WHITE);
	int fontSize = 900 / 27;
	for (int i = 0; i < 81; i++) {
		int col = i % 9;
		int originX = col * 100;
		int originY = (i - col) / 9 * 100;
		if (board[i].length == 1) {
			DrawText(TextFormat("%i", board[i].choices[0]), originX + 50 - MeasureText(TextFormat("%i", board[i].choices[0]), 50) / 2, originY + 25, 50, WHITE);
			continue;
		}
		for (int j = 0; j < board[i].length; j++) {
			int colCell = j % 3;
			DrawText(TextFormat("%i", board[i].choices[j]), originX + 10 + colCell * 33, originY + 5 + (j - colCell) / 3 * 33, fontSize, WHITE);
		}
	}
}
// check if each row, col, and box on the sudoku board has consecutive numbers 0 - 8
char* validate(struct cell board[]) {
	char code[2];
	int sum = 0;
	for (int i = 0; i < 81; i++) {
		sum += board[i].choices[0];
		if ((i + 1) % 9 == 0) {
			if (sum != 36) {
				code[0] = 1;
				code[1] = (i / 9) + 1;
				return code;
			}
			sum = 0;
		}
	}
	sum = 0;
	for (int i = 0; i < 81; i++) {
		sum += board[((i % 9) * 9) + (i / 9)].choices[0];
		if ((i + 1) % 9 == 0) {
			if (sum != 36) {
				code[0] = 2;
				code[1] = (i % 9) + 1;
				return code;
			}
			sum = 0;
		}
	}
	sum = 0;
	int origins[9] = { 0,3,6,27,30,33,54,57,60 }; // numbers of the top left cells of each box
	for (int i = 0; i < 9; i++) {
		int col = origins[i] % 9;
		int row = origins[i] / 9;
		for (int j = 0; j < 9; j++)
			sum += board[(row + (j / 3)) * 9 + col + (j % 3)].choices[0];
		if (sum != 36) {
			code[0] = 3;
			code[1] = i + 1;
			return code;
		}
		sum = 0;
	}
	code[0] = 0;
	code[1] = 0;
	return code;
}

int main() {
	srand(time(NULL)); 
	struct cell board[81];
	struct cell** minCells = NULL;
	int minCellCount = 0;
	int leastEnt = 9;
	int isValid = 0;
	// Tell the window to use vysnc and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	// Create the window and OpenGL context
	InitWindow(1920, 1080, "sudoku");
	// game loop
	while (!WindowShouldClose()) { // run the loop untill the user presses ESCAPE or presses the Close button on the 
		// drawing
		BeginDrawing();
		// Setup the backbuffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);
		
		show(board);
		DrawText("Generate Board: 'R'", 0, 950, 30, WHITE);
		if (isValid == 0 || (isValid == 1 && IsKeyPressed(KEY_R) && minCells != NULL)) {
			isValid = 1;
			free(minCells);
			// Track cells with the lowest entropy
			minCellCount = 81;
			minCells = malloc(sizeof(struct cell*) * minCellCount);

			// Check if memory was successfully allocated first or compiler cries
			if (minCells == NULL)
				exit(1);

			for (int i = 0; i < 81; i++) {
				board[i] = cell(i);
				minCells[i] = &board[i];
			}
		}
		// Generation loop
		if (minCellCount > 0) {
			int iCell = rand_lim(minCellCount - 1);
			minCells[iCell]->choices[0] = minCells[iCell]->choices[rand_lim(minCells[iCell]->length - 1)];
			minCells[iCell]->length = 1;
			prop(board, minCells[iCell]->num, minCells[iCell]->choices[0]);

			leastEnt = 9;
			for (int i = 0; i < 81; i++) {
				if (board[i].length < leastEnt && board[i].length > 1)
					leastEnt = board[i].length;
			}
			minCellCount = 0;
			for (int i = 0; i < 81; i++) {
				if (board[i].length == leastEnt)
					minCellCount++;
			}
			free(minCells);
			minCells = malloc(sizeof(struct cell*) * minCellCount);
			int iMinEnts = 0;
			for (int i = 0; i < 81; i++) {
				if (board[i].length == leastEnt) {
					minCells[iMinEnts] = &board[i];
					iMinEnts++;
				}
			}
		}
		
		if (minCellCount == 0 && minCells != NULL) {
			char* check = validate(board);
			if (check[0] != 0) 
				isValid = 0;
		}
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}
	free(minCells);
	// destory the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}