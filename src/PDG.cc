#include "PDG.h"

using namespace chopper;

PDG::PDG()
{
}

void
PDG::addEdge(Instruction *source,
        Instruction *target,
        unsigned int type)
{
    // search source vertex if exists
    unsigned int sid = 0, tid = 0;
    for (; sid < vertexSet.size(); sid++) {
        if (vertexSet[sid].inst == source) {
            break;
        }
    }

    if (sid == vertexSet.size()) {
        //not found
        Vertex v = {
            false,
            source,
            AdjList()
        };
        vertexSet.push_back(v);
    }

    // search target vertex if exists
    for (; tid < vertexSet.size(); tid++) {
        if (vertexSet[tid].inst == target) {
            break;
        }
    }

    if (tid == vertexSet.size()) {
        //not found
        Vertex v = {
            false,
            target,
            AdjList()
        };
        vertexSet.push_back(v);
    }

    AdjVertex adjV = {
        tid, type
    };

    vertexSet[sid].adjList.insert(adjV);
}

PDG::~PDG()
{
}

