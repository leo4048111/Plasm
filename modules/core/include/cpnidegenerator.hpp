#pragma once

#include "base.hpp"

#include <libsolidity/ast/AST.h>
#include <libsolidity/ast/ASTVisitor.h>

#include <memory>

#include "cpnxml.hpp"

_START_PSM_NM_

class CPNIDEGenerator: public solidity::frontend::ASTConstVisitor
{
public:
	explicit CPNIDEGenerator();

	void toCPN(solidity::frontend::ASTNode const& _node);

	bool visit(solidity::frontend::SourceUnit const& _node) override;
	bool visit(solidity::frontend::PragmaDirective const& _node) override;
	bool visit(solidity::frontend::ImportDirective const& _node) override;
	bool visit(solidity::frontend::ContractDefinition const& _node) override;
	bool visit(solidity::frontend::IdentifierPath const& _node) override;
	bool visit(solidity::frontend::InheritanceSpecifier const& _node) override;
	bool visit(solidity::frontend::UsingForDirective const& _node) override;
	bool visit(solidity::frontend::StructDefinition const& _node) override;
	bool visit(solidity::frontend::EnumDefinition const& _node) override;
	bool visit(solidity::frontend::EnumValue const& _node) override;
	bool visit(solidity::frontend::UserDefinedValueTypeDefinition const& _node) override;
	bool visit(solidity::frontend::ParameterList const& _node) override;
	bool visit(solidity::frontend::OverrideSpecifier const& _node) override;
	bool visit(solidity::frontend::FunctionDefinition const& _node) override;
	bool visit(solidity::frontend::VariableDeclaration const& _node) override;
	bool visit(solidity::frontend::ModifierDefinition const& _node) override;
	bool visit(solidity::frontend::ModifierInvocation const& _node) override;
	bool visit(solidity::frontend::EventDefinition const& _node) override;
	bool visit(solidity::frontend::ErrorDefinition const& _node) override;
	bool visit(solidity::frontend::ElementaryTypeName const& _node) override;
	bool visit(solidity::frontend::UserDefinedTypeName const& _node) override;
	bool visit(solidity::frontend::FunctionTypeName const& _node) override;
	bool visit(solidity::frontend::Mapping const& _node) override;
	bool visit(solidity::frontend::ArrayTypeName const& _node) override;
	bool visit(solidity::frontend::InlineAssembly const& _node) override;
	bool visit(solidity::frontend::Block const& _node) override;
	bool visit(solidity::frontend::PlaceholderStatement const& _node) override;
	bool visit(solidity::frontend::IfStatement const& _node) override;
	bool visit(solidity::frontend::TryCatchClause const& _node) override;
	bool visit(solidity::frontend::TryStatement const& _node) override;
	bool visit(solidity::frontend::WhileStatement const& _node) override;
	bool visit(solidity::frontend::ForStatement const& _node) override;
	bool visit(solidity::frontend::Continue const& _node) override;
	bool visit(solidity::frontend::Break const& _node) override;
	bool visit(solidity::frontend::Return const& _node) override;
	bool visit(solidity::frontend::Throw const& _node) override;
	bool visit(solidity::frontend::EmitStatement const& _node) override;
	bool visit(solidity::frontend::RevertStatement const& _node) override;
	bool visit(solidity::frontend::VariableDeclarationStatement const& _node) override;
	bool visit(solidity::frontend::ExpressionStatement const& _node) override;
	bool visit(solidity::frontend::Conditional const& _node) override;
	bool visit(solidity::frontend::Assignment const& _node) override;
	bool visit(solidity::frontend::TupleExpression const& _node) override;
	bool visit(solidity::frontend::UnaryOperation const& _node) override;
	bool visit(solidity::frontend::BinaryOperation const& _node) override;
	bool visit(solidity::frontend::FunctionCall const& _node) override;
	bool visit(solidity::frontend::FunctionCallOptions const& _node) override;
	bool visit(solidity::frontend::NewExpression const& _node) override;
	bool visit(solidity::frontend::MemberAccess const& _node) override;
	bool visit(solidity::frontend::IndexAccess const& _node) override;
	bool visit(solidity::frontend::IndexRangeAccess const& _node) override;
	bool visit(solidity::frontend::Identifier const& _node) override;
	bool visit(solidity::frontend::ElementaryTypeNameExpression const& _node) override;
	bool visit(solidity::frontend::Literal const& _node) override;
	bool visit(solidity::frontend::StructuredDocumentation const& _node) override;

	void endVisit(solidity::frontend::ContractDefinition const& _node) override;

private:
	::std::unique_ptr<CPNXml> cpnxml_{nullptr};
};

_END_PSM_NM_