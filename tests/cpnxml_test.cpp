#include "cpnxml.hpp"

int main()
{
    PSM_NM::CPNXml x = PSM_NM::CPNXml("out");
    x.AddPage("myNet");
    x.Dump();
    return 0;
}