/*
 * File: graphtypes.h
 * ------------------
 * This file defines low-level data structures that represent graphs.
 */

#ifndef _graphtypes_h
#define _graphtypes_h

#include <string>
#include "gtypes.h"
#include "map.h"
#include "set.h"
// JL
#include "graph.h"
//

using namespace std;

struct node;     /* Forward references to these two types so  */
struct arc;      /* that the C++ compiler can recognize them. */

//JL - delete this section
///*
// * Type: SimpleGraph
// * -----------------
// * This type represents a graph and consists of a set of nodes, a set of
// * arcs, and a map that creates an association between names and nodes.
// */
//
//struct SimpleGraph {
//   Set<Node *> nodes;
//   Set<Arc *> arcs;
//   Map<string, Node *> nodeMap;
//};
// endJL

/*
 * Type: node
 * ----------
 * This type represents an individual node and consists of the
 * name of the node and the set of arcs from this node.
 */

struct node {
   string name;
   Set<arc *> arcs;
   //JL
   GPoint loc;
   //endJL
};

/*
 * Type: arc
 * ---------
 * This type represents an individual arc and consists of pointers
 * to the endpoints.
 */

struct arc {
   node *start;
   node *finish;
   //JL
   double cost;
   //endJL
};

//JL
class PathfinderGraph : public Graph<node, arc>
{
public:
	void setImageFilename(string s) { imageFilename = s; }
	string getImageFilename() { return imageFilename; }
private:
	string imageFilename;
};
//endJL

#endif
