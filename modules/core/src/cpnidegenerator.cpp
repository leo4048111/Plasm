#include "cpnidegenerator.hpp"

#include "logger.hpp"

_START_PSM_NM_

using namespace solidity::frontend;

static std::string TypeCategory2String(Type::Category category)
{
    switch (category)
    {
    case Type::Category::Address:
        return "Address";
    case Type::Category::Integer:
        return "Integer";
    case Type::Category::RationalNumber:
        return "RationalNumber";
    case Type::Category::StringLiteral:
        return "StringLiteral";
    case Type::Category::Bool:
        return "Bool";
    case Type::Category::FixedPoint:
        return "FixedPoint";
    case Type::Category::Array:
        return "Array";
    case Type::Category::ArraySlice:
        return "ArraySlice";
    case Type::Category::FixedBytes:
        return "FixedBytes";
    case Type::Category::Contract:
        return "Contract";
    case Type::Category::Struct:
        return "Struct";
    case Type::Category::Function:
        return "Function";
    case Type::Category::Enum:
        return "Enum";
    case Type::Category::UserDefinedValueType:
        return "UserDefinedValueType";
    case Type::Category::Tuple:
        return "Tuple";
    case Type::Category::Mapping:
        return "Mapping";
    case Type::Category::TypeType:
        return "TypeType";
    case Type::Category::Modifier:
        return "Modifier";
    case Type::Category::Magic:
        return "Magic";
    case Type::Category::Module:
        return "Module";
    case Type::Category::InaccessibleDynamic:
        return "InaccessibleDynamic";
    default:
        return "Unknown";
    }
}

CPNIDEGenerator::CPNIDEGenerator(){};

void CPNIDEGenerator::toCPN(solidity::frontend::ASTNode const &_node)
{
    _node.accept(*this);
}

void CPNIDEGenerator::bridgeControlPlaces(int from, int to)
{
    static int bridgeCnt = 0;
    ::std::string name = "bridge_" + ::std::to_string(bridgeCnt++);
    int bridgeTransId = addTransition(name);

    // from => bridge => to
    addArc(
        CPNXml::Orientation::PLACE_TO_TRANSITION,
        bridgeTransId,
        from,
        CONTROL_TOKEN_ANNOT
    );

    addArc(
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        bridgeTransId,
        to,
        CONTROL_TOKEN_ANNOT
    );
}

::std::string CPNIDEGenerator::makeVar(::std::string type)
{
    static int varCnt = 0;
    ::std::string name = "var_" + ::std::to_string(varCnt++);

    cpnxml_->DeclareVar(name, type);

    return name;
}

int CPNIDEGenerator::makeIStart()
{
    static int iStartCnt = 0;
    ::std::string name = "IStart_" + ::std::to_string(iStartCnt++);

    int id = addPlace(name, "UNIT");
    if(lastIEndId != -1) {
        bridgeControlPlaces(lastIEndId, id);
    }

    return id;
}

int CPNIDEGenerator::makeIEnd()
{
    static int iEndCnt = 0;
    ::std::string name = "IEnd_" + ::std::to_string(iEndCnt++);

    int id = addPlace(name, "UNIT");

    lastIEndId = id;

    return id;
}

void CPNIDEGenerator::addSymbolEntry(int64_t id, ::std::string name, int cpnid, ::std::string type)
{
    symbol_name_tbl_[id] = name;
    symbol_id_tbl_[id] = cpnid;
    symbol_type_tbl_[id] = type;
}

void CPNIDEGenerator::dump() const
{
    if (cpnxml_)
        cpnxml_->Dump();
}

int CPNIDEGenerator::addPlace(::std::string name, ::std::string type, ::std::optional<::std::string> initial_marking)
{
    int id = cpnxml_->AddPlace(pageId_, name, type, 0.f, 0.f, initial_marking);

    // Save place id
    places_.insert(id);

    // // Add place to graph
    // ogdf::node placeNode = graph_.newNode();

    // // Mapping
    // mapping_onodes_cnodes_.insert(::std::make_pair(placeNode->index(), id));

    return id;
}

int CPNIDEGenerator::addTransition(::std::string name)
{
    int id = cpnxml_->AddTransition(pageId_, name, 0.f, 0.f);

    // Save transition id
    transitions_.insert(id);

    // // Add transition to graph
    // ogdf::node transNode = graph_.newNode();

    // // Mapping
    // mapping_onodes_cnodes_.insert(::std::make_pair(transNode->index(), id));

    return id;
}

void CPNIDEGenerator::addArc(
    CPNXml::Orientation orientation,
    int transendId,
    int placeendId,
    ::std::optional<::std::string> annotation)
{
    cpnxml_->AddArc(
        pageId_,
        orientation,
        transendId,
        placeendId,
        annotation);
}

bool CPNIDEGenerator::visit(SourceUnit const &_node)
{
    LOGT("CPNIDEGenerator in %s", "SourceUnit");
    return true;
}

void CPNIDEGenerator::endVisit(SourceUnit const &_node)
{
    // This will be called at the end of compilation
    LOGT("CPNIDEGenerator endVisit %s", "SourceUnit");

    // ogdf::GraphAttributes ga(graph_,
    //                          ogdf::GraphAttributes::nodeGraphics |
    //                              ogdf::GraphAttributes::edgeGraphics |
    //                              ogdf::GraphAttributes::nodeLabel |
    //                              ogdf::GraphAttributes::edgeStyle |
    //                              ogdf::GraphAttributes::nodeStyle |
    //                              ogdf::GraphAttributes::nodeTemplate);

    // ogdf::SugiyamaLayout SL;
    // SL.setCrossMin(new ogdf::MedianHeuristic);
    // SL.arrangeCCs(false);

    // ogdf::OptimalHierarchyLayout *ohl = new ogdf::OptimalHierarchyLayout;
    // ohl->layerDistance(30.0);
    // ohl->nodeDistance(25.0);
    // ohl->weightBalancing(0.7);
    // SL.setLayout(ohl);

    // SL.call(ga);

    // // Adjust node positions
    // for (ogdf::node v : graph_.nodes)
    // {
    //     int id = mapping_onodes_cnodes_[v->index()];
    //     if (places_.count(id))
    //     {
    //         cpnxml_->MovePlace(id, ga.x(v), ga.y(v));
    //     }
    //     else if (transitions_.count(id))
    //     {
    //         cpnxml_->MoveTransition(id, ga.x(v), ga.y(v));
    //     }
    // }
}

bool CPNIDEGenerator::visit(PragmaDirective const &_node)
{
    LOGT("CPNIDEGenerator in %s", "PragmaDirective");
    return true;
}

bool CPNIDEGenerator::visit(ImportDirective const &_node)
{
    LOGT("CPNIDEGenerator in %s", "ImportDirective");
    return true;
}

bool CPNIDEGenerator::visit(ContractDefinition const &_node)
{
    LOGT("CPNIDEGenerator in %s", "ContractDefinition");
    // Init cpnxml
    cpnxml_ = ::std::make_unique<CPNXml>(_node.name());
    pageId_ = cpnxml_->AddPage(_node.name());

    // Init types: Mapping to cpnide
    // enum class Category
    // {
    // 	Address -> string
    // 	Integer -> integer
    // 	RationalNumber -> INTEGER
    // 	StringLiteral -> String
    // 	Bool -> bool
    // 	FixedPoint -> real
    // 	Array -> *** Not available temporarily ***
    // 	ArraySlice -> *** Not available temporarily ***
    // 	FixedBytes -> *** Not available temporarily ***
    // 	Contract -> Not variable
    // 	Struct -> Colorset with Struct name
    // 	Function -> Not variable
    // 	Enum -> Enumerated
    // 	UserDefinedValueType -> *** Not available temporarily ***
    // 	Tuple  -> *** Not available temporarily ***
    // 	Mapping -> *** Not available temporarily ***
    // 	TypeType -> *** Not available temporarily ***
    // 	Modifier -> *** Not available temporarily ***
    // 	Magic -> *** Not available temporarily ***
    // 	Module -> *** Not available temporarily ***
    // 	InaccessibleDynamic -> *** Not available temporarily ***
    // };

    cpnxml_->DeclareStringColorSet("Address");
    cpnxml_->DeclareIntegerColorSet("Integer");
    cpnxml_->DeclareIntegerColorSet("RationalNumber");
    cpnxml_->DeclareStringColorSet("StringLiteral");
    cpnxml_->DeclareBooleanColorSet("Bool");
    cpnxml_->DeclareRealColorSet("FixedPoint");
    // TODO enum
    // TODO struct

    // Control unit
    cpnxml_->DeclareUnitColorSet("UNIT");

    return true;
}

bool CPNIDEGenerator::visit(IdentifierPath const &_node)
{
    LOGT("CPNIDEGenerator in %s", "IdentifierPath");
    return true;
}

bool CPNIDEGenerator::visit(InheritanceSpecifier const &_node)
{
    LOGT("CPNIDEGenerator in %s", "InheritanceSpecifier");
    return true;
}

bool CPNIDEGenerator::visit(UsingForDirective const &_node)
{
    LOGT("CPNIDEGenerator in %s", "UsingForDirective");
    return true;
}

bool CPNIDEGenerator::visit(StructDefinition const &_node)
{
    LOGT("CPNIDEGenerator in %s", "StructDefinition");
    return true;
}

bool CPNIDEGenerator::visit(EnumDefinition const &_node)
{
    LOGT("CPNIDEGenerator in %s", "EnumDefinition");
    return true;
}

bool CPNIDEGenerator::visit(EnumValue const &_node)
{
    LOGT("CPNIDEGenerator in %s", "EnumValue");
    return true;
}

bool CPNIDEGenerator::visit(UserDefinedValueTypeDefinition const &_node)
{
    LOGT("CPNIDEGenerator in %s", "UserDefinedValueTypeDefinition");
    return true;
}

bool CPNIDEGenerator::visit(ParameterList const &_node)
{
    LOGT("CPNIDEGenerator in %s", "ParameterList");
    return true;
}

bool CPNIDEGenerator::visit(OverrideSpecifier const &_node)
{
    LOGT("CPNIDEGenerator in %s", "OverrideSpecifier");
    return true;
}

bool CPNIDEGenerator::visit(FunctionDefinition const &_node)
{
    LOGT("CPNIDEGenerator in %s", "FunctionDefinition");
    return true;
}

bool CPNIDEGenerator::visit(VariableDeclaration const &_node)
{
    LOGT("CPNIDEGenerator in %s", "VariableDeclaration");
    return true;
}

void CPNIDEGenerator::endVisit(VariableDeclaration const &_node)
{
    LOGT("CPNIDEGenerator endVisit %s", "VariableDeclaration");
    // Simply make a place for that value
    auto category = _node.type()->category();
    auto name = _node.name();
    auto type = TypeCategory2String(category);
    // Add var decl
    cpnxml_->DeclareVar(name, type);

    // Add place for var storage
    int id = addPlace(name, TypeCategory2String(category));

    // Register symbol
    addSymbolEntry(_node.id(), name, id, type);

    // Add mapping from name to id
    variable_name_id[name] = _node.id();
}

bool CPNIDEGenerator::visit(ModifierDefinition const &_node)
{
    LOGT("CPNIDEGenerator in %s", "ModifierDefinition");
    return true;
}

bool CPNIDEGenerator::visit(ModifierInvocation const &_node)
{
    LOGT("CPNIDEGenerator in %s", "ModifierInvocation");
    return true;
}

bool CPNIDEGenerator::visit(EventDefinition const &_node)
{
    LOGT("CPNIDEGenerator in %s", "EventDefinition");
    return true;
}

bool CPNIDEGenerator::visit(ErrorDefinition const &_node)
{
    LOGT("CPNIDEGenerator in %s", "ErrorDefinition");
    return true;
}

bool CPNIDEGenerator::visit(ElementaryTypeName const &_node)
{
    LOGT("CPNIDEGenerator in %s", "ErrorDefinition");
    return true;
}

bool CPNIDEGenerator::visit(UserDefinedTypeName const &_node)
{
    LOGT("CPNIDEGenerator in %s", "UserDefinedTypeName");
    return true;
}

bool CPNIDEGenerator::visit(FunctionTypeName const &_node)
{
    LOGT("CPNIDEGenerator in %s", "FunctionTypeName");
    return true;
}

bool CPNIDEGenerator::visit(Mapping const &_node)
{
    LOGT("CPNIDEGenerator in %s", "Mapping");
    return true;
}

bool CPNIDEGenerator::visit(ArrayTypeName const &_node)
{
    LOGT("CPNIDEGenerator in %s", "ArrayTypeName");
    return true;
}

bool CPNIDEGenerator::visit(InlineAssembly const &_node)
{
    LOGT("CPNIDEGenerator in %s", "InlineAssembly");
    return true;
}

bool CPNIDEGenerator::visit(Block const &_node)
{
    LOGT("CPNIDEGenerator in %s", "Block");
    return true;
}

bool CPNIDEGenerator::visit(PlaceholderStatement const &_node)
{
    LOGT("CPNIDEGenerator in %s", "PlaceholderStatement");
    return true;
}

bool CPNIDEGenerator::visit(IfStatement const &_node)
{
    LOGT("CPNIDEGenerator in %s", "IfStatement");
    return true;
}

void CPNIDEGenerator::endVisit(IfStatement const &_node)
{
    LOGT("CPNIDEGenerator endVisit %s", "IfStatement");
    auto id1 = _node.condition().id();
    auto id2 = _node.trueStatement().id();
    // conditional else body
    auto elseBody = _node.falseStatement();
    bool hasElse = !(elseBody == nullptr);
    int condId = symbol_id_tbl_[id1];
    int trueId = symbol_id_tbl_[id2];

    ::std::string name = "ifstatement_" + ::std::to_string(_node.id());
    int ifStatId = addTransition(name);

    int iStartId = makeIStart();
    int iEndId = makeIEnd();

    auto condVar = makeVar("Bool");
    ::std::string ifElseAnnot = "if " + condVar + " then 1`() else empty";

    // IStart to trans
    addArc(
        CPNXml::Orientation::PLACE_TO_TRANSITION,
        iStartId,
        ifStatId,
        CONTROL_TOKEN_ANNOT);

    // IfElseStatement to condition
    addArc(
        CPNXml::Orientation::BIDIRECTIONAL,
        ifStatId,
        condId,
        condVar);

    ::std::string name2 = "ibodystart_" + ::std::to_string(_node.id());
    ::std::string name3 = "ibodyend_" + ::std::to_string(_node.id());
    int iBodyStartId = addPlace(name2, "UNIT");
    int iBodyEndId = addPlace(name3, "UNIT");

    // IfElseStatement to ibodystart
    addArc(
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        iBodyStartId,
        ifStatId,
        CONTROL_TOKEN_ANNOT);

    int bodyExpr = addTransition("iftruebody_" + ::std::to_string(_node.id()));

    // ibodystart to bodyexpr
    addArc(
        CPNXml::Orientation::PLACE_TO_TRANSITION,
        iBodyStartId,
        bodyExpr,
        CONTROL_TOKEN_ANNOT
    );

    // bodyexpr to ibodyend
    addArc(
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        iBodyEndId,
        bodyExpr,
        CONTROL_TOKEN_ANNOT
    );

    // ibodyend to endif
    int endifid = addTransition("endif_" + ::std::to_string(_node.id()));
    addArc(
        CPNXml::Orientation::PLACE_TO_TRANSITION,
        endifid,
        iBodyEndId,
        CONTROL_TOKEN_ANNOT
    );

    // endif to iend
    addArc(
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        endifid,
        iEndId,
        CONTROL_TOKEN_ANNOT
    );

    // ifstatement to iend
    ::std::string ifElseAnnot2 = "if " + condVar + " then empty else 1`()";
    addArc(
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        ifStatId,
        iEndId,
        ifElseAnnot2
    );
    // TODO: else body
}

bool CPNIDEGenerator::visit(TryCatchClause const &_node)
{
    LOGT("CPNIDEGenerator in %s", "TryCatchClause");
    return true;
}

bool CPNIDEGenerator::visit(TryStatement const &_node)
{
    LOGT("CPNIDEGenerator in %s", "TryStatement");
    return true;
}

bool CPNIDEGenerator::visit(WhileStatement const &_node)
{
    LOGT("CPNIDEGenerator in %s", "WhileStatement");
    return true;
}

void CPNIDEGenerator::endVisit(WhileStatement const &_node)
{
    LOGT("CPNIDEGenerator endVisit %s", "WhileStatement");
    // TODO: implementation

    int iStartId = makeIStart();
    int iEndId = makeIEnd();

    ::std::string name = "WhileStatement_" + ::std::to_string(_node.id());
    int whileStatId = addTransition(name);

    // Istart to whilestatement
    addArc(
        CPNXml::Orientation::PLACE_TO_TRANSITION,
        whileStatId,
        iStartId,
        CONTROL_TOKEN_ANNOT);

    ::std::string name2 = "icondstart_" + ::std::to_string(_node.id());
    ::std::string name3 = "icondend_" + ::std::to_string(_node.id());
    int iCondStartId = addPlace(name2, "UNIT");
    int iCondEndId = addPlace(name3, "UNIT");
    // FIXME: get condExpr by id
    ::std::string name4 = "condexpr_" + ::std::to_string(_node.id());
    int condExprId = addTransition(name4);

    // whilestatement to icondstart
    addArc(
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        whileStatId,
        iCondStartId,
        CONTROL_TOKEN_ANNOT);

    // icondstart to condExpr
    addArc(
        CPNXml::Orientation::PLACE_TO_TRANSITION,
        condExprId,
        iCondStartId,
        CONTROL_TOKEN_ANNOT);

    // condExpr to icondend
    addArc(
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        condExprId,
        iCondEndId,
        CONTROL_TOKEN_ANNOT
    );

    // FIXME: condresult should be generated by condExpr
    int condresultid = addPlace(
        "condresult_" + ::std::to_string(_node.id()),
        "Bool"
    );

    // condExpr to condresult
    addArc(
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        condExprId,
        condresultid,
        "1`true"
    );

    // loop transition
    int loopid = addTransition("loop_" + ::std::to_string(_node.id()));

    // condresult to loop
    auto var1 = makeVar("Bool");
    addArc(
        CPNXml::Orientation::PLACE_TO_TRANSITION,
        loopid,
        condresultid,
        var1
    );

    // loop to iend
    ::std::string loopToIendAnnot = "if " + var1 + " then empty else 1`()";
    addArc(
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        loopid,
        iEndId,
        loopToIendAnnot
    );

    // while body defs
    ::std::string name5 = "ibodystart_" + ::std::to_string(_node.id());
    ::std::string name6 = "ibodyend_" + ::std::to_string(_node.id());
    int iBodyStartId = addPlace(name5, "UNIT");
    int iBodyEndId = addPlace(name6, "UNIT");

    // loop to ibodystart
    ::std::string loopToIbodyStartAnnot = "if " + var1 + " then 1`() else empty";
    addArc(
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        loopid,
        iBodyStartId,
        loopToIbodyStartAnnot
    );

    // FIXME: bodyExpr should be generated from _node.body()
    int bodyexprid = addTransition("bodyexpr_" + ::std::to_string(_node.id()));

    // ibodystart to bodyexpr
    addArc(
        CPNXml::Orientation::PLACE_TO_TRANSITION,
        bodyexprid,
        iBodyStartId,
        CONTROL_TOKEN_ANNOT
    );

    // bodyexpr to ibodyend
    addArc(
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        bodyexprid,
        iBodyEndId,
        CONTROL_TOKEN_ANNOT
    );

    // again transition
    int againid = addTransition("again_" + ::std::to_string(_node.id()));

    // ibodyend to again
    addArc(
        CPNXml::Orientation::PLACE_TO_TRANSITION,
        againid,
        iBodyEndId,
        CONTROL_TOKEN_ANNOT
    );

    // again to icondstart
    addArc(
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        againid,
        iCondStartId,
        CONTROL_TOKEN_ANNOT
    );
}

bool CPNIDEGenerator::visit(ForStatement const &_node)
{
    LOGT("CPNIDEGenerator in %s", "ForStatement");
    return true;
}

bool CPNIDEGenerator::visit(Continue const &_node)
{
    LOGT("CPNIDEGenerator in %s", "Continue");
    return true;
}

bool CPNIDEGenerator::visit(Break const &_node)
{
    LOGT("CPNIDEGenerator in %s", "Break");
    return true;
}

bool CPNIDEGenerator::visit(Return const &_node)
{
    LOGT("CPNIDEGenerator in %s", "Return");
    return true;
}

bool CPNIDEGenerator::visit(Throw const &_node)
{
    LOGT("CPNIDEGenerator in %s", "Throw");
    return true;
}

bool CPNIDEGenerator::visit(EmitStatement const &_node)
{
    LOGT("CPNIDEGenerator in %s", "EmitStatement");
    return true;
}

bool CPNIDEGenerator::visit(RevertStatement const &_node)
{
    LOGT("CPNIDEGenerator in %s", "RevertStatement");
    return true;
}

bool CPNIDEGenerator::visit(VariableDeclarationStatement const &_node)
{
    LOGT("CPNIDEGenerator in %s", "VariableDeclarationStatement");
    return true;
}

bool CPNIDEGenerator::visit(ExpressionStatement const &_node)
{
    LOGT("CPNIDEGenerator in %s", "ExpressionStatement");
    return true;
}

bool CPNIDEGenerator::visit(Conditional const &_node)
{
    LOGT("CPNIDEGenerator in %s", "Conditional");
    return true;
}

bool CPNIDEGenerator::visit(Assignment const &_node)
{
    LOGT("CPNIDEGenerator in %s", "Assignment");
    return true;
}

void CPNIDEGenerator::endVisit(Assignment const &_node)
{
    LOGT("CPNIDEGenerator endVisit %s", "Assignment");
    auto id1 = _node.leftHandSide().id();
    auto id2 = _node.rightHandSide().id();
    int lhsId = symbol_id_tbl_[id1];
    int rhsId = symbol_id_tbl_[id2];

    ::std::string name = "assignment_" + ::std::to_string(_node.id());
    int transId = addTransition(name);

    int iStartId = makeIStart();
    int iEndId = makeIEnd();

    auto var1 = makeVar(symbol_type_tbl_[id1]);
    auto var2 = makeVar(symbol_type_tbl_[id2]);

    // IStart to trans
    addArc(
        CPNXml::Orientation::PLACE_TO_TRANSITION,
        transId,
        iStartId,
        CONTROL_TOKEN_ANNOT);

    // trans to IEnd
    addArc(
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        transId,
        iEndId,
        CONTROL_TOKEN_ANNOT);

    // Get value from right
    addArc(
        CPNXml::Orientation::BIDIRECTIONAL,
        transId,
        rhsId,
        var1);

    // Set value to left
    addArc( // assign
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        transId,
        lhsId,
        var1);

    addArc( // remove old value
        CPNXml::Orientation::PLACE_TO_TRANSITION,
        transId,
        lhsId,
        var2);
}

bool CPNIDEGenerator::visit(TupleExpression const &_node)
{
    LOGT("CPNIDEGenerator in %s", "TupleExpression");
    return true;
}

bool CPNIDEGenerator::visit(UnaryOperation const &_node)
{
    LOGT("CPNIDEGenerator in %s", "UnaryOperation");
    return true;
}

bool CPNIDEGenerator::visit(BinaryOperation const &_node)
{
    LOGT("CPNIDEGenerator in %s", "BinaryOperation");
    return true;
}

void CPNIDEGenerator::endVisit(BinaryOperation const &_node) {
    LOGT("CPNIDEGenerator endVisit %s", "BinaryOperation");

    auto id1 = _node.leftExpression().id();
    auto id2 = _node.rightExpression().id();
    int lhsId = symbol_id_tbl_[id1];
    int rhsId = symbol_id_tbl_[id2];
    auto op = _node.getOperator();

    // transition for operation
    ::std::string name = "bop_" + ::std::to_string(_node.id());
    int transId = addTransition(name);

    // result place
    // TODO: Implicit type convertion
    ::std::string name2 = "b_result_" + ::std::to_string(_node.id());
    int resultId = addPlace(name2, symbol_type_tbl_[id1]);

    int iStartId = makeIStart();
    int iEndId = makeIEnd();

    auto var1 = makeVar(symbol_type_tbl_[id1]);
    auto var2 = makeVar(symbol_type_tbl_[id2]);

    // IStart to trans
    addArc(
        CPNXml::Orientation::PLACE_TO_TRANSITION,
        transId,
        iStartId,
        CONTROL_TOKEN_ANNOT);

    // trans to IEnd
    addArc(
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        transId,
        iEndId,
        CONTROL_TOKEN_ANNOT);

    // get value from right
    addArc(
        CPNXml::Orientation::BIDIRECTIONAL,
        transId,
        rhsId,
        var1);

    // get value from left
    addArc(
        CPNXml::Orientation::BIDIRECTIONAL,
        transId,
        lhsId,
        var2);

    // set value to result
    auto exp = var1 + solidity::langutil::TokenTraits::toString(op) + var2;
    addArc(
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        transId,
        resultId,
        exp
    );

    // register result
    addSymbolEntry(_node.id(), name, resultId, symbol_type_tbl_[id1]);
}

bool CPNIDEGenerator::visit(FunctionCall const &_node)
{
    LOGT("CPNIDEGenerator in %s", "FunctionCall");
    return true;
}

bool CPNIDEGenerator::visit(FunctionCallOptions const &_node)
{
    LOGT("CPNIDEGenerator in %s", "FunctionCallOptions");
    return true;
}

bool CPNIDEGenerator::visit(NewExpression const &_node)
{
    LOGT("CPNIDEGenerator in %s", "NewExpression");
    return true;
}

bool CPNIDEGenerator::visit(MemberAccess const &_node)
{
    LOGT("CPNIDEGenerator in %s", "MemberAccess");
    return true;
}

bool CPNIDEGenerator::visit(IndexAccess const &_node)
{
    LOGT("CPNIDEGenerator in %s", "IndexAccess");
    return true;
}

bool CPNIDEGenerator::visit(IndexRangeAccess const &_node)
{
    LOGT("CPNIDEGenerator in %s", "IndexRangeAccess");
    return true;
}

bool CPNIDEGenerator::visit(Identifier const &_node)
{
    LOGT("CPNIDEGenerator in %s", "Identifier");

    ::std::string name = _node.name();
    int id = variable_name_id[name];
    int cpnid = symbol_id_tbl_[id];
    ::std::string type = symbol_type_tbl_[id];
    addSymbolEntry(_node.id(), name, cpnid, type);

    return true;
}

bool CPNIDEGenerator::visit(ElementaryTypeNameExpression const &_node)
{
    LOGT("CPNIDEGenerator in %s", "ElementaryTypeNameExpression");
    return true;
}

bool CPNIDEGenerator::visit(Literal const &_node)
{
    LOGT("CPNIDEGenerator in %s", "Literal");
    return true;
}

bool CPNIDEGenerator::visit(StructuredDocumentation const &_node)
{
    LOGT("CPNIDEGenerator in %s", "StructuredDocumentation");
    return true;
}

_END_PSM_NM_
