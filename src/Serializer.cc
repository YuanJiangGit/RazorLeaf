#include "Serializer.h"

#include "PDG.h"
#include "CtrlDep.h"
#include "json/json.h"

#include <cstdio>
#include <cstring>
#include <utility>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/Instructions.h>

using namespace chopper;
using std::pair;
using std::make_pair;

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

chopper::Serializer::Serializer() 
{
}

chopper::Serializer::~Serializer()
{
}

static int
serializer_printer_cb(void *userdata, const char *s, uint32_t length)
{

    fwrite(s, sizeof(char), length, (FILE*)userdata);
    return 0;
}

void
chopper::Serializer::serialize(SerialInfo &sInfo)
{
    json_printer print;
    FILE *fp = sInfo.fp;
    if (!fp) {
        throw new SerializerException("file open");
    }
    if (json_print_init(&print, serializer_printer_cb, 
                fp)) {
        throw new SerializerException();
    }

    json_print_args(&print, json_print_pretty,
            JSON_OBJECT_BEGIN, 
                JSON_KEY, "name", 4,
                JSON_STRING, sInfo.func->getName().data(),
                    sInfo.func->getName().size(),
                JSON_KEY, "bb", 2,
                JSON_ARRAY_BEGIN, -1);

    /* index all instruction and basicblock info */
    // first realId, second id in graph
    DenseMap<Instruction*, pair<size_t, size_t> > instMap;
    DenseMap<BasicBlock*, pair<size_t, size_t> >
        bbMap;
    size_t bbId = 0;
    size_t instId = 0;
    PDG *pdg = sInfo.pdg;
    CDG *cdg = sInfo.cdg;

    for (BasicBlock &bb : sInfo.func->getBasicBlockList()) {
        string startId = std::to_string(instId);
        string bbIdStr = std::to_string(bbId);
        
        json_print_args(&print, json_print_pretty,
                JSON_OBJECT_BEGIN,
                    JSON_KEY, "name", 4,
                    JSON_STRING, bb.getName().data(), 
                        bb.getName().size(),
                    JSON_KEY, "startId", 7,
                    JSON_INT, startId.c_str(), startId.length(),
                    JSON_KEY, "id", 2,
                    JSON_INT, bbIdStr.c_str(), bbIdStr.length(),
                    JSON_KEY, "inst", 4,
                    JSON_ARRAY_BEGIN, -1);

        for (Instruction &inst : bb) {
            string buffer;
            string instIdStr = std::to_string(instId);
            instMap[&inst] = make_pair(instId, 0);
            instId ++;
            llvm::raw_string_ostream ss(buffer);
            ss << inst ;
            json_print_args(&print, json_print_raw,
                    JSON_OBJECT_BEGIN,
                    JSON_KEY, "content", 7,
                    JSON_STRING, buffer.c_str(), buffer.length(),
                    JSON_KEY, "id", 2,
                    JSON_INT, instIdStr.c_str(), instIdStr.size(), -1);

            if (dyn_cast<BranchInst>(&inst)) {
                json_print_args(&print, json_print_raw,
                        JSON_KEY, "term", 4,
                        JSON_STRING, "b", 1, -1);
            } else if (dyn_cast<SwitchInst>(&inst)) {
                json_print_args(&print, json_print_raw,
                        JSON_KEY, "term", 4,
                        JSON_STRING, "s", 1, -1);
            }

            json_print_args(&print, json_print_raw,
                    JSON_OBJECT_END, -1);
        }
        bbMap[&bb] = make_pair(bbId, 0);
        bbId ++;
        json_print_args(&print, json_print_raw,
                JSON_ARRAY_END,
                JSON_OBJECT_END, -1);
    }
    bbId = 0;
    for (CDG::iterator iter=cdg->begin(); iter!=cdg->end();
            iter++, bbId++) {
        if (!iter->bb) continue;
        bbMap[iter->bb].second = bbId;
    }

    instId = 0;
    json_print_args(&print, json_print_pretty, JSON_ARRAY_END,
            JSON_KEY, "pdg", 3,
            JSON_ARRAY_BEGIN, -1);
    // prcocess pdg 
    for (PDG::iterator iter=pdg->begin(); iter!=pdg->end();
            iter++, instId++) {
        //string buffer;
        if (!iter->inst) continue;
        instMap[iter->inst].second = instId;
        //llvm::raw_string_ostream ss(buffer);
        //ss << *(iter->inst) ;

        // parent block id 
        BasicBlock *parent = iter->inst->getParent();

        assert (parent && bbMap.count(parent));
        string pid = std::to_string(bbMap[parent].first);
        string realInstId = std::to_string(instMap[iter->inst].first);

        json_print_args(&print, json_print_raw,
            JSON_OBJECT_BEGIN, 
                //JSON_KEY, "inst", 4,
                //JSON_STRING, buffer.c_str(), buffer.length(),
                JSON_KEY, "bbId", 4,
                JSON_INT, pid.c_str(), pid.length(),
                JSON_KEY, "realId", 6,
                JSON_INT, realInstId.c_str(), realInstId.length(),
                JSON_KEY, "deps", 4,
                JSON_ARRAY_BEGIN, -1);
        PDG::AdjList adjList = iter->adjList;
        for (PDG::AdjList::iterator adjIter= adjList.begin();
                adjIter != adjList.end(); adjIter++) {

            string id = std::to_string(adjIter->id);
            json_print_args(&print, json_print_raw,
                JSON_OBJECT_BEGIN, 
                    JSON_KEY, "id", 2,
                    JSON_INT, id.c_str(), id.length(),
                    //TODO type
                JSON_OBJECT_END, -1);

        }
        json_print_args(&print, json_print_raw,
                JSON_ARRAY_END, JSON_OBJECT_END, -1);
    }

    json_print_args(&print, json_print_pretty,
            JSON_ARRAY_END, JSON_KEY, "cdg", 3,
            JSON_ARRAY_BEGIN, -1);

    //process cdg
    for (CDG::iterator iter=cdg->begin(); iter!=cdg->end();
            iter++) {
        if (!iter->bb) continue;
        bbMap[iter->bb].second = bbId;
        string id = std::to_string(bbMap[iter->bb].first);
        string bbName = iter->bb->getName().str();
        json_print_args(&print, json_print_raw,
            JSON_OBJECT_BEGIN, 
                JSON_KEY, "realId", 6,
                JSON_INT, id.c_str(), id.size(),
                JSON_KEY, "name", 4,
                JSON_STRING, bbName.c_str(), bbName.length(), -1);
        //TODO add instruction ID if needed
        Instruction *termInst = iter->bb->getTerminator();
        if (termInst && instMap.count(termInst)) {
            string termId = std::to_string(instMap[termInst].first);
            json_print_args(&print, json_print_raw,
                JSON_KEY, "termInst", 8,
                JSON_INT, termId.c_str(), termId.length(), -1);
        }

        //deps
        CDG::AdjList deps = iter->adjList;
        json_print_args(&print, json_print_raw, 
                JSON_KEY, "deps", 4,
                JSON_ARRAY_BEGIN, -1);
        for (unsigned int &bbId : deps) {
            string id = std::to_string(bbId);
            json_print_args(&print, json_print_raw,
                JSON_OBJECT_BEGIN, 
                    JSON_KEY, "id", 2,
                    JSON_INT, id.c_str(), id.length(),
                        //TODO type
                JSON_OBJECT_END, -1);

        }
        
        json_print_args(&print, json_print_raw,
                JSON_ARRAY_END,
             JSON_OBJECT_END, -1);
    }

    json_print_args(&print, json_print_raw,
            JSON_ARRAY_END, JSON_OBJECT_END, -1);

    json_print_free(&print);

}

