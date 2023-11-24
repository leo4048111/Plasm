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

    int id = cpnxml_->AddPlace(
        pageId_,
        name,
        "UNIT");

    return id;
}

int CPNIDEGenerator::makeIEnd()
{
    static int iEndCnt = 0;
    ::std::string name = "IEnd_" + ::std::to_string(iEndCnt++);

    int id = cpnxml_->AddPlace(
        pageId_,
        name,
        "UNIT");

    return id;
}

void CPNIDEGenerator::addSymbolEntry(int64_t id, ::std::string name, int cpnid, ::std::string type)
{
    symbol_name_tbl_[id] = name;
    symbol_id_tbl_[id] = cpnid;
    symbol_type_tbl_[id] = type;
}

void CPNIDEGenerator::Dump() const
{
    if (cpnxml_)
        cpnxml_->Dump();
}

bool CPNIDEGenerator::visit(SourceUnit const &_node)
{
    LOGT("CPNIDEGenerator in %s", "SourceUnit");
    return true;
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
    LOGT("CPNIDEGenerator in %s", "CPNIDEGenerator");
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
    PSM_ASSERT(pageId_ != -1);
    int id = cpnxml_->AddPlace(pageId_, name, TypeCategory2String(category));

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
    int lhsId = symbol_id_tbl_[_node.leftHandSide().id()];
    int rhsId = symbol_id_tbl_[_node.rightHandSide().id()];

    ::std::string name = "assignment_" + ::std::to_string(_node.id());
    int transId = cpnxml_->AddTransition(pageId_, name);

    int iStartId = makeIStart();
    int iEndId = makeIEnd();

    auto var1 = makeVar(symbol_type_tbl_[id1]);
    auto var2 = makeVar(symbol_type_tbl_[id2]);

    // IStart to trans
    cpnxml_->AddArc(
        pageId_,
        CPNXml::Orientation::PLACE_TO_TRANSITION,
        transId,
        iStartId,
        CONTROL_TOKEN_ANNOT);

    // trans to IEnd
    cpnxml_->AddArc(
        pageId_,
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        transId,
        iEndId,
        CONTROL_TOKEN_ANNOT);

    // Get value from right
    cpnxml_->AddArc(
        pageId_,
        CPNXml::Orientation::BIDIRECTIONAL,
        transId,
        rhsId,
        var1);

    // Set value to left
    cpnxml_->AddArc( // assign
        pageId_,
        CPNXml::Orientation::TRANSITION_TO_PLACE,
        transId,
        lhsId,
        var1);

    cpnxml_->AddArc( // remove old value
        pageId_,
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
