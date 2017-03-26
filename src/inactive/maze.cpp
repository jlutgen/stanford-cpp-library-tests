/*
 * File: maze.cpp
 * --------------
 * A maze is mostly a grid of walls, plus a little drawing code.
 *
 * Last modified 2014-02-16 by Jeff Lutgen
 *
 * You should not need to modify this file, but it must compiled, so
 * do not exclude it from your project.
 */

#include <cstdlib>
#include "maze.h"
#include "gwindow.h"
#include "error.h"

Maze::Maze(int numRows, int numCols, bool hasWalls) : cells(numRows, numCols)
{
    for (int r = 0; r < numRows; r++)
        for (int c = 0; c < numCols; c++)
            for (int d = 0; d < NumDirs; d++)
                cells[r][c].walls[d] = hasWalls;
    configured = false;
    gwp = NULL;
}

Maze::~Maze()
{
    if (gwp != NULL)
    {
        delete gwp;
    }
}

int Maze::numRows()
{
    return cells.numRows();
}

int Maze::numCols()
{
    return cells.numCols();
}

bool Maze::pointInBounds(pointT p)
{
    return (p.row >= 0 && p.row < numRows() && p.col >=0 && p.col < numCols());
}

void Maze::draw()
{
    gwp->clear();
    if (!configured) configureGraphics();
    for (int r = 0; r < cells.numRows(); r++)
    {
        for (int c = 0; c < cells.numCols(); c++)
        {
            pointT p = {r, c};
            drawWallsForCell(p);
        }
    }
}

bool Maze::isWall(pointT p1, pointT p2)
{
    if (!pointInBounds(p1) || !pointInBounds(p2))
        error("Point is not in bounds for maze");
    return cells[p1.row][p1.col].walls[neighborDir(p1, p2)];
}

void Maze::setWall(pointT p1, pointT p2, bool state)
{
    if (!pointInBounds(p1) || !pointInBounds(p2))
        error("Point is not in bounds for maze");
    cells[p1.row][p1.col].walls[neighborDir(p1, p2)] = state;
    cells[p2.row][p2.col].walls[neighborDir(p2, p1)] = state;
    if (!configured) configureGraphics();
    drawWallsForCell(p1);
}

void Maze::drawMark(pointT p, string color)
{
    if (!pointInBounds(p))
        error("Point is not in bounds for maze");
    if (!configured) configureGraphics();
    double margin = cellSize*.3;
    double length = cellSize - 2*margin;
    gwp->setColor(color);
    movePen(originX + p.col*cellSize + margin, originY - (p.row*cellSize + margin));
    drawLine2(length, -length);
    movePen(originX + p.col*cellSize + margin, originY - (p.row*cellSize + cellSize - margin));
    drawLine2(length, length);
}

void Maze::eraseMark(pointT p)
{
    if (!pointInBounds(p))
        error("Point is not in bounds for maze");
    if (!configured) configureGraphics();
    double margin = cellSize*.15;
    gwp->setColor("white");
    gwp->fillRect(originX + p.col*cellSize + margin, originY - (p.row + 1)*cellSize + margin,
                  cellSize - 2*margin, cellSize - 2*margin);
}

Maze::dirT Maze::neighborDir(pointT p1, pointT p2)
{
    if ((abs(p1.row-p2.row) + abs(p1.col-p2.col)) != 1)
        error("Points are not neighbors");
    if (p1.row != p2.row)
        return (p1.row < p2.row ? North : South);
    else
        return (p1.col < p2.col ? East : West);
}

void Maze::drawWallsForCell(pointT p)
{
    movePen(originX + p.col*cellSize, originY - p.row*cellSize);
    gwp->setColor(cells[p.row][p.col].walls[South] ? "Black" : "White");
    drawLine2(cellSize, 0);
    gwp->setColor(cells[p.row][p.col].walls[East] ? "Black" : "White");
    drawLine2(0, -cellSize);
    gwp->setColor(cells[p.row][p.col].walls[North] ? "Black" : "White");
    drawLine2(-cellSize, 0);
    gwp->setColor(cells[p.row][p.col].walls[West] ? "Black" : "White");
    drawLine2(0, cellSize);
}

void Maze::configureGraphics()
{
    int padding = 2;
    double width = gwp->getWidth() - padding;
    double height = gwp->getHeight() - padding;
    cellSize = min(width/numCols(), height/numRows());
    originX = (width - numCols()*cellSize)/2;
    originY = numRows()*cellSize + (height - numRows()*cellSize)/2;
    configured = true;
}

void Maze::initMazeGraphics()
{
    if (gwp != NULL)
    {
        gwp->close();
        delete gwp;
    }
    gwp = new GWindow();
}

void Maze::initMazeGraphics(int width, int height)
{
    if (gwp != NULL)
    {
        gwp->close();
        delete gwp;
    }
    gwp = new GWindow(width, height);
}

void Maze::movePen(double x, double y)
{
    curX = x;
    curY = y;
}

void Maze::drawLine2(double x, double y)
{
    gwp->drawLine(curX, curY, curX + x, curY + y);
    curX += x;
    curY += y;
}
