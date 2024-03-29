#include "noopgenerator.hpp"

#include "logger.hpp"


_START_PSM_NM_

using namespace solidity::frontend;

NoopGenerator::NoopGenerator() {};

void NoopGenerator::toCPN(solidity::frontend::ASTNode const& _node) {
    _node.accept(*this);
}

bool NoopGenerator::visit(SourceUnit const& _node)
{
	LOGI("NoopGenerator in %s", "SourceUnit");
	return true;
}

bool NoopGenerator::visit(PragmaDirective const& _node)
{
	LOGI("NoopGenerator in %s", "PragmaDirective");
	return true;
}

bool NoopGenerator::visit(ImportDirective const& _node)
{
	LOGI("NoopGenerator in %s", "ImportDirective");
	return true;
}

bool NoopGenerator::visit(ContractDefinition const& _node)
{
	LOGI("NoopGenerator in %s", "ContractDefinition");
	return true;
}

bool NoopGenerator::visit(IdentifierPath const& _node)
{
	LOGI("NoopGenerator in %s", "IdentifierPath");
	return true;
}

bool NoopGenerator::visit(InheritanceSpecifier const& _node)
{
	LOGI("NoopGenerator in %s", "InheritanceSpecifier");
	return true;
}

bool NoopGenerator::visit(UsingForDirective const& _node)
{
	LOGI("NoopGenerator in %s", "UsingForDirective");
	return true;
}

bool NoopGenerator::visit(StructDefinition const& _node)
{
    LOGI("NoopGenerator in %s", "StructDefinition");
	return true;
}

bool NoopGenerator::visit(EnumDefinition const& _node)
{
    LOGI("NoopGenerator in %s", "EnumDefinition");
	return true;
}

bool NoopGenerator::visit(EnumValue const& _node)
{
    LOGI("NoopGenerator in %s", "EnumValue");
	return true;
}

bool NoopGenerator::visit(UserDefinedValueTypeDefinition const& _node)
{
    LOGI("NoopGenerator in %s", "UserDefinedValueTypeDefinition");
	return true;
}

bool NoopGenerator::visit(ParameterList const& _node)
{
    LOGI("NoopGenerator in %s", "ParameterList");
	return true;
}

bool NoopGenerator::visit(OverrideSpecifier const& _node)
{
    LOGI("NoopGenerator in %s", "OverrideSpecifier");
	return true;
}

bool NoopGenerator::visit(FunctionDefinition const& _node)
{
    LOGI("NoopGenerator in %s", "FunctionDefinition");
	return true;
}

bool NoopGenerator::visit(VariableDeclaration const& _node)
{
    LOGI("NoopGenerator in %s", "VariableDeclaration");
	return true;
}

bool NoopGenerator::visit(ModifierDefinition const& _node)
{
    LOGI("NoopGenerator in %s", "ModifierDefinition");
	return true;
}

bool NoopGenerator::visit(ModifierInvocation const& _node)
{
    LOGI("NoopGenerator in %s", "ModifierInvocation");
	return true;
}

bool NoopGenerator::visit(EventDefinition const& _node)
{
    LOGI("NoopGenerator in %s", "EventDefinition");
	return true;
}

bool NoopGenerator::visit(ErrorDefinition const& _node)
{
    LOGI("NoopGenerator in %s", "ErrorDefinition");
	return true;
}

bool NoopGenerator::visit(ElementaryTypeName const& _node)
{
    LOGI("NoopGenerator in %s", "ErrorDefinition");
	return true;
}

bool NoopGenerator::visit(UserDefinedTypeName const& _node)
{
    LOGI("NoopGenerator in %s", "UserDefinedTypeName");
	return true;
}

bool NoopGenerator::visit(FunctionTypeName const& _node)
{
    LOGI("NoopGenerator in %s", "FunctionTypeName");
	return true;
}

bool NoopGenerator::visit(Mapping const& _node)
{
    LOGI("NoopGenerator in %s", "Mapping");
	return true;
}

bool NoopGenerator::visit(ArrayTypeName const& _node)
{
    LOGI("NoopGenerator in %s", "ArrayTypeName");
	return true;
}

bool NoopGenerator::visit(InlineAssembly const& _node)
{
    LOGI("NoopGenerator in %s", "InlineAssembly");
	return true;
}

bool NoopGenerator::visit(Block const& _node)
{
    LOGI("NoopGenerator in %s", "Block");
	return true;
}

bool NoopGenerator::visit(PlaceholderStatement const& _node)
{
    LOGI("NoopGenerator in %s", "PlaceholderStatement");
	return true;
}

bool NoopGenerator::visit(IfStatement const& _node)
{
    LOGI("NoopGenerator in %s", "IfStatement");
	return true;
}

bool NoopGenerator::visit(TryCatchClause const& _node)
{
    LOGI("NoopGenerator in %s", "TryCatchClause");
	return true;
}

bool NoopGenerator::visit(TryStatement const& _node)
{
    LOGI("NoopGenerator in %s", "TryStatement");
	return true;
}

bool NoopGenerator::visit(WhileStatement const& _node)
{
    LOGI("NoopGenerator in %s", "WhileStatement");
	return true;
}

bool NoopGenerator::visit(ForStatement const& _node)
{
    LOGI("NoopGenerator in %s", "ForStatement");
	return true;
}

bool NoopGenerator::visit(Continue const& _node)
{
    LOGI("NoopGenerator in %s", "Continue");
	return true;
}

bool NoopGenerator::visit(Break const& _node)
{
    LOGI("NoopGenerator in %s", "Break");
	return true;
}

bool NoopGenerator::visit(Return const& _node)
{
    LOGI("NoopGenerator in %s", "Return");
	return true;
}

bool NoopGenerator::visit(Throw const& _node)
{
    LOGI("NoopGenerator in %s", "Throw");
	return true;
}

bool NoopGenerator::visit(EmitStatement const& _node)
{
    LOGI("NoopGenerator in %s", "EmitStatement");
	return true;
}

bool NoopGenerator::visit(RevertStatement const& _node)
{
    LOGI("NoopGenerator in %s", "RevertStatement");
	return true;
}

bool NoopGenerator::visit(VariableDeclarationStatement const& _node)
{
    LOGI("NoopGenerator in %s", "VariableDeclarationStatement");
	return true;
}

bool NoopGenerator::visit(ExpressionStatement const& _node)
{
    LOGI("NoopGenerator in %s", "ExpressionStatement");
	return true;
}

bool NoopGenerator::visit(Conditional const& _node)
{
    LOGI("NoopGenerator in %s", "Conditional");
	return true;
}

bool NoopGenerator::visit(Assignment const& _node)
{
    LOGI("NoopGenerator in %s", "Assignment");
	return true;
}

bool NoopGenerator::visit(TupleExpression const& _node)
{
    LOGI("NoopGenerator in %s", "TupleExpression");
	return true;
}

bool NoopGenerator::visit(UnaryOperation const& _node)
{
    LOGI("NoopGenerator in %s", "UnaryOperation");
	return true;
}

bool NoopGenerator::visit(BinaryOperation const& _node)
{
    LOGI("NoopGenerator in %s", "BinaryOperation");
	return true;
}

bool NoopGenerator::visit(FunctionCall const& _node)
{
    LOGI("NoopGenerator in %s", "FunctionCall");
	return true;
}

bool NoopGenerator::visit(FunctionCallOptions const& _node)
{
    LOGI("NoopGenerator in %s", "FunctionCallOptions");
	return true;
}

bool NoopGenerator::visit(NewExpression const& _node)
{
    LOGI("NoopGenerator in %s", "NewExpression");
	return true;
}

bool NoopGenerator::visit(MemberAccess const& _node)
{
    LOGI("NoopGenerator in %s", "MemberAccess");
	return true;
}

bool NoopGenerator::visit(IndexAccess const& _node)
{
    LOGI("NoopGenerator in %s", "IndexAccess");
	return true;
}

bool NoopGenerator::visit(IndexRangeAccess const& _node)
{
    LOGI("NoopGenerator in %s", "IndexRangeAccess");
	return true;
}

bool NoopGenerator::visit(Identifier const& _node)
{
    LOGI("NoopGenerator in %s", "Identifier");
	return true;
}

bool NoopGenerator::visit(ElementaryTypeNameExpression const& _node)
{
    LOGI("NoopGenerator in %s", "ElementaryTypeNameExpression");
	return true;
}

bool NoopGenerator::visit(Literal const& _node)
{
    LOGI("NoopGenerator in %s", "Literal");
	return true;
}

bool NoopGenerator::visit(StructuredDocumentation const& _node)
{
    LOGI("NoopGenerator in %s", "StructuredDocumentation");
	return true;
}


_END_PSM_NM_
