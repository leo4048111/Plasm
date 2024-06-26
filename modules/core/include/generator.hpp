#pragma once

#include "base.hpp"

#include <libsolidity/ast/AST.h>
#include <libsolidity/ast/ASTVisitor.h>
#include <stack>

#include "cpn.hpp"

_START_PSM_NM_

class Generator : public solidity::frontend::ASTConstVisitor
{
public:
	explicit Generator();

	void toCPN(solidity::frontend::ASTNode const &_node);

	void dump() const;

	::std::shared_ptr<cpn::Network> getNetwork() { return network_; };

	::std::vector<::std::string> getFunctions() const { return functions_; };

	bool visit(solidity::frontend::SourceUnit const &_node) override;
	bool visit(solidity::frontend::PragmaDirective const &_node) override;
	bool visit(solidity::frontend::ImportDirective const &_node) override;
	bool visit(solidity::frontend::ContractDefinition const &_node) override;
	bool visit(solidity::frontend::IdentifierPath const &_node) override;
	bool visit(solidity::frontend::InheritanceSpecifier const &_node) override;
	bool visit(solidity::frontend::UsingForDirective const &_node) override;
	bool visit(solidity::frontend::StructDefinition const &_node) override;
	bool visit(solidity::frontend::EnumDefinition const &_node) override;
	bool visit(solidity::frontend::EnumValue const &_node) override;
	bool visit(solidity::frontend::UserDefinedValueTypeDefinition const &_node) override;
	bool visit(solidity::frontend::ParameterList const &_node) override;
	bool visit(solidity::frontend::OverrideSpecifier const &_node) override;
	bool visit(solidity::frontend::FunctionDefinition const &_node) override;
	bool visit(solidity::frontend::VariableDeclaration const &_node) override;
	bool visit(solidity::frontend::ModifierDefinition const &_node) override;
	bool visit(solidity::frontend::ModifierInvocation const &_node) override;
	bool visit(solidity::frontend::EventDefinition const &_node) override;
	bool visit(solidity::frontend::ErrorDefinition const &_node) override;
	bool visit(solidity::frontend::ElementaryTypeName const &_node) override;
	bool visit(solidity::frontend::UserDefinedTypeName const &_node) override;
	bool visit(solidity::frontend::FunctionTypeName const &_node) override;
	bool visit(solidity::frontend::Mapping const &_node) override;
	bool visit(solidity::frontend::ArrayTypeName const &_node) override;
	bool visit(solidity::frontend::InlineAssembly const &_node) override;
	bool visit(solidity::frontend::Block const &_node) override;
	bool visit(solidity::frontend::PlaceholderStatement const &_node) override;
	bool visit(solidity::frontend::IfStatement const &_node) override;
	bool visit(solidity::frontend::TryCatchClause const &_node) override;
	bool visit(solidity::frontend::TryStatement const &_node) override;
	bool visit(solidity::frontend::WhileStatement const &_node) override;
	bool visit(solidity::frontend::ForStatement const &_node) override;
	bool visit(solidity::frontend::Continue const &_node) override;
	bool visit(solidity::frontend::Break const &_node) override;
	bool visit(solidity::frontend::Return const &_node) override;
	bool visit(solidity::frontend::Throw const &_node) override;
	bool visit(solidity::frontend::EmitStatement const &_node) override;
	bool visit(solidity::frontend::RevertStatement const &_node) override;
	bool visit(solidity::frontend::VariableDeclarationStatement const &_node) override;
	bool visit(solidity::frontend::ExpressionStatement const &_node) override;
	bool visit(solidity::frontend::Conditional const &_node) override;
	bool visit(solidity::frontend::Assignment const &_node) override;
	bool visit(solidity::frontend::TupleExpression const &_node) override;
	bool visit(solidity::frontend::UnaryOperation const &_node) override;
	bool visit(solidity::frontend::BinaryOperation const &_node) override;
	bool visit(solidity::frontend::FunctionCall const &_node) override;
	bool visit(solidity::frontend::FunctionCallOptions const &_node) override;
	bool visit(solidity::frontend::NewExpression const &_node) override;
	bool visit(solidity::frontend::MemberAccess const &_node) override;
	bool visit(solidity::frontend::IndexAccess const &_node) override;
	bool visit(solidity::frontend::IndexRangeAccess const &_node) override;
	bool visit(solidity::frontend::Identifier const &_node) override;
	bool visit(solidity::frontend::ElementaryTypeNameExpression const &_node) override;
	bool visit(solidity::frontend::Literal const &_node) override;
	bool visit(solidity::frontend::StructuredDocumentation const &_node) override;

	void endVisit(solidity::frontend::SourceUnit const &_node) override;
	void endVisit(solidity::frontend::ContractDefinition const &_node) override;
	void endVisit(solidity::frontend::FunctionDefinition const &_node) override;
	void endVisit(solidity::frontend::BinaryOperation const &_node) override;
	void endVisit(solidity::frontend::Assignment const &_node) override;
	void endVisit(solidity::frontend::IfStatement const &_node) override;
	void endVisit(solidity::frontend::WhileStatement const &_node) override;
	void endVisit(solidity::frontend::Block const &_node) override;
	void endVisit(solidity::frontend::FunctionCall const &_node) override;
	void endVisit(solidity::frontend::VariableDeclarationStatement const &_node) override;
	void endVisit(solidity::frontend::ExpressionStatement const &_node) override;
	void endVisit(solidity::frontend::Return const &_node) override;
	void endVisit(solidity::frontend::Literal const &_node) override;
	void endVisit(solidity::frontend::ParameterList const &_node) override;
	void endVisit(solidity::frontend::VariableDeclaration const &_node) override;
	void endVisit(solidity::frontend::MemberAccess const &_node) override;
	void endVisit(solidity::frontend::RevertStatement const &_node) override;
	void endVisit(solidity::frontend::ErrorDefinition const &_node) override;
	void endVisit(solidity::frontend::ElementaryTypeNameExpression const &_node) override;
	void endVisit(solidity::frontend::Identifier const &_node) override;
	void endVisit(solidity::frontend::IndexAccess const &_node) override;
	void endVisit(solidity::frontend::TupleExpression const &_node) override;

private:
	::std::string scope() const { return sscope_.top(); };
	void pushScope(::std::string scope) { sscope_.push(scope); };
	void popScope()
	{
		if (!sscope_.empty())
			sscope_.pop();
	};
	void declareRequire();
	void declareTransfer();
	void declareMsg();

	::std::string getFullNodeType(int64_t id) const{ return nodeTypes_.at(id) + "." + ::std::to_string(id); };

private:
	static constexpr const char *SCOPE_GLOB = "global.";
	static constexpr const char *SCOPE_PARAM = "param.";
	static constexpr const char *SCOPE_RET = "ret.";
	static constexpr const char *VAR_MSG = "msg";
	static constexpr const char *VAR_THIS = "this";
	static constexpr const char *TYPE_STRUCT = "struct";
	::std::shared_ptr<cpn::Network> network_;
	::std::stack<::std::string> sscope_;
	::std::map<::std::string, ::std::vector<::std::string>> functionParams_;
	::std::map<int64_t, ::std::string> nodeTypes_;
	::std::vector<::std::string> functions_;
};

_END_PSM_NM_