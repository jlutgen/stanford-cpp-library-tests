/*
 * File: path.h
 * ------------
 * Name: [TODO: enter name here]
 * This file is the interface for a Path class that encapsulates
 * the idea of a multistep path.
 */

/////////////////////////////////////////////////////////////////////
// Your task in this part of the assignment is to design a Path    //
// class that supports the implementation of Dijkstra's algorithm. //
// As noted in the assignment handout, your class must meet the    //
// following conditions:                                           //
//                                                                 //
// 1. The class must not contain any public instance variables.    //
//    All data required to store the path must be private to the   //
//    class.                                                       //
//                                                                 //
// 2. The method that returns the total cost of the path must      //
//    run in constant time.                                        //
//                                                                 //
// 3. The class must have a public toString method that returns a  //
//    string composed of the nodes on the path separated by        //
//    arrows formed by the two-character sequence ->.              //
//                                                                 //
// 4. Any heap storage allocated by this class must be freed       //
//    when the object is deleted.                                  //
//                                                                 //
// Beyond those restrictions, you are free to design and implement //
// this class as you see fit.  You must, however, document your    //
// design decisions in the corresponding path.cpp file.            //
/////////////////////////////////////////////////////////////////////

#ifndef _path_h
#define _path_h

//JL
#include "vector.h"
#include "graphtypes.h"
//endJL

class Path {

// [TODO: complete the class definition]
//JL
public:
	Path();
	string toString();
    void addArc(arc *arc);
    arc *getLastArc();
	void removeLastArc();
	void clear();
	double cost();
    arc *getAt(int i);
	int size();
private:
    Vector<arc *> arcs;
	double totalCost;
//endJL
};

#endif
