/*
 * File: mazeSolution.cpp
 * --------------
 * Name: [TODO: fill in your name here]
 *
 * Write all your code for Assignment 3 (Maze) in this file.
 *
 */

#include <iostream>
#include "console.h"
#include "maze.h"
#include "gwindow.h"
#include "grid.h"
#include "queue.h"
#include "random.h"
#include "simpio.h"
#include "stack.h"
#include "vector.h"

using namespace std;

bool allIncluded(Grid<bool> & g);
void makePerfect(Maze & m);
Vector<pointT> neighbors(Maze & m, pointT p);
pointT randomNeighbor(Maze & m, pointT p);
Vector<pointT> solveMaze(Maze & m);
void drawPath(Maze & m, Stack<pointT> path, string color);
void erasePath(Maze & m, Stack<pointT> path);

const bool DRAW_PARTIAL_SOLUTIONS = false;
const bool PAUSE = false;
const int PAUSE_MS = 1;

int main()
{
    setConsoleExitProgramOnClose(true);
    setConsolePrintExceptions(true);
    setConsoleSize(600, 200);
    //////////////////////////////////////////////////

    while (true)
    {
        Maze mz(14, 21, true); // feel free to try different dimensions
        mz.initMazeGraphics(600, 450);
        mz.draw();
        string response = getLine("Press ENTER to generate a new maze");
        //cout << "got" << quit << "length " << quit.length() << endl;
        //if (quit == "Q" || quit == "q") break;
        cout << "Generating maze...";
        makePerfect(mz);
        cout << "done." << endl;
        response = getLine("Press ENTER and I will solve the maze");
        //cout << "got" << quit << "length " << quit.length() << endl;
        //if (quit == "Q" || quit == "q") break;
        cout << "Solving maze...";
        Vector<pointT> solution = solveMaze(mz);
        cout << "done." << endl;
        cout << "Drawing solution path...";
        for(pointT p : solution)
        {
            mz.drawMark(p, "Blue");
            pause(20);
        }
        cout << "done." << endl;
        response = getLine("\nWould you like to play again? (y/n) ");
        if (response != "y" && response != "Y") break;
    }

    /////////////////////////////////////////////////
    getLine("\nPress [Enter] to close the windows... ");
    exitGraphics();
    return 0;
}

// Generating a perfect maze:
// All cells start "excluded" from the maze. Choose a cell at random and
// mark it as "included" in the maze. This cell is the current point.
// 1. Randomly choose one of the neighbors of the current point
// 2. If neighbor is "excluded", remove wall between current point and neighbor and mark
//    neighbor as "included"
// 3. Neighbor becomes current point (whether or not wall was removed)
// 4. Repeat steps 1-3 until all cells in the maze have been included

void makePerfect(Maze & m)
{
    pointT p;
    Grid<bool> included(m.numRows(), m.numCols());
    for (int i=0; i<m.numRows(); i++)
        for (int j=0; j<m.numCols(); j++)
            included[i][j]= false;

    p.row = randomInteger(0, m.numRows() - 1);
    p.col = randomInteger(0, m.numCols() - 1);
    included[p.row][p.col] = true;

    while (!allIncluded(included))
    {
        pointT n = randomNeighbor(m, p);
        if (!included[n.row][n.col])
        {
            m.setWall(p, n, false);
            included[n.row][n.col] = true;
        }
        p = n;
        //pause(2);
    }
}

bool allIncluded(Grid<bool> & g)
{
    for(bool b : g)
        if (!b) return false;
    return true;
}

Vector<pointT> neighbors(Maze & m, pointT p)
{
    Vector<pointT> nbrs;
    pointT n;
    n.col = p.col;
    n.row = p.row + 1;
    if (m.pointInBounds(n))
        nbrs.add(n);
    n.col = p.col;
    n.row = p.row - 1;
    if (m.pointInBounds(n))
        nbrs.add(n);
    n.col = p.col + 1;
    n.row = p.row;
    if (m.pointInBounds(n))
        nbrs.add(n);
    n.col = p.col - 1;
    n.row = p.row;
    if (m.pointInBounds(n))
        nbrs.add(n);
    return nbrs;
}

pointT randomNeighbor(Maze & m, pointT p)
{
    Vector<pointT> nbrs = neighbors(m, p);
    int i = randomInteger(0, nbrs.size() - 1);
    return nbrs[i];
}

// Breadth-first search. It works on a queue of paths (stack objects).
// 1. Create path with just start location and enqueue it
// 2. Dequeue shortest path
// 3. If path ends at goal you're done, otherwise
//   a. For each neighbor accessible from path end, make copy of path, extend by adding
//      neighbor and enqueue it
// 4. Repeat steps 2-3 until at goal (also stop if queue is empty, won't happen for perfect maze)
Vector<pointT> solveMaze(Maze & m)
{
    pointT start = {0, 0};
    pointT end = {m.numRows() - 1, m.numCols() - 1};
    Stack<pointT> path;
    Queue< Stack<pointT> > q;
    path.push(start);
    q.enqueue(path);
    while (1)
    {
        path = q.dequeue();
        if (DRAW_PARTIAL_SOLUTIONS)
            drawPath(m, path, "Red");
        pointT last = path.pop();
        if (last.row == end.row && last.col == end.col)
        {
            path.push(last);
            break;
        }
        Vector<pointT> nbrs = neighbors(m, last);
        if (path.isEmpty())
        {
            path.push(last);
            if (DRAW_PARTIAL_SOLUTIONS)
                erasePath(m, path);
            for (pointT n : nbrs)
            {
                if (!m.isWall(last, n))
                {
                    Stack<pointT> extension = path;
                    extension.push(n);
                    q.enqueue(extension);
                }
            }
        }
        else
        {
            pointT prev = path.peek();
            path.push(last);
            if (DRAW_PARTIAL_SOLUTIONS)
                erasePath(m, path);
            for (pointT n : nbrs)
            {
                if ((n.row != prev.row || n.col != prev.col) && !m.isWall(last, n))   // avoid doubling back
                {
                    Stack<pointT> extension = path;
                    extension.push(n);
                    q.enqueue(extension);
                }
            }
        }
    }
    // convert stack for path into vector and return the vector
    Vector<pointT> solution;
    while (!path.isEmpty())
        solution.insert(0, path.pop());
    return solution;
}

void drawPath(Maze & m, Stack<pointT> path, string color)
{
    Vector<pointT> vpath;
    while (!path.isEmpty())
        vpath.insert(0, path.pop());
    for (pointT p : vpath)
    {
        m.drawMark(p, color);
        if (PAUSE)
            pause(PAUSE_MS);
    }
}

void erasePath(Maze & m, Stack<pointT> path)
{
    Vector<pointT> vpath;
    while (!path.isEmpty())
        vpath.insert(0, path.pop());
    for (pointT p : vpath)
        m.eraseMark(p);
}
