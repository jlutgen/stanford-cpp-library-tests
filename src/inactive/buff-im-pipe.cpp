/*
 * @file buffered-image-test.cpp
 *
 * @author Jeff Lutgen
 */

/*
 * Loading large images can cause stack overflow. On Windows (mingw-g++), add
 * -Wl,--stack,8388608
 * to the linker options to increase the stack size to 8MB.
*/

#include <iostream>
#include "gwindow.h"
#include "console.h"
#include "gbufferedimage.h"
#include "gevents.h"
#include "simpio.h"
#include "random.h"

using namespace std;

GWindow *gw;

void test_save() {
    GBufferedImage *gbi = new GBufferedImage(10, 10, 100, 100, "red");
    gw->add(gbi);
    gbi->save("norcos.png");
    gbi->save("norcos.gif");
    gbi->save("norcos.bmp");
    gbi->save("norcos.jpg");
    gbi->save("norcos.tiff");
    gbi->save("norcos.pict");
    gbi->save("norcos.gif89");
}

void test_load() {
    GBufferedImage *gbi = new GBufferedImage();
    //gbi->load("images/homer.png");
    //gbi->load("images/avatar.gif");
    //gbi->load("images/earth.jpg");
    //gbi->load("images/test.bmp");
    //gbi->load("images/bergman.jpg");
    gbi->load("images/picture-of-kitten.jpg");
    //gbi->load("EnglishWords.dat");
    gw->add(gbi, 10, 10);
}

void test_diff() {
    gw->add(new GLabel("original"), 10, 30);
    gw->add(new GLabel("modified"), 140, 30);
    gw->add(new GLabel("diff"), 270, 30);
    GBufferedImage *original = new GBufferedImage(100, 100);
    GBufferedImage *modified = new GBufferedImage(100, 100);
    modified->fillRegion(10, 10, 30, 30, "green");
    modified->fillRegion(60, 60, 30, 30, "blue");
    GBufferedImage *diff = original->diff(*modified);
    gw->add(original, 10, 50);
    gw->add(modified, 140, 50);
    gw->add(diff, 270, 50);
}

void test_fill() {
    GBufferedImage *bi = new GBufferedImage(600, 600);
    gw->add(bi);
    getLine("ENTER to fill");
    bi->fill("red");
    getLine("ENTER to fill region");
    bi->fillRegion(20, 20, 600 - 40, 600 - 40, "green");
}

int randomCol() {
    return randomInteger(0x0, 0xffffff);
}

void test_setRGB() {
    GBufferedImage *bi = new GBufferedImage(600, 600);
    gw->add(bi);
    for (int i = 0; i < 600*600/2; i++) {
        int x = randomInteger(0, 599);
        int y = randomInteger(0, 599);
//        bi->setRGB(x, y, randomCol());
        bi->setRGB(x, y, "white");

    }
    for (int i=0; i<500; i++)
        bi->setRGB(i, 200, "white"); // horizontal line
}

int main() {
    //setConsolePrintExceptions(true);
    //setConsoleSize(getScreenWidth() - 710 - 10, 300);
    //setConsoleLocation(720, 470);
    //cout << "Ready." << endl;

    //getLine("ENTER to go");
    gw = new GWindow(710, 610);
//    gw->setVisible(false);
//    while (true) {
//        cout << "s) save" << endl;
//        cout << "l) load" << endl;
//        cout << "d) diff" << endl;
//        cout << "f) fill, fill region" << endl;
//        cout << "r) setRGB" << endl;

//        string cmd = getLine("Command (Enter to quit)?");
//        if (cmd.empty()) {
//            break;
//        } else if (cmd == "s") {
//            gw->setVisible(true);
//            gw->clear();
//            test_save();
//        } else if (cmd == "l") {
//            gw->setVisible(true);
//            gw->clear();
//            test_load();
//        } else if (cmd == "d") {
//            gw->setVisible(true);
//            gw->clear();
//            test_diff();
//        }
//        else if (cmd == "f") {
//            gw->setVisible(true);
//            gw->clear();
//            test_fill();
//        }
//        else if (cmd == "r") {
//            gw->setVisible(true);
//            gw->clear();
//            test_setRGB();
//        }
//    }

//    getLine("ENTER to exit");

    test_setRGB();


    exitGraphics();
    return 0;
}
