#ifndef CTRL_DEP_WRITER_CHOOPER
#define CTRL_DEP_WRITER_CHOOPER 

#include "CtrlDep.h"

namespace razorleaf {
    class CtrlDepWriter {
    public:
        CtrlDepWriter ();
        virtual ~CtrlDepWriter ();

        static void write(CtrlDep*);

    
    };
} /* razorleaf */

#endif

