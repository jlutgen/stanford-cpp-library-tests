#include "testcases-jl.h"
#include "simpio.h"
#include "filelib.h"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

void readEntireFileTest() {
    ifstream ifs;
    string filename = getLine("File to read in: ");
    string wholeThing = readEntireFile(filename);
    cout << "Here it comes..." << endl;
    cout << wholeThing << endl;
}

