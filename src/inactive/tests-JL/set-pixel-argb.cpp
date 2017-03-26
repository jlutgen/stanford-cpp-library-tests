/*
 * @file set-pixel-argb.cpp
 *
 * @author Jeff Lutgen
 */

//#include "gbufferedimage.h"
#include "gwindow.h"
#include "console.h"

int main()
{
    GWindow gw;
    for (int x = 0; x < 20; x++)
        for (int y = 0; y < 20; y++)
            gw.setPixelARGB(x, y, 0xff00ff00);
    for (int x = 10; x < 30; x++)
        for (int y = 10; y < 30; y++)
            gw.setPixelARGB(x, y, 0x700000ff);
    return 0;
}








