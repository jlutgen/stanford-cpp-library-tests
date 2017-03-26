/*
 * @file key-event-in-canvas.cpp
 *
 * @author Jeff Lutgen
 */

#include <iostream>
#include "console.h"
#include "gevents.h"
#include "gwindow.h"

using namespace std;

int main() {
    setConsoleLocation(10, 600);
    GWindow gw;
    gw.setWindowTitle("Key Event Test");
    gw.add(new GLabel("Click in here and type!", 20, 20));
    cout << "Ready." << endl;
    while (true) {
        GEvent e = waitForEvent();
        EventType type = e.getEventType();
        if (e.getEventClass() == KEY_EVENT) {
            GKeyEvent ke(e); // "private" (i.e., undocumented) constructor, but still ...
            std::cout << e.toString() << std::endl;
            cout << "Key event: " << ke.toString() << endl;
            cout << "Window: " << ke.getGWindow().getWindowTitle() << endl;
        }
    }
    return 0;
}







