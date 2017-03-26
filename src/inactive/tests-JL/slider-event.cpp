/*
 * @file slider-event.cpp
 *
 * @author Jeff Lutgen
 */

#include "gevents.h"
#include "ginteractors.h"
#include "gwindow.h"

int main()
{
    GWindow gw;
    GSlider *slider = new GSlider();
    slider->setActionCommand("slidey");
    gw.addToRegion(slider, "SOUTH");
    while (true)
        GEvent e = waitForEvent();
    return 0;
}








