/**************************
*	COSC 220
***************************
*
*	File: hello_console.cpp
*/

#include <iostream>
#include "console.h"
#include "simpio.h"

using namespace std;


int main() {
    setConsoleExitProgramOnClose(true);
    setConsolePrintExceptions(true);
    ////////////////////////////////////////////////////////////

    cout << "Hello, COSC 220!" << endl;

    ////////////////////////////////////////////////////////////
    getLine("\nProgram finished. Press ENTER to close window(s)");
    closeConsoleAndExit();
    return 0;
}


