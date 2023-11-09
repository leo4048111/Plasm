// cpnxml_test.cpp

#include "cpnxml.hpp"

int main()
{
    PSM_NM::CPNXml x = PSM_NM::CPNXml("out");
    // AddPage
    int pageId1 = x.AddPage("TestNet");

    // Color set declarations
    x.DeclareUnitColorSet("UNIT");
    x.DeclareBooleanColorSet("BOOL");
    x.DeclareIntegerColorSet("INT");
    x.DeclareLargeIntegerColorSet("INTINF");
    x.DeclareRealColorSet("REAL");
    x.DeclareTimeColorSet("TIME");
    x.DeclareStringColorSet("STRING");
    x.DeclareEnumeratedColorSet("DAYS", {"MON", "TUES", "THURS", "WEDNES", "FRI", "SATUR", "SUN"});
    x.DeclareIndexColorSet("PH", "1..5");
    x.DeclareAliasColorSets("DATA", "STRING");
    x.DeclareProductColorSet("PACKAGE", {"DATA", "STRING", "INT", "BOOL"});

    x.Dump();
    return 0;
}