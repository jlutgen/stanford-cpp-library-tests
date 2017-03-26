/**************************
*   COSC 220
***************************
*
*   File: random-ovals.cpp
*/

#include <iostream>
#include "console.h"
#include "simpio.h"
#include "gwindow.h"
#include "random.h"

using namespace std;

const int WIN_W = 800;
const int WIN_H = 600;

int randomARGB() {
    // Returns an int representing a color with random red, green,
    // and blue components, with an alpha component of 0x80, which
    // is halfway between 0x00 (completely transparent) and 0xff
    // (completely opaque).
    //
    // The int has the form 0x80rrggbb
    // (alpha = 0x80, red = 0xrr, green = 0xgg, blue = 0xbb).

    return 0x80000000 + randomInteger(0x000000, 0xffffff);
}

void drawRandomOval(GWindow &gw) {
    // Draws an oval of a random size at a random
    // location in the GWindow.

    int x = randomInteger(-WIN_H/4, WIN_W);
    int y = randomInteger(-WIN_H/4, WIN_H);
    int w = randomInteger(10, WIN_W/2);
    int h = randomInteger(10, WIN_H/2);

    gw.setColor(randomARGB());
    gw.fillOval(x, y, w, h);
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

    for (int i=0; i<500; i++) {
        drawRandomOval(gw);
        pause(5); // milliseconds
    }

    ////////////////////////////////////////////////////////////
    //getLine("Program finished. Press ENTER to close window(s)");
    //closeConsoleAndExit();
    return 0;
}
