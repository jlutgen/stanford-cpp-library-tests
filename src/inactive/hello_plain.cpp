/**************************
*	COSC 220
***************************
*
*	File: hello_plain.cpp
*/

#include <iostream>
#include "vector.h"

using namespace std;

int main() {
    Vector<int> v;
    v.add(9);
    v.add(17);
    cout << "Hello, COSC 220!" << endl;
    cout << v << endl;
    return 0;
}


