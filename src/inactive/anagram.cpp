/*
 *  File: anagram.cpp
 *  ----------------------
 *
 *  Checks whether a given string is an anagram of
 *  an English word, using a recursive backtracking
 *  algorithm.
 */
#include <iostream>
#include "simpio.h"
#include "console.h"
#include "lexicon.h"
#include "strlib.h"

using namespace std;

bool isAnagram(string soFar, string rest, Lexicon & lex) {
    if (rest == "") {
        cout << soFar << endl;
        if (lex.contains(soFar))
            return true;
    }
    for (int i = 0; i < rest.length(); i++) {
        // remaining = rest with rest[i] deleted
        string remaining = rest.substr(0, i) + rest.substr(i+1);
        if (isAnagram(soFar + rest[i], remaining, lex))
            return true;
    }
    return false;
}

// wrapper function
bool isAnagram(string s) {
    Lexicon english("EnglishWords.dat");
    return isAnagram("", s, english);
}

int main() {
    setConsoleSize(600, 700);
    setConsoleFont("Monospaced-14");
    /////////////////////////////////

    while (true) {
        string s = getLine("string? ");
        if (s.empty())
            break;
        if (isAnagram(s))
            cout << "anagram!" << endl;
        else
            cout << "not an anagram" << endl;
    }

    /////////////////////////////////
    cout << endl;
    getLine("Press [Enter] to close the window... ");
    closeConsoleAndExit();
    return 0;
}
