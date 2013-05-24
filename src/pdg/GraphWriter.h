#ifndef GRAPH_WRITER_CHOPPER
#define GRAPH_WRITER_CHOPPER 

#include <string>

using namespace std;

namespace razorleaf {
    class CDG;
    class PDG;
    class GraphWriter {
    public:
        static void writeCDG(CDG*, string&);
        static void writePDG(PDG*, string&);
    };
    
} /* razorleaf */

#endif

