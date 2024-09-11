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

// return a random number between 0 and limit inclusive (https://stackoverflow.com/questions/2999075/generate-a-random-number-within-range)
int rand_lim(int limit) {
	int divisor = RAND_MAX / (limit + 1);
	int retval;
	do {
		retval = rand() / divisor;
	} while (retval > limit);
	return retval;
}

struct cell {
	int num;
	int* choices;
	int length;
};

struct cell cell(int num) {
	struct cell cell;
	cell.choices = malloc(9 * sizeof(int));
	cell.length = 9;
	cell.num = num;
	for (int i = 0; i < 9; i++) 
		cell.choices[i] = i;
	return cell;
}

void elim(struct cell board[], int cellNum, int choice);
void prop(struct cell board[], int cellNum, int choice);

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

void prop(struct cell board[], int cellNum, int choice) {
	int col = cellNum % 9;
	int row = (cellNum - col) / 9;
	int jBox = col;
	int iBox = row;
	while (jBox % 3 != 0) --jBox;
	while (iBox % 3 != 0) --iBox;
	int jBoxEnd = jBox + 3;
	int iBoxEnd = iBox + 3;
	for (int i = 0; i < 9; i++) {
		if (i < jBox || i > jBox + 2) 
			elim(board, row * 9 + i, choice);
	}
	for (int i = 0; i < 9; i++) {
		if (i < iBox || i > iBox + 2)
			elim(board, i * 9 + col, choice);
	}
	while (jBox < jBoxEnd) {
		while (iBox < iBoxEnd) {
			elim(board, iBox * 9 + jBox, choice);
			iBox++;
		}
		jBox++;
		iBox -= 3;
	}
}

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
			DrawText(TextFormat("%i", board[i].choices[0]), originX  + 50 - MeasureText(TextFormat("%i", board[i].choices[0]), 50) / 2, originY + 25, 50, WHITE);
			continue;
		}
		for (int j = 0; j < board[i].length; j++) {
			int colCell = j % 3;
			DrawText(TextFormat("%i", board[i].choices[j]), originX + 10 + colCell * 33, originY + 5 + (j - colCell) / 3 * 33, fontSize, WHITE);
		}
	}
}

int main() {
	// Seed the RNG
	srand(time(NULL));

	struct cell board[81];

	// Track cells with the lowest entropy
	int minEntsCount = 81;
	struct cell** minEnts = malloc(sizeof(struct cell*) * minEntsCount);

	// Check if memory was successfully allocated or compiler cries
	if (minEnts == NULL) {
		printf("Memory not allocated");
		return;
	}

	for (int i = 0; i < 81; i++) {
		board[i] = cell(i);
		minEnts[i] = &board[i];
	}

	// Tell the window to use vysnc and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(1920, 1080, "sudoku");
	
	int minEnt = 9;
	int chosen = 0;
	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		// drawing
		BeginDrawing();

		// Setup the backbuffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);
		
		show(board);

		int singularCount = 0;
		for (int i = 0; i < 81; i++) {
			if (board[i].length == 1)
				singularCount++;
		}
		DrawText(TextFormat("The current lowest entropy is: %i", minEnt), 0, 1000, 30, WHITE);
		DrawText(TextFormat("Cell %i has been chosen", chosen), 0, 950, 30, WHITE);

		if (singularCount < 81) {
			int iCell = rand_lim(minEntsCount - 1);
			chosen = minEnts[iCell]->num;
			minEnts[iCell]->choices[0] = minEnts[iCell]->choices[rand_lim(minEnts[iCell]->length - 1)];
			minEnts[iCell]->length = 1;
			prop(board, minEnts[iCell]->num, minEnts[iCell]->choices[0]);

			minEnt = 9;
			for (int i = 0; i < 81; i++) {
				if (board[i].length < minEnt && board[i].length > 1)
					minEnt = board[i].length;
			}
			minEntsCount = 0;
			for (int i = 0; i < 81; i++) {
				if (board[i].length == minEnt)
					minEntsCount++;
			}
			free(minEnts);
			minEnts = malloc(sizeof(struct cell*) * minEntsCount);
			int iMinEnts = 0;
			for (int i = 0; i < 81; i++) {
				if (board[i].length == minEnt) {
					minEnts[iMinEnts] = &board[i];
					iMinEnts++;
				}
			}
		}

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// destory the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}