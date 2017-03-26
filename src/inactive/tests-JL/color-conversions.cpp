/*
 * @file set-pixel-argb.cpp
 *
 * @author Jeff Lutgen
 */

#include <iomanip>
#include "gwindow.h"
#include "gobjects.h"
#include "console.h"
#include "vector.h"

using namespace std;

string strIntStr(string s) {
    return convertRGBToColor(convertColorToRGB(s));
}

string intStr(int n, bool hasAlpha = false) {
    return convertRGBToColor(n, hasAlpha);
}

int strInt(string s) {
    return convertColorToRGB(s);
}

void printS(string s) {
    cout << setw(12) << setfill(' ') << left << s << " "
         << "0x" << setfill('0') << setw(8) << right << strInt(s) << " " << strIntStr(s) << endl;
}

void printI(int i, bool hasAlpha = false) {
    cout << "0x" << setfill('0') << setw(8) << right << i << " " << intStr(i, hasAlpha) << endl;
}

void printSS(string s) {
    GOval oval(0, 0);
    oval.setColor(s);
    cout << setfill(' ') << setw(12) << left << s << " " << oval.getColor() << endl;
}

void printIS(int i, bool hasAlpha = false) {
    GOval oval(0, 0);
    oval.setColor(i, hasAlpha);
    string s = oval.getColor();
    cout << "0x" << setfill('0') << setw(8) << right << i << " "
         << setfill(' ') << setw(12) << left << s << endl;
}

int main()
{
    cout << hex;
    cout << setfill('0');
    Vector<string> v = {
                        "green",
                        "black",
                        "light gray",
                        "light_GRAY",
                        "#112233",
                        "#00112233",
                        "#88112233",
                        "#000000",
                        "#00000000",
                        "#ffffff",
                        "#00ffffff",
                        "#ffffffff"
                        };
    Vector<int> vv = {
                      0x00000000,
                      0x00112233,
                      0x01000000,
                      0x00ffffff,
                      int(0x80bbccdd),
                      int(0xff4488cc),
                      int(0xffffffff)
                      };
//    for (string s: v)
//        printS(s);
//    cout << endl;
    cout << "convertRGBToColor(n)" << endl;
    for (int i: vv) {
        printI(i);
    }
    cout << endl << "convertRGBToColor(n, true)" << endl;
    for (int i: vv) {
        printI(i, true);
    }
    cout << endl << "GObject setColor(s) then getColor()" << endl;
    for (string s: v) {
        printSS(s);
    }
    cout << endl << "GObject setColor(n) then getColor()" << endl;
    for (int i: vv) {
        printIS(i);
    }
    cout << endl << "GObject setColor(n, true) then getColor()" << endl;
    for (int i: vv) {
        printIS(i, true);
    }
    return 0;

}








