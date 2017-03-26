/*
 * File: path.cpp
 * --------------
 * Name: [TODO: enter name here]
 * This file implements the path.h interface.
 */

/////////////////////////////////////////////////////////////////////
// Your task in this part of the assignment is to write the code   //
// that implements your path.h interface.  For the benefit of      //
// future programmers who will need to work on this interface,     //
// you must also write an extended comment (presumably replacing   //
// this to-do box) that documents the design decisions you made    //
// in structuring the Path class as you did.                       //
/////////////////////////////////////////////////////////////////////

// [TODO: implement the methods in your Path class]

//JL
#include "path.h"

Path::Path() {
	totalCost = 0;
}

string Path::toString() {
	string s = "";
	for (int i=0; i<arcs.size(); i++) {
		s += arcs[i]->start->name + "->";
		if (i == arcs.size() - 1)
			s += arcs[i]->finish->name;
	}
	return s;
}

void Path::addArc(arc *a) {
    arcs.add(a);
    totalCost += a->cost;
}

arc *Path::getLastArc() {
	return arcs[arcs.size() - 1];
}

void Path::removeLastArc() {
    arc *lastArc = getLastArc();
	arcs.remove(arcs.size() - 1);
	totalCost -= lastArc->cost;
}

void Path::clear() {
	arcs.clear();
}

double Path::cost() {
	return totalCost;
}

arc *Path::getAt(int i) {
	return arcs[i];
}

int Path::size() {
	return arcs.size();
}
//endJL
