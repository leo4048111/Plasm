#include "generator.hpp"

#include "logger.hpp"
#include "cpn.hpp"

_START_PSM_NM_

using namespace solidity::frontend;

Generator::Generator()
{
    network_ = ::std::make_shared<cpn::Network>();
};

void Generator::toCPN(solidity::frontend::ASTNode const &_node)
{
    _node.accept(*this);
}

void Generator::declareRequire()
{
    // create inout control places
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>("require.in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>("require.out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create deadend
    ::std::shared_ptr<cpn::Place> deadend = ::std::make_shared<cpn::Place>("require.deadend", cpn::CTRL_COLOR);
    network_->addPlace(deadend);

    // create function call transition and call ret transition
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>("require.con0");
    network_->addTransition(con0);

    // register function parameters
    ::std::vector<::std::string> paramNames = {"condition", "message"};
    functionParams_.insert(::std::make_pair("require", paramNames));

    // create function parameter places
    ::std::shared_ptr<cpn::Place> conditionPlace = ::std::make_shared<cpn::Place>(::std::string("require.") + SCOPE_PARAM + "condition", "bool");
    ::std::shared_ptr<cpn::Place> messagePlace = ::std::make_shared<cpn::Place>(::std::string("require.") + SCOPE_PARAM + "message", "string");
    network_->addPlace(conditionPlace);
    network_->addPlace(messagePlace);

    // create arcs
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(outPlace, con0, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(deadend, con0, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(conditionPlace, con0, cpn::Arc::Orientation::BD);
    network_->addArc(arc1);
    network_->addArc(arc2);
    network_->addArc(arc3);
    network_->addArc(arc4);
}

void Generator::declareTransfer()
{
}

void Generator::declareMsg()
{
    ::std::shared_ptr<cpn::Place> msgPlace = ::std::make_shared<cpn::Place>(::std::string(SCOPE_GLOB) + "msg", "t_magic_message");
    network_->addPlace(msgPlace);
}

bool Generator::visit(SourceUnit const &_node)
{
    LOGT("Generator in %s", "SourceUnit");
    pushScope(SCOPE_GLOB);
    nodeTypes_.insert(::std::make_pair(_node.id(), "SourceUnit"));
    return true;
}

void Generator::endVisit(SourceUnit const &_node)
{
    popScope();
}

bool Generator::visit(PragmaDirective const &_node)
{
    LOGT("Generator in %s", "PragmaDirective");
    nodeTypes_.insert(::std::make_pair(_node.id(), "PragmaDirective"));
    return true;
}

bool Generator::visit(ImportDirective const &_node)
{
    LOGT("Generator in %s", "ImportDirective");
    nodeTypes_.insert(::std::make_pair(_node.id(), "ImportDirective"));
    return true;
}

// parse topnet decls
bool Generator::visit(ContractDefinition const &_node)
{
    LOGT("Generator in %s", "ContractDefinition");
    pushScope(SCOPE_GLOB);

    // register global msg
    ::std::shared_ptr<cpn::Place> msgPlace = ::std::make_shared<cpn::Place>(scope() + VAR_MSG, TYPE_STRUCT);
    network_->addPlace(msgPlace);

    // register global this
    ::std::shared_ptr<cpn::Place> thisPlace = ::std::make_shared<cpn::Place>(scope() + VAR_THIS, TYPE_STRUCT);
    network_->addPlace(thisPlace);
    nodeTypes_.insert(::std::make_pair(_node.id(), "ContractDefinition"));

    // declare global function require(condition, msg)
    declareRequire();

    // declare global variable msg
    declareMsg();
    return true;
}

void Generator::endVisit(ContractDefinition const &_node)
{
    popScope();
}

bool Generator::visit(IdentifierPath const &_node)
{
    LOGT("Generator in %s", "IdentifierPath");
    nodeTypes_.insert(::std::make_pair(_node.id(), "IdentifierPath"));
    return true;
}

bool Generator::visit(InheritanceSpecifier const &_node)
{
    LOGT("Generator in %s", "InheritanceSpecifier");
    nodeTypes_.insert(::std::make_pair(_node.id(), "InheritanceSpecifier"));
    return true;
}

bool Generator::visit(UsingForDirective const &_node)
{
    LOGT("Generator in %s", "UsingForDirective");
    nodeTypes_.insert(::std::make_pair(_node.id(), "UsingForDirective"));
    return true;
}

bool Generator::visit(StructDefinition const &_node)
{
    LOGT("Generator in %s", "StructDefinition");
    nodeTypes_.insert(::std::make_pair(_node.id(), "StructDefinition"));
    return true;
}

bool Generator::visit(EnumDefinition const &_node)
{
    LOGT("Generator in %s", "EnumDefinition");
    nodeTypes_.insert(::std::make_pair(_node.id(), "EnumDefinition"));
    return true;
}

bool Generator::visit(EnumValue const &_node)
{
    LOGT("Generator in %s", "EnumValue");
    nodeTypes_.insert(::std::make_pair(_node.id(), "EnumValue"));
    return true;
}

bool Generator::visit(UserDefinedValueTypeDefinition const &_node)
{
    LOGT("Generator in %s", "UserDefinedValueTypeDefinition");
    nodeTypes_.insert(::std::make_pair(_node.id(), "UserDefinedValueTypeDefinition"));
    return true;
}

bool Generator::visit(ParameterList const &_node)
{
    LOGT("Generator in %s", "ParameterList");
    pushScope(scope() + SCOPE_PARAM);
    return true;
}

void Generator::endVisit(ParameterList const &_node)
{
    popScope();
}

bool Generator::visit(OverrideSpecifier const &_node)
{
    LOGT("Generator in %s", "OverrideSpecifier");
    nodeTypes_.insert(::std::make_pair(_node.id(), "OverrideSpecifier"));
    return true;
}

bool Generator::visit(FunctionDefinition const &_node)
{
    LOGT("Generator in %s", "FunctionDefinition");
    pushScope(_node.name() + ".");

    // register function parameters
    ::std::vector<::std::string> paramNames;
    for (auto const &param : _node.parameters())
    {
        auto name = param->name();
        paramNames.push_back(name);
    }

    // register function return values
    for (auto const &ret : _node.returnParameters())
    {
        auto name = ret->name();
        paramNames.push_back(name);
    }

    functionParams_.insert(::std::make_pair(_node.name(), paramNames));

    nodeTypes_.insert(::std::make_pair(_node.id(), "FunctionDefinition"));
    return true;
}

void Generator::endVisit(FunctionDefinition const &_node)
{
    // create alias for block io places
    auto blockInPlace = network_->getPlaceByName(::std::to_string(_node.body().id()) + ".in");
    auto blockOutPlace = network_->getPlaceByName(::std::to_string(_node.body().id()) + ".out");

    network_->alias(blockInPlace, scope() + "in");
    network_->alias(blockOutPlace, scope() + "out");

    // check return
    if (_node.returnParameters().size())
    {
        auto retPlace = network_->getPlaceByName(scope() + SCOPE_RET);

        if (retPlace == nullptr)
        {
            retPlace = network_->getPlaceByName(scope() + SCOPE_PARAM + _node.returnParameters().front()->name());
            network_->alias(retPlace, scope() + SCOPE_RET);
        }
    }

    popScope();
}

bool Generator::visit(VariableDeclaration const &_node)
{
    LOGT("Generator in %s", "VariableDeclaration");
    nodeTypes_.insert(::std::make_pair(_node.id(), "VariableDeclaration"));
    return true;
}

void Generator::endVisit(VariableDeclaration const &_node)
{
    // check if has value
    auto expr = _node.value();

    if (expr)
    {
        auto exprInPlace = network_->getPlaceByName(::std::to_string(expr->id()) + ".in");
        auto exprOutPlace = network_->getPlaceByName(::std::to_string(expr->id()) + ".out");
        auto exprResultPlace = network_->getPlaceByName(::std::to_string(expr->id()) + ".result");

        // create alias
        network_->alias(exprResultPlace, scope() + _node.name());
        network_->alias(exprInPlace, ::std::to_string(_node.id()) + ".in");
        network_->alias(exprOutPlace, ::std::to_string(_node.id()) + ".out");
    }
    else
    {
        auto name = _node.name();
        auto type = _node.typeName().annotation().type->toString();

        // create places
        ::std::shared_ptr<cpn::Place> place = ::std::make_shared<cpn::Place>(scope() + name, type);
        network_->addPlace(place);
    }
}

bool Generator::visit(ModifierDefinition const &_node)
{
    LOGT("Generator in %s", "ModifierDefinition");
    nodeTypes_.insert(::std::make_pair(_node.id(), "ModifierDefinition"));
    return true;
}

bool Generator::visit(ModifierInvocation const &_node)
{
    LOGT("Generator in %s", "ModifierInvocation");
    nodeTypes_.insert(::std::make_pair(_node.id(), "ModifierInvocation"));
    return true;
}

bool Generator::visit(EventDefinition const &_node)
{
    LOGT("Generator in %s", "EventDefinition");
    nodeTypes_.insert(::std::make_pair(_node.id(), "EventDefinition"));
    return true;
}

bool Generator::visit(ErrorDefinition const &_node)
{
    LOGT("Generator in %s", "ErrorDefinition");
    nodeTypes_.insert(::std::make_pair(_node.id(), "ErrorDefinition"));
    return true;
}

bool Generator::visit(ElementaryTypeName const &_node)
{
    LOGT("Generator in %s", "ErrorDefinition");
    nodeTypes_.insert(::std::make_pair(_node.id(), "ErrorDefinition"));
    return true;
}

bool Generator::visit(UserDefinedTypeName const &_node)
{
    LOGT("Generator in %s", "UserDefinedTypeName");
    nodeTypes_.insert(::std::make_pair(_node.id(), "UserDefinedTypeName"));
    return true;
}

bool Generator::visit(FunctionTypeName const &_node)
{
    LOGT("Generator in %s", "FunctionTypeName");
    nodeTypes_.insert(::std::make_pair(_node.id(), "FunctionTypeName"));
    return true;
}

bool Generator::visit(Mapping const &_node)
{
    LOGT("Generator in %s", "Mapping");
    nodeTypes_.insert(::std::make_pair(_node.id(), "Mapping"));
    return true;
}

bool Generator::visit(ArrayTypeName const &_node)
{
    LOGT("Generator in %s", "ArrayTypeName");
    nodeTypes_.insert(::std::make_pair(_node.id(), "ArrayTypeName"));
    return true;
}

bool Generator::visit(InlineAssembly const &_node)
{
    LOGT("Generator in %s", "InlineAssembly");
    nodeTypes_.insert(::std::make_pair(_node.id(), "InlineAssembly"));
    return true;
}

bool Generator::visit(Block const &_node)
{
    LOGT("Generator in %s", "Block");
    nodeTypes_.insert(::std::make_pair(_node.id(), "Block"));
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
    ::std::shared_ptr<cpn::Transition> con1 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con" + ".last");
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(lastStmtOutPlace, con1, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(outPlace, con1, cpn::Arc::Orientation::T2P);
    network_->addTransition(con1);
    network_->addArc(arc3);
    network_->addArc(arc4);

    // connect other statement io places
    while (cnt < _node.statements().size())
    {
        auto lastStmt = _node.statements().at(cnt - 1);

        auto curStmt = _node.statements().at(cnt);
        ::std::shared_ptr<cpn::Place> lastOutPlace = network_->getPlaceByName(::std::to_string(lastStmt->id()) + ".out");
        ::std::shared_ptr<cpn::Place> curInPlace = network_->getPlaceByName(::std::to_string(curStmt->id()) + ".in");
        ::std::shared_ptr<cpn::Transition> con = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con" + ::std::to_string(cnt++));
        network_->addTransition(con);

        // connect last stmt outplace with cur stmt inplace
        ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(lastOutPlace, con, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(curInPlace, con, cpn::Arc::Orientation::T2P);
        network_->addArc(arc5);
        network_->addArc(arc6);
    }
}

bool Generator::visit(PlaceholderStatement const &_node)
{
    LOGT("Generator in %s", "PlaceholderStatement");
    nodeTypes_.insert(::std::make_pair(_node.id(), "PlaceholderStatement"));
    return true;
}

bool Generator::visit(IfStatement const &_node)
{
    LOGT("Generator in %s", "IfStatement");
    nodeTypes_.insert(::std::make_pair(_node.id(), "IfStatement"));
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
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con0");
    network_->addTransition(con0);
    ::std::shared_ptr<cpn::Transition> con1 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con1");
    network_->addTransition(con1);
    ::std::shared_ptr<cpn::Transition> con2 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con2");
    network_->addTransition(con2);

    // get condition places
    auto conditionResultPlace = network_->getPlaceByName(::std::to_string(_node.condition().id()) + ".result");
    auto conditionInPlace = network_->getPlaceByName(::std::to_string(_node.condition().id()) + ".in");
    auto conditionOutPlace = network_->getPlaceByName(::std::to_string(_node.condition().id()) + ".out");

    // connect condition places
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(conditionInPlace, con0, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(conditionOutPlace, con1, cpn::Arc::Orientation::P2T);
    network_->addArc(arc1);
    network_->addArc(arc2);
    network_->addArc(arc3);

    // get if body io place
    auto ifBodyInPlace = network_->getPlaceByName(::std::to_string(_node.trueStatement().id()) + ".in");
    auto ifBodyOutPlace = network_->getPlaceByName(::std::to_string(_node.trueStatement().id()) + ".out");

    // create arcs
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(conditionResultPlace, con1, cpn::Arc::Orientation::BD);
    ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(ifBodyInPlace, con1, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(ifBodyOutPlace, con2, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc7 = ::std::make_shared<cpn::Arc>(outPlace, con2, cpn::Arc::Orientation::T2P);
    network_->addArc(arc4);
    network_->addArc(arc5);
    network_->addArc(arc6);
    network_->addArc(arc7);

    // create arcs for false body
    if (_node.falseStatement())
    {
        ::std::shared_ptr<cpn::Transition> con3 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con3");
        network_->addTransition(con3);
        auto ifFalseBodyInPlace = network_->getPlaceByName(::std::to_string(_node.falseStatement()->id()) + ".in");
        auto ifFalseBodyOutPlace = network_->getPlaceByName(::std::to_string(_node.falseStatement()->id()) + ".out");

        ::std::shared_ptr<cpn::Arc> arc8 = ::std::make_shared<cpn::Arc>(ifFalseBodyInPlace, con1, cpn::Arc::Orientation::T2P);
        ::std::shared_ptr<cpn::Arc> arc9 = ::std::make_shared<cpn::Arc>(ifFalseBodyOutPlace, con3, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc10 = ::std::make_shared<cpn::Arc>(outPlace, con3, cpn::Arc::Orientation::T2P);
        network_->addArc(arc8);
        network_->addArc(arc9);
        network_->addArc(arc10);
    }
    else
    {
        ::std::shared_ptr<cpn::Arc> arc8 = ::std::make_shared<cpn::Arc>(outPlace, con1, cpn::Arc::Orientation::T2P);
        network_->addArc(arc8);
    }
}

bool Generator::visit(TryCatchClause const &_node)
{
    LOGT("Generator in %s", "TryCatchClause");
    nodeTypes_.insert(::std::make_pair(_node.id(), "TryCatchClause"));
    return true;
}

bool Generator::visit(TryStatement const &_node)
{
    LOGT("Generator in %s", "TryStatement");
    nodeTypes_.insert(::std::make_pair(_node.id(), "TryStatement"));
    return true;
}

bool Generator::visit(WhileStatement const &_node)
{
    LOGT("Generator in %s", "WhileStatement");
    nodeTypes_.insert(::std::make_pair(_node.id(), "WhileStatement"));
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
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con0");
    network_->addTransition(con0);

    // get condition io place and result place
    auto conditionResultPlace = network_->getPlaceByName(::std::to_string(_node.condition().id()) + ".result");
    auto conditionInPlace = network_->getPlaceByName(::std::to_string(_node.condition().id()) + ".in");
    auto conditionOutPlace = network_->getPlaceByName(::std::to_string(_node.condition().id()) + ".out");

    // create while statement loop transition
    ::std::shared_ptr<cpn::Transition> loop = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".loop");
    network_->addTransition(loop);

    // get while loop body
    auto whileBodyInPlace = network_->getPlaceByName(::std::to_string(_node.body().id()) + ".in");
    auto whileBodyOutPlace = network_->getPlaceByName(::std::to_string(_node.body().id()) + ".out");

    // create again transition
    ::std::shared_ptr<cpn::Transition> again = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".again");
    network_->addTransition(again);

    // create arcs
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(conditionInPlace, con0, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(conditionResultPlace, loop, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(conditionOutPlace, loop, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(outPlace, loop, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(whileBodyInPlace, loop, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc7 = ::std::make_shared<cpn::Arc>(whileBodyOutPlace, again, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc8 = ::std::make_shared<cpn::Arc>(conditionInPlace, again, cpn::Arc::Orientation::T2P);
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
    LOGT("Generator in %s", "ForStatement");
    nodeTypes_.insert(::std::make_pair(_node.id(), "ForStatement"));
    return true;
}

bool Generator::visit(Continue const &_node)
{
    LOGT("Generator in %s", "Continue");
    nodeTypes_.insert(::std::make_pair(_node.id(), "Continue"));
    return true;
}

bool Generator::visit(Break const &_node)
{
    LOGT("Generator in %s", "Break");
    nodeTypes_.insert(::std::make_pair(_node.id(), "Break"));
    return true;
}

bool Generator::visit(Return const &_node)
{
    LOGT("Generator in %s", "Return");
    nodeTypes_.insert(::std::make_pair(_node.id(), "Return"));
    return true;
}

void Generator::endVisit(Return const &_node)
{
    // create inout control places
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create return transition
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con0");
    network_->addTransition(con0);

    // get function return place
    auto expr = _node.expression();

    // create ret place
    ::std::shared_ptr<cpn::Place> retPlace = ::std::make_shared<cpn::Place>(scope() + SCOPE_RET, "unknown");
    network_->addPlace(retPlace);

    if (expr == nullptr)
    {
        ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(outPlace, con0, cpn::Arc::Orientation::T2P);
        network_->addArc(arc1);
        network_->addArc(arc2);
    }
    else
    {
        // create ret transition
        ::std::shared_ptr<cpn::Transition> con1 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con1");
        network_->addTransition(con1);

        // get expression io place
        auto exprInPlace = network_->getPlaceByName(::std::to_string(_node.expression()->id()) + ".in");
        auto exprOutPlace = network_->getPlaceByName(::std::to_string(_node.expression()->id()) + ".out");
        auto exprResultPlace = network_->getPlaceByName(::std::to_string(_node.expression()->id()) + ".result");

        // connect places
        ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(exprInPlace, con0, cpn::Arc::Orientation::T2P);
        ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(exprOutPlace, con1, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(outPlace, con1, cpn::Arc::Orientation::T2P);
        ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(exprResultPlace, con1, cpn::Arc::Orientation::BD);
        ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(retPlace, con1, cpn::Arc::Orientation::BD);

        network_->addArc(arc1);
        network_->addArc(arc2);
        network_->addArc(arc3);
        network_->addArc(arc4);
        network_->addArc(arc5);
        network_->addArc(arc6);
    }
}

bool Generator::visit(Throw const &_node)
{
    LOGT("Generator in %s", "Throw");
    nodeTypes_.insert(::std::make_pair(_node.id(), "Throw"));
    return true;
}

bool Generator::visit(EmitStatement const &_node)
{
    LOGT("Generator in %s", "EmitStatement");
    nodeTypes_.insert(::std::make_pair(_node.id(), "EmitStatement"));
    return true;
}

bool Generator::visit(RevertStatement const &_node)
{
    LOGT("Generator in %s", "RevertStatement");
    nodeTypes_.insert(::std::make_pair(_node.id(), "RevertStatement"));
    return true;
}

bool Generator::visit(VariableDeclarationStatement const &_node)
{
    LOGT("Generator in %s", "VariableDeclarationStatement");
    nodeTypes_.insert(::std::make_pair(_node.id(), "VariableDeclarationStatement"));
    return true;
}

void Generator::endVisit(VariableDeclarationStatement const &_node)
{
    // create inout control places
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create VariableDeclarationStatement transition
    ::std::shared_ptr<cpn::Transition> transition = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()));
    network_->addTransition(transition);

    // check if has value
    auto expr = _node.initialValue();

    if (expr)
    {
        // get expression io place
        auto exprInPlace = network_->getPlaceByName(::std::to_string(expr->id()) + ".in");
        auto exprOutPlace = network_->getPlaceByName(::std::to_string(expr->id()) + ".out");
        auto exprResultPlace = network_->getPlaceByName(::std::to_string(expr->id()) + ".result");

        // get variable place
        auto varPlace = network_->getPlaceByName(scope() + _node.declarations().front()->name());

        // create out transition
        ::std::shared_ptr<cpn::Transition> exprOutTransition = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".valueout");
        network_->addTransition(exprOutTransition);

        // connect
        ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, transition, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(exprInPlace, transition, cpn::Arc::Orientation::T2P);
        ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(exprOutPlace, exprOutTransition, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(outPlace, exprOutTransition, cpn::Arc::Orientation::T2P);
        ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(exprResultPlace, exprOutTransition, cpn::Arc::Orientation::BD);
        ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(varPlace, exprOutTransition, cpn::Arc::Orientation::BD);
        network_->addArc(arc1);
        network_->addArc(arc2);
        network_->addArc(arc3);
        network_->addArc(arc4);
        network_->addArc(arc5);
        network_->addArc(arc6);
    }
    else
    {
        // connect
        ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, transition, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(outPlace, transition, cpn::Arc::Orientation::T2P);
        network_->addArc(arc1);
        network_->addArc(arc2);
    }
}

bool Generator::visit(ExpressionStatement const &_node)
{
    LOGT("Generator in %s", "ExpressionStatement");
    nodeTypes_.insert(::std::make_pair(_node.id(), "ExpressionStatement"));
    return true;
}

void Generator::endVisit(ExpressionStatement const &_node)
{
    // create inout control places
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create VariableDeclarationStatement transition
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con0");
    network_->addTransition(con0);
    ::std::shared_ptr<cpn::Transition> con1 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con1");
    network_->addTransition(con1);

    // get expression io place
    auto exprInPlace = network_->getPlaceByName(::std::to_string(_node.expression().id()) + ".in");
    auto exprOutPlace = network_->getPlaceByName(::std::to_string(_node.expression().id()) + ".out");

    // connect
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(exprInPlace, con0, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(exprOutPlace, con1, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(outPlace, con1, cpn::Arc::Orientation::T2P);
    network_->addArc(arc1);
    network_->addArc(arc2);
    network_->addArc(arc3);
    network_->addArc(arc4);
}

bool Generator::visit(Conditional const &_node)
{
    LOGT("Generator in %s", "Conditional");
    nodeTypes_.insert(::std::make_pair(_node.id(), "Conditional"));
    return true;
}

bool Generator::visit(Assignment const &_node)
{
    LOGT("Generator in %s", "Assignment");
    nodeTypes_.insert(::std::make_pair(_node.id(), "Assignment"));
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
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con0");
    ::std::shared_ptr<cpn::Transition> con1 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con1");
    network_->addTransition(con0);
    network_->addTransition(con1);

    // get lhs storage places
    auto lhsPlace = network_->getPlaceByName(::std::to_string(_node.leftHandSide().id()) + ".result");

    // get rhs io control places and storage place
    auto rhsResultPlace = network_->getPlaceByName(::std::to_string(_node.rightHandSide().id()) + ".result");
    auto rhsInPlace = network_->getPlaceByName(::std::to_string(_node.rightHandSide().id()) + ".in");
    auto rhsOutPlace = network_->getPlaceByName(::std::to_string(_node.rightHandSide().id()) + ".out");

    // create alias for result place
    network_->alias(lhsPlace, ::std::to_string(_node.id()) + ".result");

    // create arcs
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(rhsInPlace, con0, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(rhsOutPlace, con1, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(lhsPlace, con1, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(lhsPlace, con1, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(rhsResultPlace, con1, cpn::Arc::Orientation::BD);
    ::std::shared_ptr<cpn::Arc> arc7 = ::std::make_shared<cpn::Arc>(outPlace, con1, cpn::Arc::Orientation::T2P);
    network_->addArc(arc1);
    network_->addArc(arc2);
    network_->addArc(arc3);
    network_->addArc(arc4);
    network_->addArc(arc5);
    network_->addArc(arc6);
    network_->addArc(arc7);
}

bool Generator::visit(TupleExpression const &_node)
{
    LOGT("Generator in %s", "TupleExpression");
    nodeTypes_.insert(::std::make_pair(_node.id(), "TupleExpression"));
    return true;
}

bool Generator::visit(UnaryOperation const &_node)
{
    LOGT("Generator in %s", "UnaryOperation");
    nodeTypes_.insert(::std::make_pair(_node.id(), "UnaryOperation"));
    return true;
}

bool Generator::visit(BinaryOperation const &_node)
{
    LOGT("Generator in %s", "BinaryOperation");
    nodeTypes_.insert(::std::make_pair(_node.id(), "BinaryOperation"));
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
    ::std::shared_ptr<cpn::Transition> op = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".op");
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con0");
    ::std::shared_ptr<cpn::Transition> con1 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con1");
    network_->addTransition(op);
    network_->addTransition(con0);
    network_->addTransition(con1);

    // get lhs and rhs places
    auto lhsPlace = network_->getPlaceByName(::std::to_string(_node.leftExpression().id()) + ".result");
    auto rhsPlace = network_->getPlaceByName(::std::to_string(_node.rightExpression().id()) + ".result");
    auto lhsInPlace = network_->getPlaceByName(::std::to_string(_node.leftExpression().id()) + ".in");
    auto rhsInPlace = network_->getPlaceByName(::std::to_string(_node.rightExpression().id()) + ".in");
    auto lhsOutPlace = network_->getPlaceByName(::std::to_string(_node.leftExpression().id()) + ".out");
    auto rhsOutPlace = network_->getPlaceByName(::std::to_string(_node.rightExpression().id()) + ".out");

    // TODO: support auto type conversion
    // PSM_ASSERT(lhsPlace->color() == rhsPlace->color());

    // create place for result
    ::std::shared_ptr<cpn::Place> resultPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".result", "unknown");
    network_->addPlace(resultPlace);

    // create arcs
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(lhsInPlace, con0, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(lhsOutPlace, con1, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(rhsInPlace, con1, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(rhsOutPlace, op, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(outPlace, op, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc7 = ::std::make_shared<cpn::Arc>(lhsPlace, op, cpn::Arc::Orientation::BD);
    ::std::shared_ptr<cpn::Arc> arc8 = ::std::make_shared<cpn::Arc>(rhsPlace, op, cpn::Arc::Orientation::BD);
    ::std::shared_ptr<cpn::Arc> arc9 = ::std::make_shared<cpn::Arc>(resultPlace, op, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc10 = ::std::make_shared<cpn::Arc>(resultPlace, op, cpn::Arc::Orientation::P2T);
    network_->addArc(arc1);
    network_->addArc(arc2);
    network_->addArc(arc3);
    network_->addArc(arc4);
    network_->addArc(arc5);
    network_->addArc(arc6);
    network_->addArc(arc7);
    network_->addArc(arc8);
    network_->addArc(arc9);
    network_->addArc(arc10);
}

bool Generator::visit(FunctionCall const &_node)
{
    LOGT("Generator in %s", "FunctionCall");
    nodeTypes_.insert(::std::make_pair(_node.id(), "FunctionCall"));
    return true;
}

void Generator::endVisit(FunctionCall const &_node)
{
    // create inout control places
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create function call transition and call ret transition
    ::std::shared_ptr<cpn::Transition> callTransition = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".call");
    network_->addTransition(callTransition);
    ::std::shared_ptr<cpn::Transition> callRetTransition = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".call.ret");
    network_->addTransition(callRetTransition);

    // get function in place
    Expression *exp = const_cast<Expression *>(&_node.expression());
    ::std::string callee = "";
    if (auto identifier = dynamic_cast<Identifier *>(exp))
    {
        callee = identifier->name();
    }
    else
    {
        LOGE("FunctionCall callee not found");
        return;
    }
    auto funcInPlace = network_->getPlaceByName(callee + ".in");
    auto funcOutPlace = network_->getPlaceByName(callee + ".out");

    if (funcInPlace == nullptr || funcOutPlace == nullptr)
    {
        LOGE("Function %s not found", callee.c_str());
        return;
    }

    // connect function call transition with in place
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, callTransition, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(funcInPlace, callTransition, cpn::Arc::Orientation::T2P);
    network_->addArc(arc1);
    network_->addArc(arc2);

    // connect function call ret transition with out place
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(funcOutPlace, callRetTransition, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(outPlace, callRetTransition, cpn::Arc::Orientation::T2P);
    network_->addArc(arc3);
    network_->addArc(arc4);

    // connect function param places
    int cnt = 0;
    for (auto const &param : _node.arguments())
    {
        auto paramResultPlace = network_->getPlaceByName(::std::to_string(param->id()) + ".result");
        auto functionParamPlace = network_->getPlaceByName(callee + "." + SCOPE_PARAM + functionParams_[callee][cnt++]);
        ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(paramResultPlace, callTransition, cpn::Arc::Orientation::BD);
        ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(functionParamPlace, callTransition, cpn::Arc::Orientation::BD);
        network_->addArc(arc5);
        network_->addArc(arc6);
    }

    // alias return value(if function has return value)
    auto funcRetPlace = network_->getPlaceByName(callee + "." + SCOPE_RET);

    if (funcRetPlace)
        network_->alias(funcRetPlace, ::std::to_string(_node.id()) + ".result");
}

bool Generator::visit(FunctionCallOptions const &_node)
{
    LOGT("Generator in %s", "FunctionCallOptions");
    nodeTypes_.insert(::std::make_pair(_node.id(), "FunctionCallOptions"));
    return true;
}

bool Generator::visit(NewExpression const &_node)
{
    LOGT("Generator in %s", "NewExpression");
    nodeTypes_.insert(::std::make_pair(_node.id(), "NewExpression"));
    return true;
}

bool Generator::visit(MemberAccess const &_node)
{
    LOGT("Generator in %s", "MemberAccess");
    nodeTypes_.insert(::std::make_pair(_node.id(), "MemberAccess"));
    return true;
}

void Generator::endVisit(MemberAccess const &_node)
{
    auto exprPlace = network_->getPlaceByName(::std::to_string(_node.expression().id()) + ".result");
    auto exprInPlace = network_->getPlaceByName(::std::to_string(_node.expression().id()) + ".in");
    auto exprOutPlace = network_->getPlaceByName(::std::to_string(_node.expression().id()) + ".out");

    network_->alias(exprPlace, ::std::to_string(_node.id()) + ".result");
    network_->alias(exprInPlace, ::std::to_string(_node.id()) + ".in");
    network_->alias(exprOutPlace, ::std::to_string(_node.id()) + ".out");
}

bool Generator::visit(IndexAccess const &_node)
{
    LOGT("Generator in %s", "IndexAccess");
    nodeTypes_.insert(::std::make_pair(_node.id(), "IndexAccess"));
    return true;
}

bool Generator::visit(IndexRangeAccess const &_node)
{
    LOGT("Generator in %s", "IndexRangeAccess");
    nodeTypes_.insert(::std::make_pair(_node.id(), "IndexRangeAccess"));
    return true;
}

bool Generator::visit(Identifier const &_node)
{
    LOGT("Generator in %s", "Identifier");

    // search for local var
    auto place = network_->getPlaceByName(scope() + _node.name());

    // check if identifier is function name
    auto typeString = _node.annotation().type->toString();

    if (typeString.find("function") != ::std::string::npos)
    {
        // Do nothing with function name identifier...
        return true;
    }

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

    // create inout control places
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create transitions
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con0");
    network_->addTransition(con0);

    // create arcs
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(outPlace, con0, cpn::Arc::Orientation::T2P);
    network_->addArc(arc1);
    network_->addArc(arc2);

    network_->alias(place, ::std::to_string(_node.id()) + ".result");

    nodeTypes_.insert(::std::make_pair(_node.id(), "Identifier"));
    return true;
}

bool Generator::visit(ElementaryTypeNameExpression const &_node)
{
    LOGT("Generator in %s", "ElementaryTypeNameExpression");
    nodeTypes_.insert(::std::make_pair(_node.id(), "ElementaryTypeNameExpression"));
    return true;
}

bool Generator::visit(Literal const &_node)
{
    LOGT("Generator in %s", "Literal");
    nodeTypes_.insert(::std::make_pair(_node.id(), "Literal"));
    return true;
}

void Generator::endVisit(Literal const &_node)
{
    // create inout control places
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create ifstatement transitions
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(::std::to_string(_node.id()) + ".con0");
    network_->addTransition(con0);

    // add result place for literal(literal value in default token value)
    ::std::shared_ptr<cpn::Place> resultPlace = ::std::make_shared<cpn::Place>(::std::to_string(_node.id()) + ".result", _node.annotation().type->toString());
    network_->addPlace(resultPlace);

    // create arcs
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(outPlace, con0, cpn::Arc::Orientation::T2P);
    network_->addArc(arc1);
    network_->addArc(arc2);
}

bool Generator::visit(StructuredDocumentation const &_node)
{
    LOGT("Generator in %s", "StructuredDocumentation");
    nodeTypes_.insert(::std::make_pair(_node.id(), "StructuredDocumentation"));
    return true;
}

void Generator::dump() const
{
    if (!network_)
    {
        LOGI("Generator::dump - Network is null");
        return;
    }

    LOGI("=== CPN Network Dump Start ===");

    // Dump Places
    LOGI("Places:");
    for (const auto &place : network_->places())
    {
        LOGI("\tPlace Name: %s, Color: %s", place->name().c_str(), place->color().c_str());
    }

    // Dump Transitions
    LOGI("Transitions:");
    for (const auto &transition : network_->transitions())
    {
        LOGI("\tTransition Name: %s", transition->name().c_str());
    }

    // Dump Arcs
    LOGI("Arcs:");
    int cnt = 0;
    for (const auto &arc : network_->arcs())
    {
        cnt++;
        if (arc->place() == nullptr || arc->transition() == nullptr)
        {
            LOGW("Arc %d Place or Transition is null", cnt);
            continue;
        }
        std::string orientation;
        switch (arc->orientation())
        {
        case cpn::Arc::Orientation::P2T:
            orientation = "Place to Transition";
            break;
        case cpn::Arc::Orientation::T2P:
            orientation = "Transition to Place";
            break;
        case cpn::Arc::Orientation::BD:
            orientation = "Bidirectional";
            break;
        }
        LOGI("\tArc: Place: %s, Transition: %s, Orientation: %s",
             arc->place()->name().c_str(),
             arc->transition()->name().c_str(),
             orientation.c_str());
    }

    LOGI("=== CPN Network Dump End ===");
}

_END_PSM_NM_
