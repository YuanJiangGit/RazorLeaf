#include "Serializer.h"

#include "PDG.h"
#include "CtrlDep.h"
#include "json/json.h"

#include <cstdio>
#include <llvm/Support/raw_ostream.h>

using namespace chopper;

SerializerException::SerializerException() noexcept
{}

SerializerException::SerializerException(const string &msg) noexcept:
    exception(), message(msg) {}
SerializerException::SerializerException(const char *msg) noexcept:
    exception(), message(msg) {}

const char*
SerializerException::what() const noexcept
{
    return message.c_str();
}

Serializer::Serializer() 
{
}

Serializer::~Serializer()
{
}

static int
serializer_printer_cb(void *userdata, const char *s, uint32_t length)
{
    fwrite(s, length, 1, (FILE*)userdata);
    return 0;
}

void
Serializer::serialize(SerialInfo &sInfo)
{
    json_printer print;
    FILE *fp = fopen(sInfo.filename.c_str(), "w+");
    if (!fp) {
        throw new SerializerException("file open");
    }
    if (json_print_init(&print, serializer_printer_cb, 
                fp)) {
        throw new SerializerException();
    }

    json_print_args(&print, json_print_pretty,
            JSON_OBJECT_BEGIN, JSON_KEY, "pdg",
            JSON_ARRAY_BEGIN, -1);

    PDG *pdg = sInfo.pdg;
    string buffer;
    llvm::raw_string_ostream ss(buffer);
    for (PDG::iterator iter=pdg->begin(); iter!=pdg->end();
            iter++) {
        ss << (*iter->inst) ;
        json_print_args(&print, json_print_pretty,
            JSON_OBJECT_BEGIN, 
                JSON_KEY, "inst", 
                JSON_STRING, ss.str().c_str(),
                -1);
    }
    json_print_args(&print, json_print_pretty,
            JSON_ARRAY_END, -1);

    json_print_args(&print, json_print_pretty,
            JSON_OBJECT_END, -1);

    fclose(fp);
    json_print_free(&print);

}

