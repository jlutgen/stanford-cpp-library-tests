/**************************
*   COSC 220
***************************
*
*   File: drawFractal.cpp
*
*/

#include <iostream>

#include "console.h"
#include "gwindow.h"
#include "simpio.h"

using namespace std;



void drawTriangle(double x, double y, double w, double h, GWindow & gw) {
    gw.drawLine(x, y + h, x + w/2, y);
    gw.drawLine(x + w/2, y, x + w, y + h);
    gw.drawLine(x + w, y + h, x, y + h);
}

void drawFractal(double x, double y, double w, double h, GWindow & gw) {
    if (w < 1 || h < 1) return;
    drawTriangle(x, y, w, h, gw);
    drawFractal(x + w/4, y, w/2, h/2, gw); // T
    drawFractal(x, y + h/2, w/2, h/2, gw); // L
    drawFractal(x + w/2, y + h/2, w/2, h/2, gw); // R
}

int main() {
    setConsoleExitProgramOnClose(true);
    setConsolePrintExceptions(true);
    /////////////////////////////////////////////////
    GWindow gw(600, 400);
    drawFractal(5, 5, 590, 390, gw);

    /////////////////////////////////////////////////
    cout << endl;
    getLine("Press [Enter] to close the window... ");
    exitGraphics();
    return 0;
}

