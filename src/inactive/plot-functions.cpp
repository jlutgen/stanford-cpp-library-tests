/**************************
*   COSC 220
***************************
*
*   File: plot-functions.cpp
*/

#include <iostream>
#include <cmath> // for sqrt(), sin(), M_PI

#include "console.h"
#include "gwindow.h"
#include "simpio.h"

using namespace std;

void plotSin(double start, double stop, GWindow & gw);
void plotSqrt(double start, double stop, GWindow & gw);

// generic plot function. Takes function to be plotted
// as an argument
void plot(double f(double), double start, double stop, GWindow & gw);

double mySweetF(double x) {
    return sin(10*x) * cos(3*x);
}

const double INCR = 0.01; // x-distance between successive points

int main() {
    setConsoleExitProgramOnClose(true);
    setConsolePrintExceptions(true);
    setConsoleSize(500, 100);
    setConsoleLocation(10, 370);
    /////////////////////////////////////////////////

    GWindow gw(500, 350);
    plot(mySweetF, 0, 2 * M_PI, gw);

    /////////////////////////////////////////////////
    cout << endl;
    getLine("Press [Enter] to close the window(s)... ");
    exitGraphics();
    return 0;
}

void plotSin(double start, double stop, GWindow & gw) {
    double centerY = gw.getHeight() / 2;
    double scaleFactor = (stop - start) / gw.getWidth();
    double x = start;
    double y = sin(x);
    while (x < stop) {
        double nextX = x + INCR;
        double nextY = sin(nextX);

        // convert (x, y) and (nextX, nextY) to graphics window coordinates
        double xG, yG, nextXG, nextYG;
        xG = (x - start) / scaleFactor;
        nextXG = (nextX - start) / scaleFactor;
        yG = centerY - (y / scaleFactor);
        nextYG = centerY - (nextY / scaleFactor);

        gw.drawLine(xG, yG, nextXG, nextYG);
        x = nextX;
        y = nextY;
    }
}

void plotSqrt(double start, double stop, GWindow & gw) {
    double centerY = gw.getHeight() / 2;
    double scaleFactor = (stop - start) / gw.getWidth();
    double x = start;
    double y = sqrt(x);
    while (x < stop) {
        double nextX = x + INCR;
        double nextY = sqrt(nextX);

        // convert (x, y) and (nextX, nextY) to graphics window coordinates
        double xG, yG, nextXG, nextYG;
        xG = (x - start) / scaleFactor;
        nextXG = (nextX - start) / scaleFactor;
        yG = centerY - (y / scaleFactor);
        nextYG = centerY - (nextY / scaleFactor);

        gw.drawLine(xG, yG, nextXG, nextYG);
        x = nextX;
        y = nextY;
    }
}

void plot(double f(double), double start, double stop, GWindow & gw) {
    // general-purpose plotting function
    double centerY = gw.getHeight() / 2;
    double scaleFactor = (stop - start) / gw.getWidth();
    double x = start;
    double y = f(x);
    while (x < stop) {
        double nextX = x + INCR;
        double nextY = f(nextX);

        // convert (x, y) and (nextX, nextY) to graphics window coordinates
        double xG, yG, nextXG, nextYG;
        xG = (x - start) / scaleFactor;
        nextXG = (nextX - start) / scaleFactor;
        yG = centerY - (y / scaleFactor);
        nextYG = centerY - (nextY / scaleFactor);

        gw.drawLine(xG, yG, nextXG, nextYG);
        x = nextX;
        y = nextY;
    }
}
