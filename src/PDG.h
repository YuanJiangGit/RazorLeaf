#ifndef PDG_H_CHOPER
#define PDG_H_CHOPER 

#include <vector>
#include <list>

#include <llvm/Instruction.h>

using std::vector;
using std::list;

using llvm::Instruction;

namespace chopper {

class PDG {
public:
    enum pdg_vertex_type {
        PDG_LOOP_CARRIED,
        PDG_LOOP_INDEPENDENT
    };

    typedef struct {
        unsigned int id, type;
    } AdjVertex;

    typedef list<AdjVertex> AdjList;

    typedef struct {
        bool isMarked;
        Instruction *inst;
        AdjList adjList;
    } Vertex;

    typedef vector<Vertex> VertexSet;
    typedef VertexSet::iterator iterator;

    PDG ();

    void addEdge(Instruction*, Instruction*, unsigned int);

    inline iterator begin() 
    { return vertexSet.begin(); }

    inline iterator end() 
    { return vertexSet.end(); }

    virtual ~PDG ();
private:
    VertexSet vertexSet;
};
    
} /* chopper */

#endif

