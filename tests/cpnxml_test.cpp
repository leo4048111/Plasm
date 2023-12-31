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

    // Var declarations
    x.DeclareVar("v", "UNIT");
    x.DeclareVar("cond", "BOOL");
    x.DeclareVar({"x", "y", "z"}, "INT");

    // Add place
    int placeId = x.AddPlace(pageId1, "IStart", "UNIT",  100, 100, "5`()");
    int transId = x.AddTransition(pageId1, "BTrans", 100, 100);
    int placeEndId = x.AddPlace(pageId1, "IEnd", "UNIT", 100, 100, "5`()");
    int trans2Id = x.AddTransition(pageId1, "BTrans2", 100, 100);
    int placeEnd2Id = x.AddPlace(pageId1, "IEnd2", "UNIT", 100, 100, "");
    x.AddArc(
        pageId1,
        PSM_NM::CPNXml::Orientation::PLACE_TO_TRANSITION,
        transId,
        placeId,
        "if cond then 1`() else empty");

    x.AddArc(
        pageId1,
        PSM_NM::CPNXml::Orientation::TRANSITION_TO_PLACE,
        transId,
        placeEndId,
        "1`()");

    x.AddArc(
        pageId1,
        PSM_NM::CPNXml::Orientation::PLACE_TO_TRANSITION,
        trans2Id,
        placeEndId,
        "1`()");

    x.AddArc(
        pageId1,
        PSM_NM::CPNXml::Orientation::TRANSITION_TO_PLACE,
        trans2Id,
        placeEnd2Id,
        "1`()");

    x.Dump();
    return 0;
}