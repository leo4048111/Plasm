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
    // Do not visit VariableDeclaration since we have visited it in FunctionDefinition
    return false;
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

    auto name = _node.name();
    auto type = _node.typeName().annotation().type->toString();

    // create places
    ::std::shared_ptr<cpn::Place> place = ::std::make_shared<cpn::Place>(scope() + name, type);
    network_->addPlace(place);

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

void Generator::endVisit(Block const &_node)
{
    // create inout control places
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // connect statement io places
    PSM_ASSERT(_node.statements().size());

    // connect first statementinplace with block inplace
    int cnt = 0;
    auto firstStmtInPlace = network_->getPlaceByName(::std::to_string(_node.statements().front()->id()) + ".in");
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con" + ::std::to_string(cnt++));
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(firstStmtInPlace, con0, cpn::Arc::Orientation::T2P);
    network_->addTransition(con0);
    network_->addArc(arc1);
    network_->addArc(arc2);

    // connect last statementoutplace with block outplace
    auto lastStmtOutPlace = network_->getPlaceByName(::std::to_string(_node.statements().back()->id()) + ".out");
    ::std::shared_ptr<cpn::Transition> con1 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con" + ::std::to_string(_node.statements().size() - 1));
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(lastStmtOutPlace, con1, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(outPlace, con1, cpn::Arc::Orientation::T2P);
    network_->addTransition(con1);
    network_->addArc(arc3);
    network_->addArc(arc4);

    // connect other statement io places
    while(cnt < _node.statements().size()) {
        auto lastStmt = _node.statements().at(cnt - 1);
        auto curStmt = _node.statements().at(cnt);
        ::std::shared_ptr<cpn::Place> lastOutPlace = network_->getPlaceByName(::std::to_string(lastStmt->id()) + ".out");
        ::std::shared_ptr<cpn::Place> curInPlace = network_->getPlaceByName(::std::to_string(curStmt->id()) + ".in");
        ::std::shared_ptr<cpn::Transition> con = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con" + ::std::to_string(cnt++));
        network_->addTransition(con);

        // connect last stmt outplace with cur stmt inplace
        ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(lastOutPlace, con, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(curInPlace, con, cpn::Arc::Orientation::T2P);
    }
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

void Generator::endVisit(IfStatement const &_node)
{
    // create inout control places
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create ifstatement transitions
    ::std::shared_ptr<cpn::Transition> ifstmtTransition = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()));
    network_->addTransition(ifstmtTransition);
    // create endif transition for true body
    ::std::shared_ptr<cpn::Transition> endifTrueTransition = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".endif.true");
    network_->addTransition(endifTrueTransition);

    // get condition place
    auto conditionPlace = network_->getPlaceByName(::std::to_string(_node.condition().id()) + ".result");

    // get if body io place
    auto ifBodyInPlace = network_->getPlaceByName(::std::to_string(_node.trueStatement().id()) + ".in");
    auto ifBodyOutPlace = network_->getPlaceByName(::std::to_string(_node.trueStatement().id()) + ".out");

    // get if false body io place(if has false body)
    if (_node.falseStatement())
    {
        auto ifFalseBodyInPlace = network_->getPlaceByName(::std::to_string(_node.falseStatement()->id()) + ".in");
        auto ifFalseBodyOutPlace = network_->getPlaceByName(::std::to_string(_node.falseStatement()->id()) + ".out");
    }

    // create arcs
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, ifstmtTransition, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(outPlace, ifstmtTransition, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(ifBodyInPlace, ifstmtTransition, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(ifBodyOutPlace, endifTrueTransition, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(outPlace, endifTrueTransition, cpn::Arc::Orientation::T2P);
    network_->addArc(arc1);
    network_->addArc(arc2);
    network_->addArc(arc3);
    network_->addArc(arc4);
    network_->addArc(arc5);

    // create arcs for false body
    if (_node.falseStatement())
    {
        ::std::shared_ptr<cpn::Transition> endifFalseTransition = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".endif.false");
        network_->addTransition(endifFalseTransition);
        ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(ifBodyInPlace, ifstmtTransition, cpn::Arc::Orientation::T2P);
        ::std::shared_ptr<cpn::Arc> arc7 = ::std::make_shared<cpn::Arc>(ifBodyOutPlace, endifFalseTransition, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc8 = ::std::make_shared<cpn::Arc>(outPlace, endifFalseTransition, cpn::Arc::Orientation::T2P);
        network_->addArc(arc6);
        network_->addArc(arc7);
        network_->addArc(arc8);
    }
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

void Generator::endVisit(WhileStatement const &_node)
{
    // create inout control places
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create whilestatement transitions
    ::std::shared_ptr<cpn::Transition> whileStmtTransition = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()));
    network_->addTransition(whileStmtTransition);

    // get condition io place and result place
    auto conditionResultPlace = network_->getPlaceByName(::std::to_string(_node.condition().id()) + ".result");
    auto conditionInPlace = network_->getPlaceByName(::std::to_string(_node.id()) + ".in");
    auto conditionOutPlace = network_->getPlaceByName(::std::to_string(_node.id()) + ".out");

    // create while statement loop transition
    ::std::shared_ptr<cpn::Transition> loopTransition = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".loop");
    network_->addTransition(loopTransition);

    // get while loop body
    auto whileBodyInPlace = network_->getPlaceByName(::std::to_string(_node.body().id()) + ".in");
    auto whileBodyOutPlace = network_->getPlaceByName(::std::to_string(_node.body().id()) + ".out");

    // create again transition
    ::std::shared_ptr<cpn::Transition> againTransition = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".again");
    network_->addTransition(againTransition);

    // create arcs
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, whileStmtTransition, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(conditionInPlace, whileStmtTransition, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(conditionResultPlace, loopTransition, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(conditionOutPlace, loopTransition, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(outPlace, loopTransition, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(whileBodyInPlace, loopTransition, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc7 = ::std::make_shared<cpn::Arc>(whileBodyOutPlace, againTransition, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc8 = ::std::make_shared<cpn::Arc>(conditionInPlace, againTransition, cpn::Arc::Orientation::T2P);
    network_->addArc(arc1);
    network_->addArc(arc2);
    network_->addArc(arc3);
    network_->addArc(arc4);
    network_->addArc(arc5);
    network_->addArc(arc6);
    network_->addArc(arc7);
    network_->addArc(arc8);
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

void Generator::endVisit(Assignment const &_node)
{
    // create inout control places
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create assignment transition
    ::std::shared_ptr<cpn::Transition> transition = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()));
    network_->addTransition(transition);

    // get lhs and rhs places
    auto lhsPlace = network_->getPlaceByName(::std::to_string(_node.leftHandSide().id()) + ".result");
    auto rhsPlace = network_->getPlaceByName(::std::to_string(_node.rightHandSide().id()) + ".result");

    // create alias for result place
    network_->alias(lhsPlace, ::std::to_string(_node.id()) + ".result");

    // create arcs
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, transition, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(outPlace, transition, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(lhsPlace, transition, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(lhsPlace, transition, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(rhsPlace, transition, cpn::Arc::Orientation::BD);
    network_->addArc(arc1);
    network_->addArc(arc2);
    network_->addArc(arc3);
    network_->addArc(arc4);
    network_->addArc(arc5);
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

void Generator::endVisit(BinaryOperation const &_node)
{
    // create inout control places
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create binary op transition
    ::std::shared_ptr<cpn::Transition> transition = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()));
    network_->addTransition(transition);

    // create place for result
    ::std::shared_ptr<cpn::Place> resultPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".result", "unknown");
    network_->addPlace(resultPlace);

    // get lhs and rhs places
    auto lhsPlace = network_->getPlaceByName(::std::to_string(_node.leftExpression().id()) + ".result");
    auto rhsPlace = network_->getPlaceByName(::std::to_string(_node.rightExpression().id()) + ".result");

    // create arcs
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, transition, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(outPlace, transition, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(lhsPlace, transition, cpn::Arc::Orientation::BD);
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(rhsPlace, transition, cpn::Arc::Orientation::BD);
    ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(resultPlace, transition, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(resultPlace, transition, cpn::Arc::Orientation::P2T);
    network_->addArc(arc1);
    network_->addArc(arc2);
    network_->addArc(arc3);
    network_->addArc(arc4);
    network_->addArc(arc5);
    network_->addArc(arc6);
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

    // search for local var
    auto place = network_->getPlaceByName(scope() + _node.name());

    // search for param
    if (place == nullptr)
    {
        place = network_->getPlaceByName(scope() + SCOPE_PARAM + _node.name());
    }

    // search for ret
    if (place == nullptr)
    {
        place = network_->getPlaceByName(scope() + SCOPE_RET + _node.name());
    }

    // search for global var
    if (place == nullptr)
    {
        place = network_->getPlaceByName(SCOPE_GLOB + _node.name());
    }

    // not found
    if (place == nullptr)
    {
        LOGE("Identifier %s not found", _node.name().c_str());
        return false;
    }

    network_->alias(place, ::std::to_string(_node.id()) + ".result");

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

    // transitions
    for (auto const &transition : network_->transitions())
    {
        LOGI("Transition: %s", transition->name().c_str());
    }

    // arcs
    for (auto const &arc : network_->arcs())
    {
        LOGI("Arc trans: %s, place: %s", arc->place()->name().c_str(), arc->transition()->name().c_str());
    }
}

_END_PSM_NM_
