#include "utils/print.h"
#include "utils/skiplist.h"

#include <string>

int main()
{
    using namespace utils;

    SkipList<std::string, double> ranklist;

    ranklist.insert({"jeo",3.2});
    ranklist.insert({"leo",1});
    ranklist.print();


    return 0;
}
