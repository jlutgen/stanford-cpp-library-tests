/*
 * @file streamInsertionTests.cpp
 *
 * @author Jeff Lutgen
 *
 */


#include <iostream>
#include <string>
#include <fstream>
#include "testcases-jl.h"
#include "simpio.h"
#include "lexicon.h"
#include "vector.h"
#include "stack.h"
#include "queue.h"
#include "pqueue.h"
#include "map.h"
#include "hashmap.h"
#include "set.h"
#include "hashset.h"
#include "grid.h"
#include "graph.h"

using namespace std;

struct DumbNode;
struct DumbEdge;
struct DumbNode {
    std::string name;
    Set<DumbEdge*> arcs;
    double cost;
};

struct DumbEdge {
    DumbNode* start;
    DumbNode* finish;
    double cost;
};

void streamInsertionLexiconTest() {
    Lexicon lex;
    cout << "empty lexicon: " << lex << endl;
    lex.add("alpher");
    cout << "1-item lexicon: " << lex << endl;
    lex.add("beter");
    lex.add("gammer");
    cout << "3-item lexicon: " << lex << endl;
    cout << "looping over lexicon..." << endl;
    for (string s : lex) {
        cout << s << endl;
    }
}

void streamInsertionOthersTest() {
    Vector<int> v;
    v.add(14);
    v.add(42);
    cout << "Vector: " << v << endl;

    Stack<int> s;
    s.add(14);
    s.add(42);
    cout << "Stack: " << s << endl;

    Queue<int> q;
    q.add(14);
    q.add(42);
    cout << "Queue: " << q << endl;

    PriorityQueue<int> pq;
    pq.add(14, 1);
    pq.add(42, 2);
    cout << "PriorityQueue: " << pq << endl;

    Grid<int> grid;
    grid.resize(2, 2);
    grid.fill(14);
    cout << "Grid: " << grid << endl;

    Map<string, Vector<int>> map;
    map.add("corfu", v);
    cout << "Map<string, Vector>: " << map << endl;

    HashMap<Stack<int>, Vector<int>> hashmap;
    hashmap.add(s, v);
    cout << "Map<Stack, Vector>: " << hashmap << endl;

    Set<int> set;
    set.add(14);
    set.add(42);
    cout << "Set: " << set << endl;

    HashSet<Set<int>> hashset;
    hashset.add(set);
    cout << "HashSet<Set>: " << hashset << endl;

    Graph<DumbNode, DumbEdge> graph;
    graph.addNode("a");
    graph.addNode("b");
    graph.addNode("c");
    graph.addNode("d");
    graph.addNode("e");
    graph.addArc("a", "b");
    graph.addArc("a", "d");
    graph.addArc("b", "c");
    graph.addArc("b", "d");
    graph.addArc("c", "b");
    graph.addArc("c", "e");
    cout << "Graph: " << graph << endl;
}
