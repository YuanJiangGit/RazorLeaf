#ifndef CTRL_DEP_WRITER_CHOOPER
#define CTRL_DEP_WRITER_CHOOPER 

#include "CtrlDep.h"

namespace chopper {
    class CtrlDepWriter {
    public:
        CtrlDepWriter ();
        virtual ~CtrlDepWriter ();

        static void write(CtrlDep*);

    
    };
} /* chopper */

#endif

