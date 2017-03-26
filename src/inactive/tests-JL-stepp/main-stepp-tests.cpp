/*
 * Test file for verifying the Stanford C++ lib functionality.
 */

#include "console.h"
#include "simpio.h"
#include "testcases.h"
#include "testcases-jl.h"
#include "private/version.h"
#include <iostream>
#include <fstream>
using namespace std;

int main() {
    setConsoleSize(1000, 600);
    setConsolePrintExceptions(true);
    setConsoleLocation(-1, -1);
    //setConsoleWindowTitle("Tests!");
    
    while (true) {
        cout << "c) collections" << endl;
        cout << "e) cout / cerr mix" << endl;
        cout << "f) read entire file into string" << endl;
        cout << "g) gui" << endl;
        cout << "i) cin / cout" << endl;
        cout << "n) segfault (NULL ptr)" << endl;
        cout << "o) divide by zero" << endl;
        cout << "r) stream insertion (collections)" << endl;
        cout << "s) stack overflow" << endl;
        cout << "t) throw exception (Vector index out of range)" << endl;
        cout << "u) urlstream" << endl;
        string cmd = getLine("Command (Enter to quit)?");
        if (cmd.empty()) {
            break;
        } else if (cmd == "c") {
            compareTest();
            foreachTest();
            hashCodeTest();
        } else if (cmd == "e") {
            coutCerrMixTest();
        } else if (cmd =="f") {
            readEntireFileTest();
        } else if (cmd == "g") {
            //fileDialogTest();
            gbufferedImageTest();
            //goptionpaneTest();
        } else if (cmd == "i") {
            cinOutTest();
        } else if (cmd == "n") {
            segfaultTest();
        } else if (cmd == "o") {
            segfaultTest(SIGFPE);
        } else if (cmd == "r") {
            streamInsertionLexiconTest();
            streamInsertionOthersTest();
        } else if (cmd == "s") {
            //getPlatform()->setStackSize(1024*1024*128);
            stackOverflowTest();
        } else if (cmd == "t") {
            exceptionTest();
        } else if (cmd == "u") {
//            urlstreamTest();
        }
    }
    
    cout << "All done." << endl;
    return 0;
}
