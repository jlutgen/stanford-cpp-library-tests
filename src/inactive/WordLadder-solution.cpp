/*
 * File: WordLadder.cpp
 * --------------------
 * Name: [TODO: enter name here]
 *
 * Assignment 2, part 1: word ladder
 */

#include <iostream>
#include "lexicon.h"
#include "queue.h"
#include "simpio.h"
#include "vector.h"
#include "console.h"
#include "gwindow.h"
using namespace std;

Vector<string> findLadder(string start, string end);

int main() {
	setConsoleSize(600, 400);
    setConsoleFont("Monospaced-12");
    setConsolePrintExceptions(true);
	/////////////////////////////////////////////////

	// [TODO: fill in the code]

	string start, end;
	Vector<string> ladder;
	while(true) {
		start = getLine("Enter start word: (RETURN to quit): ");
		if (start.empty()) break;
		end = getLine("Enter end word: (RETURN to quit): ");
		if (end.empty()) break;
		ladder = findLadder(start, end);
		if (ladder.isEmpty()) cout << "No ladder found." << endl;
		else {
			for  (string word : ladder) {
				cout << word << " ";
			}
			cout << endl;
		}
	}

    /////////////////////////////////////////////////
    //getLine("\n *** Program finished. Press [Enter] to close the window... ");
    //closeConsoleAndExit();

	return 0;
}

//	Create an empty queue.
//	Add the start word to the end of the queue.
//	while (the queue is not empty) {
//		Dequeue the first ladder from the queue.
//		if (the final word in this ladder is the destination word) {
//			Return this ladder as the solution.
//		}
//		for (each word in the lexicon of English words that differs by one letter) {
//			if (that word has not already been used in a ladder) {
//				Create a copy of the current ladder.
//				Add the new word to the end of the copy.
//				Add the new ladder to the end of the queue.
//			}
//		}
//	}
//	Report that no word ladder exists.
Vector<string> findLadder(string start, string end) {
	Queue<Vector<string> > ladders;
	Vector<string> curLadder;
	Vector<string> ladderCopy;
	Lexicon usedWords;
	Lexicon english = Lexicon("EnglishWords.dat");
	string candidate;

	curLadder.add(start);
	ladders.enqueue(curLadder);

	while (!ladders.isEmpty()) {
		curLadder = ladders.dequeue();
		string lastWord = curLadder.get(curLadder.size() - 1);
		if (lastWord == end) {
			return curLadder;
		}
		for (int i = 0; i < lastWord.length(); i++) {
			for (char ch = 'a'; ch <= 'z'; ch++) {
				if (lastWord[i] == ch) continue;
				candidate = lastWord;
				candidate[i] = ch;
				if (english.contains(candidate) && !usedWords.contains(candidate)) {
					ladderCopy = curLadder;
					ladderCopy.add(candidate);
					usedWords.add(candidate);
					ladders.enqueue(ladderCopy);
					/*cout << "LAD: ";
					foreach(string w in ladderCopy) {
						cout << w << " ";
					}
					cout << endl;*/
				}
			}
		}
	}
	return Vector<string>();
}
