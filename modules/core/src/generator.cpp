#include "generator.hpp"

#include "logger.hpp"


_START_PSM_NM_

using namespace solidity::frontend;

Generator::Generator() {};

void Generator::toCPN(solidity::frontend::ASTNode const& _node) {
    _node.accept(*this);
}

bool Generator::visit(SourceUnit const& _node)
{
	LOGI("In %s", "SourceUnit");
	return false;
}

bool Generator::visit(PragmaDirective const& _node)
{
	LOGI("In %s", "PragmaDirective");
	return false;
}

bool Generator::visit(ImportDirective const& _node)
{
	LOGI("In %s", "ImportDirective");
	return false;
}

bool Generator::visit(ContractDefinition const& _node)
{
	LOGI("In %s", "ContractDefinition");
	return false;
}

bool Generator::visit(IdentifierPath const& _node)
{
	LOGI("In %s", "IdentifierPath");
	return false;
}

bool Generator::visit(InheritanceSpecifier const& _node)
{
	LOGI("In %s", "InheritanceSpecifier");
	return false;
}

bool Generator::visit(UsingForDirective const& _node)
{
	LOGI("In %s", "UsingForDirective");
	return false;
}

bool Generator::visit(StructDefinition const& _node)
{
    LOGI("In %s", "StructDefinition");
	return false;
}

bool Generator::visit(EnumDefinition const& _node)
{
    LOGI("In %s", "EnumDefinition");
	return false;
}

bool Generator::visit(EnumValue const& _node)
{
    LOGI("In %s", "EnumValue");
	return false;
}

bool Generator::visit(UserDefinedValueTypeDefinition const& _node)
{
    LOGI("In %s", "UserDefinedValueTypeDefinition");
	return false;
}

bool Generator::visit(ParameterList const& _node)
{
    LOGI("In %s", "ParameterList");
	return false;
}

bool Generator::visit(OverrideSpecifier const& _node)
{
    LOGI("In %s", "OverrideSpecifier");
	return false;
}

bool Generator::visit(FunctionDefinition const& _node)
{
    LOGI("In %s", "Generator");
	return false;
}

bool Generator::visit(VariableDeclaration const& _node)
{
    LOGI("In %s", "VariableDeclaration");
	return false;
}

bool Generator::visit(ModifierDefinition const& _node)
{
    LOGI("In %s", "ModifierDefinition");
	return false;
}

bool Generator::visit(ModifierInvocation const& _node)
{
    LOGI("In %s", "ModifierInvocation");
	return false;
}

bool Generator::visit(EventDefinition const& _node)
{
    LOGI("In %s", "EventDefinition");
	return false;
}

bool Generator::visit(ErrorDefinition const& _node)
{
    LOGI("In %s", "ErrorDefinition");
	return false;
}

bool Generator::visit(ElementaryTypeName const& _node)
{
    LOGI("In %s", "ErrorDefinition");
	return false;
}

bool Generator::visit(UserDefinedTypeName const& _node)
{
    LOGI("In %s", "UserDefinedTypeName");
	return false;
}

bool Generator::visit(FunctionTypeName const& _node)
{
    LOGI("In %s", "FunctionTypeName");
	return false;
}

bool Generator::visit(Mapping const& _node)
{
    LOGI("In %s", "Mapping");
	return false;
}

bool Generator::visit(ArrayTypeName const& _node)
{
    LOGI("In %s", "ArrayTypeName");
	return false;
}

bool Generator::visit(InlineAssembly const& _node)
{
    LOGI("In %s", "InlineAssembly");
	return false;
}

bool Generator::visit(Block const& _node)
{
    LOGI("In %s", "Block");
	return false;
}

bool Generator::visit(PlaceholderStatement const& _node)
{
    LOGI("In %s", "PlaceholderStatement");
	return false;
}

bool Generator::visit(IfStatement const& _node)
{
    LOGI("In %s", "IfStatement");
	return false;
}

bool Generator::visit(TryCatchClause const& _node)
{
    LOGI("In %s", "TryCatchClause");
	return false;
}

bool Generator::visit(TryStatement const& _node)
{
    LOGI("In %s", "TryStatement");
	return false;
}

bool Generator::visit(WhileStatement const& _node)
{
    LOGI("In %s", "WhileStatement");
	return false;
}

bool Generator::visit(ForStatement const& _node)
{
    LOGI("In %s", "ForStatement");
	return false;
}

bool Generator::visit(Continue const& _node)
{
    LOGI("In %s", "Continue");
	return false;
}

bool Generator::visit(Break const& _node)
{
    LOGI("In %s", "Break");
	return false;
}

bool Generator::visit(Return const& _node)
{
    LOGI("In %s", "Return");
	return false;
}

bool Generator::visit(Throw const& _node)
{
    LOGI("In %s", "Throw");
	return false;
}

bool Generator::visit(EmitStatement const& _node)
{
    LOGI("In %s", "EmitStatement");
	return false;
}

bool Generator::visit(RevertStatement const& _node)
{
    LOGI("In %s", "RevertStatement");
	return false;
}

bool Generator::visit(VariableDeclarationStatement const& _node)
{
    LOGI("In %s", "VariableDeclarationStatement");
	return false;
}

bool Generator::visit(ExpressionStatement const& _node)
{
    LOGI("In %s", "ExpressionStatement");
	return false;
}

bool Generator::visit(Conditional const& _node)
{
    LOGI("In %s", "Conditional");
	return false;
}

bool Generator::visit(Assignment const& _node)
{
    LOGI("In %s", "Assignment");
	return false;
}

bool Generator::visit(TupleExpression const& _node)
{
    LOGI("In %s", "TupleExpression");
	return false;
}

bool Generator::visit(UnaryOperation const& _node)
{
    LOGI("In %s", "UnaryOperation");
	return false;
}

bool Generator::visit(BinaryOperation const& _node)
{
    LOGI("In %s", "BinaryOperation");
	return false;
}

bool Generator::visit(FunctionCall const& _node)
{
    LOGI("In %s", "FunctionCall");
	return false;
}

bool Generator::visit(FunctionCallOptions const& _node)
{
    LOGI("In %s", "FunctionCallOptions");
	return false;
}

bool Generator::visit(NewExpression const& _node)
{
    LOGI("In %s", "NewExpression");
	return false;
}

bool Generator::visit(MemberAccess const& _node)
{
    LOGI("In %s", "MemberAccess");
	return false;
}

bool Generator::visit(IndexAccess const& _node)
{
    LOGI("In %s", "IndexAccess");
	return false;
}

bool Generator::visit(IndexRangeAccess const& _node)
{
    LOGI("In %s", "IndexRangeAccess");
	return false;
}

bool Generator::visit(Identifier const& _node)
{
    LOGI("In %s", "Identifier");
	return false;
}

bool Generator::visit(ElementaryTypeNameExpression const& _node)
{
    LOGI("In %s", "ElementaryTypeNameExpression");
	return false;
}

bool Generator::visit(Literal const& _node)
{
    LOGI("In %s", "Literal");
	return false;
}

bool Generator::visit(StructuredDocumentation const& _node)
{
    LOGI("In %s", "StructuredDocumentation");
	return false;
}

void Generator::endVisit(EventDefinition const&)
{
}

_END_PSM_NM_
