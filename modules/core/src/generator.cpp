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
	return true;
}

bool Generator::visit(PragmaDirective const& _node)
{
	LOGI("In %s", "PragmaDirective");
	return true;
}

bool Generator::visit(ImportDirective const& _node)
{
	LOGI("In %s", "ImportDirective");
	return true;
}

bool Generator::visit(ContractDefinition const& _node)
{
	LOGI("In %s", "ContractDefinition");
	return true;
}

bool Generator::visit(IdentifierPath const& _node)
{
	LOGI("In %s", "IdentifierPath");
	return true;
}

bool Generator::visit(InheritanceSpecifier const& _node)
{
	LOGI("In %s", "InheritanceSpecifier");
	return true;
}

bool Generator::visit(UsingForDirective const& _node)
{
	LOGI("In %s", "UsingForDirective");
	return true;
}

bool Generator::visit(StructDefinition const& _node)
{
    LOGI("In %s", "StructDefinition");
	return true;
}

bool Generator::visit(EnumDefinition const& _node)
{
    LOGI("In %s", "EnumDefinition");
	return true;
}

bool Generator::visit(EnumValue const& _node)
{
    LOGI("In %s", "EnumValue");
	return true;
}

bool Generator::visit(UserDefinedValueTypeDefinition const& _node)
{
    LOGI("In %s", "UserDefinedValueTypeDefinition");
	return true;
}

bool Generator::visit(ParameterList const& _node)
{
    LOGI("In %s", "ParameterList");
	return true;
}

bool Generator::visit(OverrideSpecifier const& _node)
{
    LOGI("In %s", "OverrideSpecifier");
	return true;
}

bool Generator::visit(FunctionDefinition const& _node)
{
    LOGI("In %s", "Generator");
	return true;
}

bool Generator::visit(VariableDeclaration const& _node)
{
    LOGI("In %s", "VariableDeclaration");
	return true;
}

bool Generator::visit(ModifierDefinition const& _node)
{
    LOGI("In %s", "ModifierDefinition");
	return true;
}

bool Generator::visit(ModifierInvocation const& _node)
{
    LOGI("In %s", "ModifierInvocation");
	return true;
}

bool Generator::visit(EventDefinition const& _node)
{
    LOGI("In %s", "EventDefinition");
	return true;
}

bool Generator::visit(ErrorDefinition const& _node)
{
    LOGI("In %s", "ErrorDefinition");
	return true;
}

bool Generator::visit(ElementaryTypeName const& _node)
{
    LOGI("In %s", "ErrorDefinition");
	return true;
}

bool Generator::visit(UserDefinedTypeName const& _node)
{
    LOGI("In %s", "UserDefinedTypeName");
	return true;
}

bool Generator::visit(FunctionTypeName const& _node)
{
    LOGI("In %s", "FunctionTypeName");
	return true;
}

bool Generator::visit(Mapping const& _node)
{
    LOGI("In %s", "Mapping");
	return true;
}

bool Generator::visit(ArrayTypeName const& _node)
{
    LOGI("In %s", "ArrayTypeName");
	return true;
}

bool Generator::visit(InlineAssembly const& _node)
{
    LOGI("In %s", "InlineAssembly");
	return true;
}

bool Generator::visit(Block const& _node)
{
    LOGI("In %s", "Block");
	return true;
}

bool Generator::visit(PlaceholderStatement const& _node)
{
    LOGI("In %s", "PlaceholderStatement");
	return true;
}

bool Generator::visit(IfStatement const& _node)
{
    LOGI("In %s", "IfStatement");
	return true;
}

bool Generator::visit(TryCatchClause const& _node)
{
    LOGI("In %s", "TryCatchClause");
	return true;
}

bool Generator::visit(TryStatement const& _node)
{
    LOGI("In %s", "TryStatement");
	return true;
}

bool Generator::visit(WhileStatement const& _node)
{
    LOGI("In %s", "WhileStatement");
	return true;
}

bool Generator::visit(ForStatement const& _node)
{
    LOGI("In %s", "ForStatement");
	return true;
}

bool Generator::visit(Continue const& _node)
{
    LOGI("In %s", "Continue");
	return true;
}

bool Generator::visit(Break const& _node)
{
    LOGI("In %s", "Break");
	return true;
}

bool Generator::visit(Return const& _node)
{
    LOGI("In %s", "Return");
	return true;
}

bool Generator::visit(Throw const& _node)
{
    LOGI("In %s", "Throw");
	return true;
}

bool Generator::visit(EmitStatement const& _node)
{
    LOGI("In %s", "EmitStatement");
	return true;
}

bool Generator::visit(RevertStatement const& _node)
{
    LOGI("In %s", "RevertStatement");
	return true;
}

bool Generator::visit(VariableDeclarationStatement const& _node)
{
    LOGI("In %s", "VariableDeclarationStatement");
	return true;
}

bool Generator::visit(ExpressionStatement const& _node)
{
    LOGI("In %s", "ExpressionStatement");
	return true;
}

bool Generator::visit(Conditional const& _node)
{
    LOGI("In %s", "Conditional");
	return true;
}

bool Generator::visit(Assignment const& _node)
{
    LOGI("In %s", "Assignment");
	return true;
}

bool Generator::visit(TupleExpression const& _node)
{
    LOGI("In %s", "TupleExpression");
	return true;
}

bool Generator::visit(UnaryOperation const& _node)
{
    LOGI("In %s", "UnaryOperation");
	return true;
}

bool Generator::visit(BinaryOperation const& _node)
{
    LOGI("In %s", "BinaryOperation");
	return true;
}

bool Generator::visit(FunctionCall const& _node)
{
    LOGI("In %s", "FunctionCall");
	return true;
}

bool Generator::visit(FunctionCallOptions const& _node)
{
    LOGI("In %s", "FunctionCallOptions");
	return true;
}

bool Generator::visit(NewExpression const& _node)
{
    LOGI("In %s", "NewExpression");
	return true;
}

bool Generator::visit(MemberAccess const& _node)
{
    LOGI("In %s", "MemberAccess");
	return true;
}

bool Generator::visit(IndexAccess const& _node)
{
    LOGI("In %s", "IndexAccess");
	return true;
}

bool Generator::visit(IndexRangeAccess const& _node)
{
    LOGI("In %s", "IndexRangeAccess");
	return true;
}

bool Generator::visit(Identifier const& _node)
{
    LOGI("In %s", "Identifier");
	return true;
}

bool Generator::visit(ElementaryTypeNameExpression const& _node)
{
    LOGI("In %s", "ElementaryTypeNameExpression");
	return true;
}

bool Generator::visit(Literal const& _node)
{
    LOGI("In %s", "Literal");
	return true;
}

bool Generator::visit(StructuredDocumentation const& _node)
{
    LOGI("In %s", "StructuredDocumentation");
	return true;
}

void Generator::endVisit(EventDefinition const&)
{
}

_END_PSM_NM_
