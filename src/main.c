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
	int* choices;
	int length;
};

struct cell cell() {
	struct cell cell;
	cell.choices = malloc(9 * sizeof(int));
	cell.length = 9;
	for (int i = 0; i < 9; i++) 
		cell.choices[i] = i;
	return cell;
}

void elim(struct cell board[], int cellNum, int final);
void prop(struct cell board[], int cellNum, int final);

void elim(struct cell board[], int cellNum, int final) {
	if (board[cellNum].length == 1)
		return;
	int found = 0;
	for (int i = 0; i < board[cellNum].length; i++) {
		if (found)
			board[cellNum].choices[i - 1] = board[cellNum].choices[i];
		if (board[cellNum].choices[i] == final)
			found = 1;
	}
}

void prop(struct cell board[], int cellNum, int final) {
	int col = cellNum % 9;
	int row = (cellNum - col) / 9;
	int jBox = col;
	int iBox = row;
	int jBoxEnd = jBox + 3;
	int iBoxEnd = iBox + 3;
	while (jBox % 3 != 0) --jBox;
	while (iBox % 3 != 0) --iBox;
	for (int i = 0; i < 9; i++) {
		if (i < jBox || i > jBox + 2) 
			elim(board, row * 9 + i, final);
	}
	for (int i = 0; i < 9; i++) {
		if (i < iBox || i > iBox + 2)
			elim(board, i * 9 + col, final);
	}
	while (jBox < jBoxEnd) {
		while (iBox < iBoxEnd) {
			elim(board, iBox * 9 + jBox, final);
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

	for (int i = 0; i < 81; i++) {
		for (int j = 0; j < 9; j++) {

		}
	}
}

int main ()
{
	// Tell the window to use vysnc and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(1920, 1080, "sudoku");
	
	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		// drawing
		BeginDrawing();

		// Setup the backbuffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);
		
		show(NULL);

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// destory the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}