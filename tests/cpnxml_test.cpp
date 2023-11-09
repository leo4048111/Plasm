#include "cpnxml.hpp"

int main()
{
    PSM_NM::CPNXml x = PSM_NM::CPNXml("out");
    // AddPage
    int pageId1 = x.AddPage("TestNet");

    // Color set declarations
    x.DeclareUnitColorSet("UNIT");
    x.DeclareBooleanColorSet("BOOL");

    x.DeclareAliasColorSets("STRING", "string");
    x.DeclareAliasColorSets("TYPE", "STRING");
    x.DeclareAliasColorSets("TYPE2", "TYPE");
    x.Dump();
    return 0;
}