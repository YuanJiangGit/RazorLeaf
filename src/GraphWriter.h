#ifndef GRAPH_WRITER_CHOPPER
#define GRAPH_WRITER_CHOPPER 

#include <string>

using namespace std;

namespace chopper {
    class CDG;
    class PDG;
    class GraphWriter {
    public:
        static void writeCDG(CDG*, string&);
        static void writePDG(PDG*, string&);
    };
    
} /* chopper */

#endif

