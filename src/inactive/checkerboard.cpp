/**************************
*   COSC 220
***************************
*
*   File: checkerboard.cpp
*/

#include <iostream>
#include "console.h"
#include "simpio.h"
#include "gwindow.h"

using namespace std;

const int WIN_W = 600;
const int WIN_H = 600;

void drawCheckerboard(int size, GWindow &gw) {
    // Draws a `size` by `size` checkerboard in the given GWindow.
    int cellW = WIN_W / size;
    int cellH = WIN_H / size;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            // draw one rectangle
            if ((i+j)%2 == 0) {
                gw.setColor("red");
            } else {
                gw.setColor("black");
            }
            gw.fillRect(i*cellW, j*cellH, cellW, cellH);
        }
    }
}

int main() {
    setConsoleExitProgramOnClose(true);
    setConsolePrintExceptions(true);
    setConsoleFont("Monospaced-20");
    ////////////////////////////////////////////////////////////

    // Declare a local variable `gw` of type GWindow,
    // create a GWindow object of a certain size,
    // and initialize `gw` to refer to it.
    GWindow gw(WIN_W, WIN_H);

    drawCheckerboard(8, gw);

    ////////////////////////////////////////////////////////////
    getLine("Program finished. Press ENTER to close window(s)");
    closeConsoleAndExit();
    return 0;
}
