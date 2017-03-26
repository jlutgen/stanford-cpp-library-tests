/*
 * File: pathfinder.cpp
 * --------------------
 * Name: [TODO: enter name here]
 * This file is the entry point for the Pathfinder application
 * of Assignment #6.
 */

// [TODO: complete this implementation]

#include <iostream>
#include <string>
#include "console.h"
#include "graphtypes.h"
#include "gpathfinder.h"
//JL
#include <cstdlib>
#include "simpio.h"
#include "filelib.h"
#include "vector.h"
#include "tokenscanner.h"
#include "gtypes.h"
#include "strlib.h"
#include "pqueue.h"
#include "map.h"
#include "path.h"
//endJL

using namespace std;

/* Function prototypes */

void quitAction();
//JL
void showWelcomeMessage();
void mapAction(PathfinderGraph & graph);
void dijkstraAction(PathfinderGraph & graph);
void kruskalAction(PathfinderGraph & graph);
void readGraphFile(PathfinderGraph & graph);
void drawGraph(PathfinderGraph & graph);
Path findShortestPath(node *start, node *finish);
Vector<arc *> findMinSpanTree(PathfinderGraph & graph);
double getPathCost(const Vector<arc *> & path);
node *getNodeForLoc(GPoint p, PathfinderGraph & graph);
void drawPath(Path & path);
void drawTree(Vector<arc *> & t);
//endJL


/* Main program */

int main() {
    setConsoleSize(800, 300);
	GWindow gw(kWindowWidth, kWindowHeight + kControlStripHeight);
    initPathfinderGraphics(gw);
	showWelcomeMessage();

   //JL
   PathfinderGraph g;

   addButton("Load Map", mapAction, g);
   addButton("Dijkstra", dijkstraAction, g);
   addButton("Kruskal", kruskalAction, g);
   //endJL

   addButton("Quit", quitAction);

   //JL
   //mapAction(g);
   /*cout << "---------NODES---------" << endl;
   for (nodeT *n : g.getNodeSet()) {
	   cout << "node: " << n->name << ": " << n->loc.toString() << endl;
   }
   cout <<  "---------ARCS---------" << endl;
   for (arcT *a : g.getArcSet()) {
	   cout << "arc: " << a->start->name << " " << a->finish->name << " " << a->cost << endl;
   }*/
   //endJL

	// JL
   //mapAction(g);
	// endJL
   pathfinderEventLoop();
   return 0;
}

/* Sample callback function */

void quitAction() {
   exitGraphics();
}

//JL

void showWelcomeMessage() {
	cout << "Welcome to Pathfinder!" << endl;
}

void mapAction(PathfinderGraph & graph) {
	readGraphFile(graph);
	drawGraph(graph);
}

void readGraphFile(PathfinderGraph & graph) {
	ifstream ifs;
	promptUserForFile(ifs, "Please enter name of map data file: ");
	Vector<string> lines;
	readEntireFile(ifs, lines);
	TokenScanner scanner;
	string token;
	scanner.ignoreWhitespace();
	scanner.addWordCharacters(".");
	scanner.setInput(lines[0]);
	token = scanner.nextToken();
	graph.clear();
	graph.setImageFilename(token);
	//cout << "g.getImageFilename(): " << graph.getImageFilename() << endl;

	// read nodes
	int line = 2;
	while (true) {
		scanner.setInput(lines[line++]);
		token = scanner.nextToken();
		if (token == "ARCS") break;
                node *newNode = new node;
		//cout << "node name: " << token << endl;
		newNode->name = token;
		//cout << "node x: " << (token = scanner.nextToken()) << endl;
		double x = stringToReal(scanner.nextToken());
		//cout << "node y: " << (token = scanner.nextToken()) << endl;
		double y = stringToReal(scanner.nextToken());
		newNode->loc = GPoint(x, y);
		graph.addNode(newNode);
	}

	// read arcs
	while (line < lines.size()) {
		scanner.setInput(lines[line++]);
		if (!scanner.hasMoreTokens()) break; // guard against blank lines at end
		//cout << "arc start: " << (token = scanner.nextToken()) << endl;
		string city1 = scanner.nextToken();
		string city2 = scanner.nextToken();
		double cost = stringToReal(scanner.nextToken());
        arc *newArcA = new arc;
        arc *newArcB = new arc;
		newArcA->start = graph.getNode(city1);
		newArcA->finish = graph.getNode(city2);
		newArcA->cost = cost;
		graph.addArc(newArcA);
		newArcB->start = graph.getNode(city2);
		newArcB->finish = graph.getNode(city1);
		newArcB->cost = cost;
		graph.addArc(newArcB);
	}

	ifs.close();
}

void drawGraph(PathfinderGraph & graph) {
	drawPathfinderMap("images/" + graph.getImageFilename());
    for (node *n : graph.getNodeSet())
		drawPathfinderNode(n->loc, "dark_gray", n->name);
    for (arc *a : graph.getArcSet())
		drawPathfinderArc(a->start->loc, a->finish->loc, "dark_gray");
	//repaintPathfinderDisplay();
}

void dijkstraAction(PathfinderGraph & graph) {
	drawGraph(graph);
	//string start = getLine("Start: ");
	//string end = getLine("End: ");
    node *start, *end;
	while(true) {
		cout << "Click on starting city." << endl;
		GPoint loc = getMouseClick();
		start = getNodeForLoc(loc, graph);
		if (start != NULL) break;
		cout << "Invalid click." << endl;
	}
	//cout << endl;
	drawPathfinderNode(start->loc, "red", start->name);
	while(true) {
		cout << "Click on ending city." << endl;
		GPoint loc = getMouseClick();
		end = getNodeForLoc(loc, graph);
		if (end != NULL) break;
		cout << "Invalid click." << endl;
	}
	//cout << endl;
	drawPathfinderNode(end->loc, "red", end->name);


	cout << endl;
	Path path = findShortestPath(start, end);
	cout << path.toString() << endl;
    /*for (arcT *a : path) {
		cout << a->start->name << "->" << a->finish->name << ": " << a->cost << endl;
	}*/
	drawPath(path);
	cout << "Total cost: " << path.cost() << endl;
}

/*
* Function: findShortestPath
* Usage: Vector<arcT *> path = findShortestPath(start, finish);
* ------------------------------------------------------------
* Finds the shortest path between the nodes start and finish using
* Dijkstra's algorithm, which keeps track of the shortest paths in
* a priority queue. The function returns a vector of arcs, which is
* empty if start and finish are the same node or if no path exists.
*/
Path findShortestPath(node *start, node *finish) {
	Path path;
	PriorityQueue<Path> queue;
	Map<string, double> fixed;
	while (start != finish) {
		if (!fixed.containsKey(start->name)) {
			cout << "Fix the distance to " << start->name << " at " << path.cost() << endl;
			fixed.put(start->name, path.cost());
			cout << "Process arcs out of " << start->name << endl;
            for (arc *arc : start->arcs) {
				if (!fixed.containsKey(arc->finish->name)) {
					path.addArc(arc);
					cout << "  Enqueue path " << path.toString() << " (" << path.cost() << ")" << endl;
					queue.enqueue(path, path.cost());
					path.removeLastArc();
				}
				else
					cout << "  Ignore " << arc->finish->name << ": distance is known" << endl;
			}
		}
		if (queue.isEmpty()) {
			path.clear();
			return path;
		}
		cout << "Dequeue shortest path: ";
		path = queue.dequeue();
		cout << path.toString() << endl;
		start = path.getLastArc()->finish;
	}
	return path;
}

/*
* Function: getPathCost
* Usage: double cost = getPathCost(path);
* ---------------------------------------
* Returns the total cost of the path, which is just the sum of the
* costs of the arcs.
*/
double getPathCost(const Vector<arc *> & path) {
	double cost = 0;
    for (arc *arc : path) {
		cost += arc->cost;
	}
	return cost;
}

node *getNodeForLoc(GPoint p, PathfinderGraph & g) {
	const double fuzz = 5;
    for (node *n : g.getNodeSet()) {
		if (abs(int(n->loc.getX() - p.getX())) < fuzz && abs(int(n->loc.getY() - p.getY())) < fuzz)
			return n;
	}
	return NULL;
}

void drawPath(Path & path) {
	for (int i=0; i<path.size(); i++) {
        arc *a = path.getAt(i);
		drawPathfinderArc(a->start->loc, a->finish->loc, "red");
		drawPathfinderNode(a->start->loc, "red", a->start->name);
	}

}

void drawTree(Vector<arc *> & t) {
    for (arc *a : t) {
		drawPathfinderArc(a->start->loc, a->finish->loc, "red");
		drawPathfinderNode(a->start->loc, "black", a->start->name);
		drawPathfinderNode(a->finish->loc, "black", a->finish->name);
		drawPathfinderNode(a->start->loc, "red");
		drawPathfinderNode(a->finish->loc, "red");

	}
}

void kruskalAction(PathfinderGraph & graph) {
    Vector<arc *> tree = findMinSpanTree(graph);
	//cout << "Minimal spanning tree:" << endl;
	double cost = 0;
    for (arc *a : tree) {
		//cout << a->start->name << "->" << a->finish->name << ": " << a->cost << endl;
		cost += a->cost;
	}
	cout << "Total length: " << cost << endl;
	drawPathfinderMap("images/" + graph.getImageFilename());
	drawTree(tree);
}

Vector<arc *> findMinSpanTree(PathfinderGraph & graph) {
    Vector<arc *> tree;
	Map< string, Set<string> > connected;
    for (node *n : graph.getNodeSet()) {
		connected[n->name] = Set<string>();
		connected[n->name].add(n->name); // node is connected only to itself initially
	}
    PriorityQueue<arc *> arcs;
	Set<string> nodesUsed;
    for (node *n : graph.getNodeSet()) {
		nodesUsed.add(n->name);
        for (arc *a : graph.getArcSet(n))
			if (!nodesUsed.contains(a->finish->name))
				arcs.enqueue(a, a->cost);
	}
	while (!arcs.isEmpty()) {
        arc *a = arcs.dequeue();
		string start = a->start->name;
		string end = a->finish->name;
		double cost = a->cost;
		cout << cost << ": " << start << " -- " << end;
		/*cout << end << " is connected to: " << endl;
        for (string s : connected[end])
			cout << "\t" << s << endl;*/
		if (connected[end].contains(start)) {
			cout << " (not needed)" << endl;
			continue;
		}
		cout << endl;
		tree.add(a);
		/*cout << "merging connected sets" << endl;
		cout << start << " is connected to: " << endl;
        for (string s : connected[start])
			cout << "\t" << s << endl;
		cout << end << " is connected to: " << endl;
        for (string s : connected[end])
			cout << "\t" << s << endl;*/
		connected[start] += connected[end];
        for(string node : connected[start]) {
			if (node != start)
				connected[node] += connected[start];
		}
		/*cout << "after merging" << endl;
		cout << start << " is connected to: " << endl;
        for (string s : connected[start])
			cout << "\t" << s << endl;
		cout << end << " is connected to: " << endl;
        for (string s : connected[end])
			cout << "\t" << s << endl;*/
		if (connected[start].size() == graph.size()) // tree now spans
			break;
	}
	return tree;
}
//endJL
