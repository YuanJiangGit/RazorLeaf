#ifndef SERIALIZER_H_CHOPPER
#define SERIALIZER_H_CHOPPER value

#include <string>
#include <exception>

using std::string;
using std::exception;

namespace chopper {
    class PDG;
    class CDG;
    class SerializerException : public exception {
    public:
        SerializerException(const char *) noexcept;
        SerializerException(const string &) noexcept;
        SerializerException() noexcept;
        const char * what() const noexcept;
    private:
        string message;
    };
    class Serializer {
    public:
        typedef struct {
            string filename;
            string funcname;
            PDG *pdg;
            CDG *cdg;
        } SerialInfo;
        Serializer ();
        static void serialize(SerialInfo&);
        virtual ~Serializer ();
    
    };
    
} /* chopper */

#endif
