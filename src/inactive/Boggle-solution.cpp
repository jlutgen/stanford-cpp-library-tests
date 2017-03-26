/*
 * File: Boggle-solution.cpp
 * ----------------
 * JL solution
 */

#include <iostream>
#include <ctime>
#include "gboggle.h"
#include "grid.h"
#include "lexicon.h"
#include "point.h"
#include "random.h"
#include "simpio.h"
#include "vector.h"
#include "strlib.h"
#include "set.h"
#include "foreach.h"
#include "console.h"


using namespace std;

/* Constants */

const int BOGGLE_WINDOW_WIDTH = 650;
const int BOGGLE_WINDOW_HEIGHT = 350;
const int CONSOLE_WIDTH = 650;
const int CONSOLE_HEIGHT = 290;

const string STANDARD_CUBES[16]  =
{
  "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
  "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
  "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
  "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};

const string BIG_BOGGLE_CUBES[25]  =
{
  "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
  "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
  "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
  "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
  "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

/* Function prototypes */

void welcome();
void giveInstructions();
void initBoard(int numRows, int numCols);
void showLetters(Grid<char> & board);
Grid<char> createBoard(bool big);
void humanPlay(Lexicon & english, Lexicon & words, Grid<char> & board);
void computerPlay(Lexicon & englishWords, Lexicon & humanWords, Grid<char> & board);
void highlightCubes(Vector<Point> cubes);
Vector<Point> getNeighbors(int row, int col, Grid<char> & board);
bool canMakeWord(string word, Grid<char> & board, Vector<Point> & usedPositions);
bool canMakeWordStartingAt(int row, int col, string word, Grid<char> & board, Vector<Point> & usedPositions);
bool alreadyUsed(Point p, Vector<Point> used);
void findAllWordsStartingAt(int row, int col,
                            Grid<char> & board,
                            Vector<Point> & usedPositions,
                            Lexicon & allWords,
                            Lexicon & humanWords,
                            Lexicon & compWords,
                            string prefix = "");


/* Main program */

int main()
{
  //setConsoleExitProgramOnClose(true);
  //setConsolePrintExceptions(true);
  Lexicon englishWords("EnglishWords.dat");
  Lexicon humanWords;

  //setConsoleSize(CONSOLE_WIDTH, CONSOLE_HEIGHT);
  GWindow gw(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);
  initGBoggle(gw);
  setRandomSeed((int)time(NULL));

  welcome();
  giveInstructions();

  Grid<char> board = createBoard(false);
  initBoard(board.numRows(), board.numCols());
  showLetters(board);
  humanPlay(englishWords, humanWords, board);
  computerPlay(englishWords, humanWords, board);
  getLine("Thanks for playing! Hit ENTER to quit.");
  exitGraphics();
  return 0;
}

/*
 * Function: welcome
 * Usage: welcome();
 * -----------------
 * Print out a cheery welcome message.
 */

void welcome()
{
  cout << "Welcome!  You're about to play an intense game ";
  cout << "of mind-numbing Boggle.  The good news is that ";
  cout << "you might improve your vocabulary a bit.  The ";
  cout << "bad news is that you're probably going to lose ";
  cout << "miserably to this little dictionary-toting hunk ";
  cout << "of silicon.  If only YOU had a gig of RAM..." << endl << endl;
}

/*
 * Function: giveInstructions
 * Usage: giveInstructions();
 * --------------------------
 * Print out the instructions for the user.
 */

void giveInstructions()
{
  cout << endl;
  cout << "The boggle board is a grid onto which I ";
  cout << "I will randomly distribute cubes. These ";
  cout << "6-sided cubes have letters rather than ";
  cout << "numbers on the faces, creating a grid of ";
  cout << "letters on which you try to form words. ";
  cout << "You go first, entering all the words you can ";
  cout << "find that are formed by tracing adjoining ";
  cout << "letters. Two letters adjoin if they are next ";
  cout << "to each other horizontally, vertically, or ";
  cout << "diagonally. A letter can only be used once ";
  cout << "in each word. Words must be at least four ";
  cout << "letters long and can be counted only once. ";
  cout << "You score points based on word length: a ";
  cout << "4-letter word is worth 1 point, 5-letters ";
  cout << "earn 2 points, and so on. After your puny ";
  cout << "brain is exhausted, I, the supercomputer, ";
  cout << "will find all the remaining words and double ";
  cout << "or triple your paltry score." << endl << endl;
  cout << "Hit return when you're ready...";
  getLine();
}

// [TODO: Fill in the rest of the code]

// Copy the constant array into a vector vec so you can modify it.
// Shuffle vec using the following approach:
//	for (int i = 0; i < vec.size(); i++) {
//		Choose a random index r between i and the last element position, inclusive.
//		Swap the element at positions i and r.
//	}
// Fill the Boggle grid by choosing the elements of vec in order.
Grid<char> createBoard(bool big)
{
  const string *cubeList = STANDARD_CUBES;
  int numCubes = 16;
  int boardSize = 4;
  if (big)
    {
      cubeList = BIG_BOGGLE_CUBES;
      numCubes = 25;
      boardSize = 5;
    }
  Vector<string> cubes;
  // copy const array into vector
  for (int i = 0; i < numCubes; i++)
    cubes.add(cubeList[i]);

  // shuffle cubes vector
  string temp;
  int r;
  for (int i = 0; i < cubes.size(); i++)
    {
      r = randomInteger(i, cubes.size() - 1);
      temp = cubes[i];
      cubes[i] = cubes[r];
      cubes[r] = temp;
    }

  Grid<char> board(boardSize, boardSize);
  for (int row = 0; row < boardSize; row++)
    for (int col = 0; col < boardSize; col++)
      board[row][col] = cubes[row * boardSize + col][randomInteger(0, 5)];

  return board;
}

void initBoard(int numRows, int numCols)
{
  //initGraphics();
  drawBoard(numRows, numCols);
}

void showLetters(Grid<char> & board)
{
  for (int row = 0; row < board.numRows(); row++)
    for (int col = 0; col < board.numCols(); col++)
      labelCube(row, col, board[row][col]);
}
// The human player's turn
// The human player enters each word she finds on the board. For each word, check that:
//		* It is at least four letters long.
//		* It is contained in the English lexicon.
//		* It has not already been included in the player's word list (even if there is an alternate path on the
//			board to form the same word, the word is counted at most once).
//		* It can be formed on the board (i.e., it is composed of adjoining letters and each cube is used at most
//			once).
// If any of these conditions fail, the word is rejected. If all is good, you add the word to the player's
// word list and score. In addition, you graphically show the word's path by temporarily highlighting its
// cubes on the board. You can use the graphics function pause to implement the delay. The length of the
// word determines the score: one point for a 4letter word, two points for 5-letters, and so on. The
// functions from the gboggle.h interface provide helpful routines for highlighting cubes, displaying
// word lists, and handling scores.
// The player enters a blank line when done finding words, which signals the end of the human's turn.
void humanPlay(Lexicon & english, Lexicon & words, Grid<char> & board)
{
  while(true)
    {
      string word = getLine("Enter word (RETURN when done): ");
      if (word == "") break;
      if (word.length() < 4)
        {
          cout << "Word too short. Try again." << endl;
          continue;
        }
      if (!english.contains(word))
        {
          cout << "That's not a word. Try again." << endl;
          continue;
        }
      if (words.contains(word))
        {
          cout << "You already used that word. Try again." << endl;
          continue;
        }
      Vector<Point> usedPositions;
      if (!canMakeWord(toUpperCase(word), board, usedPositions))
        {
          cout << "That word isn't on the board. Try again." << endl;
          continue;
        }
      words.add(word);
      recordWordForPlayer(word, HUMAN);
      highlightCubes(usedPositions);
    }
}

bool canMakeWord(string word, Grid<char> & board, Vector<Point> & usedPositions)
{
  for (int row = 0; row < board.numRows(); row++)
    for (int col = 0; col < board.numCols(); col++)
      {
        usedPositions.clear();
        if (canMakeWordStartingAt(row, col, word, board, usedPositions))
          return true;
      }
  return false;
}

bool canMakeWordStartingAt(int row, int col, string word, Grid<char> & board, Vector<Point> & usedPositions)
{
  //cout << row << " " << col << " " << word << endl;
  if (board[row][col] != word[0] || alreadyUsed(Point(row, col), usedPositions)) return false;
  if (word.length() == 1)
    {
      if (board[row][col] == word[0])   // && !alreadyUsed(Point(row, col), usedPositions)) {
        {
          //cout << "ADD " << row << " " << col << endl;
          usedPositions.add(Point(row, col));
          return true;
        }
      else return false;
    }
  usedPositions.add(Point(row, col));
  //cout << "ADD " << row << " " << col << endl;
  Vector<Point> neighbors = getNeighbors(row, col, board);
  foreach (Point p in neighbors)
    {
      if (canMakeWordStartingAt(p.getX(), p.getY(), word.substr(1), board, usedPositions))
        return true;
    }
  //cout << "DEL " << row << " " << col << endl;
  usedPositions.remove(usedPositions.size() - 1);
  return false;
}

Vector<Point> getNeighbors(int row, int col, Grid<char> & board)
{
  int rows = board.numRows();
  int cols = board.numCols();
  Vector<Point> nbrs;
  if (col > 0)
    {
      if (row > 0) nbrs.add(Point(row - 1, col - 1));
      nbrs.add(Point(row, col - 1));
      if (row < rows - 1) nbrs.add(Point(row + 1, col - 1));
    }
  if (row > 0) nbrs.add(Point(row - 1, col));
  if (row < rows - 1) nbrs.add(Point(row + 1, col));
  if (col < cols - 1)
    {
      if (row > 0) nbrs.add(Point(row - 1, col + 1));
      nbrs.add(Point(row, col + 1));
      if (row < rows - 1) nbrs.add(Point(row + 1, col + 1));
    }
  return nbrs;
}

void highlightCubes(Vector<Point> cubes)
{
  foreach(Point p in cubes)
    highlightCube(p.getX(), p.getY(), true);
  pause(1000);
  foreach(Point p in cubes)
    highlightCube(p.getX(), p.getY(), false);
}

bool alreadyUsed(Point p, Vector<Point> used)
{
  //cout << "USED? " << p << " ";
  foreach(Point u in used)
    {
      if (p == u)
        {
          //cout << "YES" << endl;
          return true;
        }
    }
  //cout << "NO" << endl;
  return false;
}

// Computer Player's turn
// The computer then searches the entire board to find the remaining words missed by the human player.
// The computer earns points for each word found that meets the requirements (minimum length,
// contained in English lexicon, not already found, and can be formed on board).
// As with any exponential search algorithm, it is important to look for ways to limit the search to ensure
// that the process can be completed in a reasonable time. One of the most important Boggle strategies is
// to prune dead end searches. For example, if you have a path starting zx, the lexicon's containsPrefix
// member function will inform you that there are no English words down that path. Thus, you should
// stop right here and move on to more promising combinations. If you miss this optimization, you'll find
// yourself taking long coffee breaks while the computer is futilely looking for words like zxgub, zxaep,
// etc. Even though you may love coffee, this is obviously not the best idea.
void computerPlay(Lexicon & englishWords, Lexicon & humanWords, Grid<char> & board)
{
  Lexicon compWords;

  for (int row = 0; row < board.numRows(); row++)
    for (int col = 0; col < board.numCols(); col++)
      {
        Vector<Point> usedPositions;
        usedPositions.clear();
        findAllWordsStartingAt(row, col, board, usedPositions,
                               englishWords, humanWords, compWords);
      }
}

void findAllWordsStartingAt(int row, int col,
                            Grid<char> & board,
                            Vector<Point> & usedPositions,
                            Lexicon & allWords,
                            Lexicon & humanWords,
                            Lexicon & compWords,
                            string prefix)
{
  prefix += board[row][col];
  if (alreadyUsed(Point(row, col), usedPositions)) return;
  if (!allWords.containsPrefix(prefix)) return;

  usedPositions.add(Point(row, col));
  if (allWords.contains(prefix) && prefix.length() >= 4 &&
      !humanWords.contains(prefix) && !compWords.contains(prefix))
    {
      recordWordForPlayer(prefix, COMPUTER);
      compWords.add(prefix);
    }
  Vector<Point> neighbors = getNeighbors(row, col, board);
  foreach (Point p in neighbors)
    {
      findAllWordsStartingAt(p.getX(), p.getY(), board, usedPositions, allWords, humanWords, compWords, prefix);
    }
  usedPositions.remove(usedPositions.size() - 1);
}
