#include "generator.hpp"

#include "logger.hpp"
#include "cpn.hpp"

_START_PSM_NM_

using namespace solidity::frontend;

Generator::Generator()
{
    network_ = ::std::make_unique<cpn::Network>();
};

void Generator::toCPN(solidity::frontend::ASTNode const &_node)
{
    _node.accept(*this);
}

bool Generator::visit(SourceUnit const &_node)
{
    LOGI("Generator in %s", "SourceUnit");
    pushScope(SCOPE_GLOB);
    return true;
}

void Generator::endVisit(SourceUnit const &_node)
{
    popScope();
}

bool Generator::visit(PragmaDirective const &_node)
{
    LOGI("Generator in %s", "PragmaDirective");
    return true;
}

bool Generator::visit(ImportDirective const &_node)
{
    LOGI("Generator in %s", "ImportDirective");
    return true;
}

// parse topnet decls
bool Generator::visit(ContractDefinition const &_node)
{
    LOGI("Generator in %s", "ContractDefinition");
    pushScope(SCOPE_GLOB);

    // register global vars to place
    for (auto const &var : _node.stateVariables())
    {
        auto name = var->name();
        auto type = var->typeName().annotation().type->toString();

        // create places
        ::std::shared_ptr<cpn::Place> place = ::std::make_shared<cpn::Place>(scope() + name, type);
        network_->addPlace(place);
    }

    // register global msg
    ::std::shared_ptr<cpn::Place> msgPlace = ::std::make_shared<cpn::Place>(scope() + VAR_MSG, TYPE_STRUCT);
    network_->addPlace(msgPlace);

    // register global this
    ::std::shared_ptr<cpn::Place> thisPlace = ::std::make_shared<cpn::Place>(scope() + VAR_THIS, TYPE_STRUCT);
    network_->addPlace(thisPlace);
    return true;
}

void Generator::endVisit(ContractDefinition const &_node)
{
    popScope();
}

bool Generator::visit(IdentifierPath const &_node)
{
    LOGI("Generator in %s", "IdentifierPath");
    return true;
}

bool Generator::visit(InheritanceSpecifier const &_node)
{
    LOGI("Generator in %s", "InheritanceSpecifier");
    return true;
}

bool Generator::visit(UsingForDirective const &_node)
{
    LOGI("Generator in %s", "UsingForDirective");
    return true;
}

bool Generator::visit(StructDefinition const &_node)
{
    LOGI("Generator in %s", "StructDefinition");
    return true;
}

bool Generator::visit(EnumDefinition const &_node)
{
    LOGI("Generator in %s", "EnumDefinition");
    return true;
}

bool Generator::visit(EnumValue const &_node)
{
    LOGI("Generator in %s", "EnumValue");
    return true;
}

bool Generator::visit(UserDefinedValueTypeDefinition const &_node)
{
    LOGI("Generator in %s", "UserDefinedValueTypeDefinition");
    return true;
}

bool Generator::visit(ParameterList const &_node)
{
    LOGI("Generator in %s", "ParameterList");
    return true;
}

bool Generator::visit(OverrideSpecifier const &_node)
{
    LOGI("Generator in %s", "OverrideSpecifier");
    return true;
}

bool Generator::visit(FunctionDefinition const &_node)
{
    LOGI("Generator in %s", "FunctionDefinition");
    pushScope(_node.name() + ".");

    // register function parameters
    for (auto const &param : _node.parameters())
    {
        auto name = param->name();
        auto type = param->typeName().annotation().type->toString();

        // create places
        ::std::shared_ptr<cpn::Place> place = ::std::make_shared<cpn::Place>(scope() + SCOPE_PARAM + name, type);
        network_->addPlace(place);
    }

    // register function return values
    for (auto const &ret : _node.returnParameters())
    {
        auto name = ret->name();
        auto type = ret->typeName().annotation().type->toString();

        // create places
        ::std::shared_ptr<cpn::Place> place = ::std::make_shared<cpn::Place>(scope() + SCOPE_RET + name, type);
        network_->addPlace(place);
    }

    return true;
}

void Generator::endVisit(FunctionDefinition const &_node)
{
    popScope();
}

bool Generator::visit(VariableDeclaration const &_node)
{
    LOGI("Generator in %s", "VariableDeclaration");
    return true;
}

bool Generator::visit(ModifierDefinition const &_node)
{
    LOGI("Generator in %s", "ModifierDefinition");
    return true;
}

bool Generator::visit(ModifierInvocation const &_node)
{
    LOGI("Generator in %s", "ModifierInvocation");
    return true;
}

bool Generator::visit(EventDefinition const &_node)
{
    LOGI("Generator in %s", "EventDefinition");
    return true;
}

bool Generator::visit(ErrorDefinition const &_node)
{
    LOGI("Generator in %s", "ErrorDefinition");
    return true;
}

bool Generator::visit(ElementaryTypeName const &_node)
{
    LOGI("Generator in %s", "ErrorDefinition");
    return true;
}

bool Generator::visit(UserDefinedTypeName const &_node)
{
    LOGI("Generator in %s", "UserDefinedTypeName");
    return true;
}

bool Generator::visit(FunctionTypeName const &_node)
{
    LOGI("Generator in %s", "FunctionTypeName");
    return true;
}

bool Generator::visit(Mapping const &_node)
{
    LOGI("Generator in %s", "Mapping");
    return true;
}

bool Generator::visit(ArrayTypeName const &_node)
{
    LOGI("Generator in %s", "ArrayTypeName");
    return true;
}

bool Generator::visit(InlineAssembly const &_node)
{
    LOGI("Generator in %s", "InlineAssembly");
    return true;
}

bool Generator::visit(Block const &_node)
{
    LOGI("Generator in %s", "Block");
    return true;
}

bool Generator::visit(PlaceholderStatement const &_node)
{
    LOGI("Generator in %s", "PlaceholderStatement");
    return true;
}

bool Generator::visit(IfStatement const &_node)
{
    LOGI("Generator in %s", "IfStatement");
    return true;
}

bool Generator::visit(TryCatchClause const &_node)
{
    LOGI("Generator in %s", "TryCatchClause");
    return true;
}

bool Generator::visit(TryStatement const &_node)
{
    LOGI("Generator in %s", "TryStatement");
    return true;
}

bool Generator::visit(WhileStatement const &_node)
{
    LOGI("Generator in %s", "WhileStatement");
    return true;
}

bool Generator::visit(ForStatement const &_node)
{
    LOGI("Generator in %s", "ForStatement");
    return true;
}

bool Generator::visit(Continue const &_node)
{
    LOGI("Generator in %s", "Continue");
    return true;
}

bool Generator::visit(Break const &_node)
{
    LOGI("Generator in %s", "Break");
    return true;
}

bool Generator::visit(Return const &_node)
{
    LOGI("Generator in %s", "Return");
    return true;
}

bool Generator::visit(Throw const &_node)
{
    LOGI("Generator in %s", "Throw");
    return true;
}

bool Generator::visit(EmitStatement const &_node)
{
    LOGI("Generator in %s", "EmitStatement");
    return true;
}

bool Generator::visit(RevertStatement const &_node)
{
    LOGI("Generator in %s", "RevertStatement");
    return true;
}

bool Generator::visit(VariableDeclarationStatement const &_node)
{
    LOGI("Generator in %s", "VariableDeclarationStatement");
    return true;
}

bool Generator::visit(ExpressionStatement const &_node)
{
    LOGI("Generator in %s", "ExpressionStatement");
    return true;
}

bool Generator::visit(Conditional const &_node)
{
    LOGI("Generator in %s", "Conditional");
    return true;
}

bool Generator::visit(Assignment const &_node)
{
    LOGI("Generator in %s", "Assignment");
    return true;
}

bool Generator::visit(TupleExpression const &_node)
{
    LOGI("Generator in %s", "TupleExpression");
    return true;
}

bool Generator::visit(UnaryOperation const &_node)
{
    LOGI("Generator in %s", "UnaryOperation");
    return true;
}

bool Generator::visit(BinaryOperation const &_node)
{
    LOGI("Generator in %s", "BinaryOperation");
    return true;
}

bool Generator::visit(FunctionCall const &_node)
{
    LOGI("Generator in %s", "FunctionCall");
    return true;
}

bool Generator::visit(FunctionCallOptions const &_node)
{
    LOGI("Generator in %s", "FunctionCallOptions");
    return true;
}

bool Generator::visit(NewExpression const &_node)
{
    LOGI("Generator in %s", "NewExpression");
    return true;
}

bool Generator::visit(MemberAccess const &_node)
{
    LOGI("Generator in %s", "MemberAccess");
    return true;
}

bool Generator::visit(IndexAccess const &_node)
{
    LOGI("Generator in %s", "IndexAccess");
    return true;
}

bool Generator::visit(IndexRangeAccess const &_node)
{
    LOGI("Generator in %s", "IndexRangeAccess");
    return true;
}

bool Generator::visit(Identifier const &_node)
{
    LOGI("Generator in %s", "Identifier");
    return true;
}

bool Generator::visit(ElementaryTypeNameExpression const &_node)
{
    LOGI("Generator in %s", "ElementaryTypeNameExpression");
    return true;
}

bool Generator::visit(Literal const &_node)
{
    LOGI("Generator in %s", "Literal");
    return true;
}

bool Generator::visit(StructuredDocumentation const &_node)
{
    LOGI("Generator in %s", "StructuredDocumentation");
    return true;
}

void Generator::dump() const
{
    // places
    for (auto const &place : network_->places())
    {
        LOGI("Place: %s, type: %s", place->name().c_str(), place->color().c_str());
    }
}

_END_PSM_NM_
