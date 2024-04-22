#include "generator.hpp"

#include "logger.hpp"
#include "randomizer.hpp"
#include "cpn.hpp"

_START_PSM_NM_

using namespace solidity::frontend;
using namespace solidity::langutil;

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
    // randomize msg value
    auto addr = Randomizer::GetInstance().makeAddress();
    network_->addInitialMarking(msgPlace, cpn::Token("address", addr));
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

    // register function definitions
    for (auto const &functionDefinition : _node.definedFunctions())
    {
        // register symbols
        // register function parameters
        ::std::vector<::std::string> paramNames;
        for (auto const &param : functionDefinition->parameters())
        {
            auto name = param->name();
            paramNames.push_back(name);
        }

        // register function return values
        for (auto const &ret : functionDefinition->returnParameters())
        {
            auto name = ret->name();
            paramNames.push_back(name);
        }

        // add function name
        functions_.push_back(functionDefinition->name());

        functionParams_.insert(::std::make_pair(functionDefinition->name(), paramNames));

        // make io places
        ::std::shared_ptr<cpn::Place> functionInPlace = ::std::make_shared<cpn::Place>(functionDefinition->name() + ".in", cpn::CTRL_COLOR);
        ::std::shared_ptr<cpn::Place> functionOutPlace = ::std::make_shared<cpn::Place>(functionDefinition->name() + ".out", cpn::CTRL_COLOR);
        network_->addPlace(functionInPlace);
        network_->addPlace(functionOutPlace);

        for (auto const &param : functionDefinition->parameters())
        {
            // make param places
            auto name = param->name();
            ::std::shared_ptr<cpn::Place> paramPlace = ::std::make_shared<cpn::Place>(functionDefinition->name() + "." + SCOPE_PARAM + name, "int");
            network_->addPlace(paramPlace);
        }

        // make return place
        ::std::shared_ptr<cpn::Place> retPlace = ::std::make_shared<cpn::Place>(functionDefinition->name() + "." + SCOPE_RET, "int");
        network_->addPlace(retPlace);
    }

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
    nodeTypes_.insert(::std::make_pair(_node.id(), "FunctionDefinition"));
    return true;
}

void Generator::endVisit(FunctionDefinition const &_node)
{
    // create alias for block io places
    auto blockInPlace = network_->getPlaceByName(getFullNodeType(_node.body().id()) + ".in");
    auto blockOutPlace = network_->getPlaceByName(getFullNodeType(_node.body().id()) + ".out");

    // get function io places
    auto functionInPlace = network_->getPlaceByName(scope() + "in");
    auto functionOutPlace = network_->getPlaceByName(scope() + "out");

    // connect places
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con0");
    ::std::shared_ptr<cpn::Transition> con1 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con1");
    network_->addTransition(con0);
    network_->addTransition(con1);

    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(functionInPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(blockInPlace, con0, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(blockOutPlace, con1, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(functionOutPlace, con1, cpn::Arc::Orientation::T2P);
    network_->addArc(arc1);
    network_->addArc(arc2);
    network_->addArc(arc3);
    network_->addArc(arc4);

    // check entry point
    if (_node.visibility() == Visibility::Public)
    {
        ::std::vector<::std::shared_ptr<cpn::Place>> requiredParams;
        for (auto const &param : _node.parameters())
        {
            auto name = scope() + SCOPE_PARAM + param->name();
            auto paramPlace = network_->getPlaceByName(name);
            requiredParams.push_back(paramPlace);
        }

        network_->addEntryPoint(functionInPlace, requiredParams);
    }

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
        auto exprInPlace = network_->getPlaceByName(getFullNodeType(expr->id()) + ".in");
        auto exprOutPlace = network_->getPlaceByName(getFullNodeType(expr->id()) + ".out");
        auto exprResultPlace = network_->getPlaceByName(getFullNodeType(expr->id()) + ".result");

        // create alias
        network_->alias(exprResultPlace, scope() + _node.name());
        network_->alias(exprInPlace, getFullNodeType(_node.id()) + ".in");
        network_->alias(exprOutPlace, getFullNodeType(_node.id()) + ".out");

        // check whether is global state variable
        if(::std::string(scope()) == ::std::string(SCOPE_GLOB))
        {
            network_->track(exprResultPlace);
        }
    }
    else
    {
        auto name = scope() + _node.name();
        auto type = _node.typeName().annotation().type->toString();

        // create places
        ::std::shared_ptr<cpn::Place> place = network_->getPlaceByName(name);
        if (place == nullptr)
        {
            place = ::std::make_shared<cpn::Place>(name, type);
            network_->addPlace(place);
        }
        // check whether is global state variable
        if(::std::string(scope()) == ::std::string(SCOPE_GLOB))
        {
            network_->track(place);
        }
        // Create initial markings in case value reference fails
        network_->addInitialMarking(place, cpn::Token(type, 0));
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
    pushScope(_node.name() + ".");
    nodeTypes_.insert(::std::make_pair(_node.id(), "ErrorDefinition"));
    return true;
}

void Generator::endVisit(ErrorDefinition const &_node)
{
    popScope();
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
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // connect statement io places
    if (!_node.statements().size())
    {
        ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con0");
        network_->addTransition(con0);
        ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(outPlace, con0, cpn::Arc::Orientation::T2P);
        network_->addArc(arc1);
        network_->addArc(arc2);
        return;
    }

    // connect first statementinplace with block inplace
    int cnt = 0;
    auto firstStmtInPlace = network_->getPlaceByName(getFullNodeType(_node.statements().front()->id()) + ".in");
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con" + ::std::to_string(cnt++));
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(firstStmtInPlace, con0, cpn::Arc::Orientation::T2P);
    network_->addTransition(con0);
    network_->addArc(arc1);
    network_->addArc(arc2);

    // connect last statementoutplace with block outplace
    auto lastStmtOutPlace = network_->getPlaceByName(getFullNodeType(_node.statements().back()->id()) + ".out");
    ::std::shared_ptr<cpn::Transition> con1 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con" + ".last");
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
        ::std::shared_ptr<cpn::Place> lastOutPlace = network_->getPlaceByName(getFullNodeType(lastStmt->id()) + ".out");
        ::std::shared_ptr<cpn::Place> curInPlace = network_->getPlaceByName(getFullNodeType(curStmt->id()) + ".in");
        ::std::shared_ptr<cpn::Transition> con = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con" + ::std::to_string(cnt++));
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
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create ifstatement transitions
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con0");
    network_->addTransition(con0);
    ::std::shared_ptr<cpn::Transition> con1 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con1");
    network_->addTransition(con1);
    ::std::shared_ptr<cpn::Transition> con2 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con2");
    network_->addTransition(con2);

    // get condition places
    auto conditionResultPlace = network_->getPlaceByName(getFullNodeType(_node.condition().id()) + ".result");
    auto conditionInPlace = network_->getPlaceByName(getFullNodeType(_node.condition().id()) + ".in");
    auto conditionOutPlace = network_->getPlaceByName(getFullNodeType(_node.condition().id()) + ".out");

    // connect condition places
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(conditionInPlace, con0, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(conditionOutPlace, con1, cpn::Arc::Orientation::P2T);
    network_->addArc(arc1);
    network_->addArc(arc2);
    network_->addArc(arc3);

    // get if body io place
    auto ifBodyInPlace = network_->getPlaceByName(getFullNodeType(_node.trueStatement().id()) + ".in");
    auto ifBodyOutPlace = network_->getPlaceByName(getFullNodeType(_node.trueStatement().id()) + ".out");

    // get condition boolean value
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(
        conditionResultPlace,
        con1,
        cpn::Arc::Orientation::BD,
        ::std::vector<::std::string>({"condition"}),
        [](::std::vector<cpn::Token> params) -> ::std::optional<cpn::Token>
        {
            PSM_ASSERT(params.size() == 1);
            return params[0];
        });

    // if condition is true, send control token to if body
    ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(
        ifBodyInPlace,
        con1,
        cpn::Arc::Orientation::T2P,
        ::std::vector<::std::string>({"condition"}),
        [](::std::vector<cpn::Token> params) -> ::std::optional<cpn::Token>
        {
            PSM_ASSERT(params.size() == 1);
            // FIXME: should cast to bool value
            int conditionValue = ::std::any_cast<int>(params[0].value());
            if (conditionValue)
                return cpn::Token(cpn::CTRL_COLOR, "()");
            else
                return ::std::nullopt;
        });
    ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(ifBodyOutPlace, con2, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc7 = ::std::make_shared<cpn::Arc>(outPlace, con2, cpn::Arc::Orientation::T2P);
    network_->addArc(arc4);
    network_->addArc(arc5);
    network_->addArc(arc6);
    network_->addArc(arc7);

    // create arcs for false body
    if (_node.falseStatement())
    {
        ::std::shared_ptr<cpn::Transition> con3 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con3");
        network_->addTransition(con3);
        auto ifFalseBodyInPlace = network_->getPlaceByName(getFullNodeType(_node.falseStatement()->id()) + ".in");
        auto ifFalseBodyOutPlace = network_->getPlaceByName(getFullNodeType(_node.falseStatement()->id()) + ".out");

        // trigger false body if condition is false
        ::std::shared_ptr<cpn::Arc> arc8 = ::std::make_shared<cpn::Arc>(
            ifFalseBodyInPlace,
            con1,
            cpn::Arc::Orientation::T2P,
            ::std::vector<::std::string>({"condition"}),
            [](::std::vector<cpn::Token> params) -> ::std::optional<cpn::Token>
            {
                PSM_ASSERT(params.size() == 1);
                // FIXME: should cast to bool value
                int conditionValue = ::std::any_cast<int>(params[0].value());
                if (!conditionValue)
                    return cpn::Token(cpn::CTRL_COLOR, "()");
                else
                    return ::std::nullopt;
            });
        ::std::shared_ptr<cpn::Arc> arc9 = ::std::make_shared<cpn::Arc>(ifFalseBodyOutPlace, con3, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc10 = ::std::make_shared<cpn::Arc>(outPlace, con3, cpn::Arc::Orientation::T2P);
        network_->addArc(arc8);
        network_->addArc(arc9);
        network_->addArc(arc10);
    }
    else
    {
        ::std::shared_ptr<cpn::Arc> arc8 = ::std::make_shared<cpn::Arc>(
            outPlace,
            con1,
            cpn::Arc::Orientation::T2P,
            ::std::vector<::std::string>({"condition"}),
            [](::std::vector<cpn::Token> params) -> ::std::optional<cpn::Token>
            {
                PSM_ASSERT(params.size() == 1);
                // FIXME: should cast to bool value
                int conditionValue = ::std::any_cast<int>(params[0].value());
                if (!conditionValue)
                    return cpn::Token(cpn::CTRL_COLOR, "()");
                else
                    return ::std::nullopt;
            });
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
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create whilestatement transitions
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con0");
    network_->addTransition(con0);

    // get condition io place and result place
    auto conditionResultPlace = network_->getPlaceByName(getFullNodeType(_node.condition().id()) + ".result");
    auto conditionInPlace = network_->getPlaceByName(getFullNodeType(_node.condition().id()) + ".in");
    auto conditionOutPlace = network_->getPlaceByName(getFullNodeType(_node.condition().id()) + ".out");

    // create while statement loop transition
    ::std::shared_ptr<cpn::Transition> loop = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".loop");
    network_->addTransition(loop);

    // get while loop body
    auto whileBodyInPlace = network_->getPlaceByName(getFullNodeType(_node.body().id()) + ".in");
    auto whileBodyOutPlace = network_->getPlaceByName(getFullNodeType(_node.body().id()) + ".out");

    // create again transition
    ::std::shared_ptr<cpn::Transition> again = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".again");
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
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create return transition
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con0");
    network_->addTransition(con0);

    // get function return place
    auto expr = _node.expression();

    // get ret place
    ::std::shared_ptr<cpn::Place> retPlace = network_->getPlaceByName(scope() + SCOPE_RET);

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
        ::std::shared_ptr<cpn::Transition> con1 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con1");
        network_->addTransition(con1);

        // get expression io place
        auto exprInPlace = network_->getPlaceByName(getFullNodeType(_node.expression()->id()) + ".in");
        auto exprOutPlace = network_->getPlaceByName(getFullNodeType(_node.expression()->id()) + ".out");
        auto exprResultPlace = network_->getPlaceByName(getFullNodeType(_node.expression()->id()) + ".result");

        // connect places
        ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(exprInPlace, con0, cpn::Arc::Orientation::T2P);
        ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(exprOutPlace, con1, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(outPlace, con1, cpn::Arc::Orientation::T2P);

        // reference return value
        ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(
            exprResultPlace,
            con1,
            cpn::Arc::Orientation::BD,
            ::std::vector<::std::string>({"x"}),
            [](::std::vector<cpn::Token> params) -> ::std::optional<cpn::Token>
            {
                PSM_ASSERT(params.size() == 1);
                return params[0];
            });

        // add return value to place
        ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(
            retPlace,
            con1,
            cpn::Arc::Orientation::T2P,
            ::std::vector<::std::string>({"x"}),
            [](::std::vector<cpn::Token> params) -> ::std::optional<cpn::Token>
            {
                PSM_ASSERT(params.size() == 1);
                return params[0];
            });

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
    // Does literally nothing...
    // create inout control places
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create connector
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con0");
    network_->addTransition(con0);

    // create arcs
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(outPlace, con0, cpn::Arc::Orientation::T2P);
    network_->addArc(arc1);
    network_->addArc(arc2);
    return false;
}

bool Generator::visit(RevertStatement const &_node)
{
    LOGT("Generator in %s", "RevertStatement");
    nodeTypes_.insert(::std::make_pair(_node.id(), "RevertStatement"));
    // Does literally nothing...
    // create inout control places
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create VariableDeclarationStatement transition
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con0");
    network_->addTransition(con0);

    // create arcs
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(outPlace, con0, cpn::Arc::Orientation::T2P);
    network_->addArc(arc1);
    network_->addArc(arc2);
    return false;
}

void Generator::endVisit(RevertStatement const &_node)
{
    // Does nothing...
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
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create VariableDeclarationStatement transition
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con0");
    network_->addTransition(con0);

    // check if has value
    auto expr = _node.initialValue();

    if (expr)
    {
        // get expression io place
        auto exprInPlace = network_->getPlaceByName(getFullNodeType(expr->id()) + ".in");
        auto exprOutPlace = network_->getPlaceByName(getFullNodeType(expr->id()) + ".out");
        auto exprResultPlace = network_->getPlaceByName(getFullNodeType(expr->id()) + ".result");

        // get variable place
        auto varPlace = network_->getPlaceByName(scope() + _node.declarations().front()->name());

        // create out transition
        ::std::shared_ptr<cpn::Transition> con1 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con1");
        network_->addTransition(con1);

        // connect
        ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(exprInPlace, con0, cpn::Arc::Orientation::T2P);
        ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(exprOutPlace, con1, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(outPlace, con1, cpn::Arc::Orientation::T2P);

        // get expression value
        ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(
            exprResultPlace,
            con1,
            cpn::Arc::Orientation::BD,
            ::std::vector<::std::string>({"x"}),
            [](::std::vector<cpn::Token> params) -> ::std::optional<cpn::Token>
            {
                PSM_ASSERT(params.size() == 1);
                return params[0];
            });

        // update variable place value
        ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(
            varPlace,
            con1,
            cpn::Arc::Orientation::T2P,
            ::std::vector<::std::string>({"x"}),
            [](::std::vector<cpn::Token> params) -> ::std::optional<cpn::Token>
            {
                PSM_ASSERT(params.size() == 1);
                return params[0];
            });

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
        ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(outPlace, con0, cpn::Arc::Orientation::T2P);
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
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create ExpressionStatement transition
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con0");
    network_->addTransition(con0);
    ::std::shared_ptr<cpn::Transition> con1 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con1");
    network_->addTransition(con1);

    // get expression io place
    auto exprInPlace = network_->getPlaceByName(getFullNodeType(_node.expression().id()) + ".in");
    auto exprOutPlace = network_->getPlaceByName(getFullNodeType(_node.expression().id()) + ".out");

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
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create assignment transition
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con0");
    ::std::shared_ptr<cpn::Transition> con1 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con1");
    network_->addTransition(con0);
    network_->addTransition(con1);

    // get lhs storage places
    auto lhsResultPlace = network_->getPlaceByName(getFullNodeType(_node.leftHandSide().id()) + ".result");

    // get rhs io control places and storage place
    auto rhsResultPlace = network_->getPlaceByName(getFullNodeType(_node.rightHandSide().id()) + ".result");
    auto rhsInPlace = network_->getPlaceByName(getFullNodeType(_node.rightHandSide().id()) + ".in");
    auto rhsOutPlace = network_->getPlaceByName(getFullNodeType(_node.rightHandSide().id()) + ".out");

    // create alias for result place
    network_->alias(lhsResultPlace, getFullNodeType(_node.id()) + ".result");

    // create arcs
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(rhsInPlace, con0, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(rhsOutPlace, con1, cpn::Arc::Orientation::P2T);

    // update lhs value
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(
        lhsResultPlace,
        con1,
        cpn::Arc::Orientation::T2P,
        ::std::vector<::std::string>({"x"}),
        [](::std::vector<cpn::Token> params) -> ::std::optional<cpn::Token>
        {
            PSM_ASSERT(params.size() == 1);
            return params[0];
        });

    // get rhs value
    ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(
        rhsResultPlace,
        con1,
        cpn::Arc::Orientation::BD,
        ::std::vector<::std::string>({"x"}),
        [](::std::vector<cpn::Token> params) -> ::std::optional<cpn::Token>
        {
            PSM_ASSERT(params.size() == 1);
            return params[0];
        });

    ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(outPlace, con1, cpn::Arc::Orientation::T2P);
    network_->addArc(arc1);
    network_->addArc(arc2);
    network_->addArc(arc3);
    network_->addArc(arc4);
    network_->addArc(arc5);
    network_->addArc(arc6);
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
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create binary op transition
    ::std::shared_ptr<cpn::Transition> op = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".op");
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con0");
    ::std::shared_ptr<cpn::Transition> con1 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con1");
    network_->addTransition(op);
    network_->addTransition(con0);
    network_->addTransition(con1);

    // get lhs and rhs places
    auto lhsResultPlace = network_->getPlaceByName(getFullNodeType(_node.leftExpression().id()) + ".result");
    auto rhsResultPlace = network_->getPlaceByName(getFullNodeType(_node.rightExpression().id()) + ".result");
    auto lhsInPlace = network_->getPlaceByName(getFullNodeType(_node.leftExpression().id()) + ".in");
    auto rhsInPlace = network_->getPlaceByName(getFullNodeType(_node.rightExpression().id()) + ".in");
    auto lhsOutPlace = network_->getPlaceByName(getFullNodeType(_node.leftExpression().id()) + ".out");
    auto rhsOutPlace = network_->getPlaceByName(getFullNodeType(_node.rightExpression().id()) + ".out");

    // TODO: support auto type conversion
    // PSM_ASSERT(lhsPlace->color() == rhsPlace->color());

    // create place for result
    ::std::shared_ptr<cpn::Place> resultPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".result", "unknown");
    network_->addPlace(resultPlace);

    // create arcs
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(lhsInPlace, con0, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(lhsOutPlace, con1, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(rhsInPlace, con1, cpn::Arc::Orientation::T2P);
    ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(rhsOutPlace, op, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(outPlace, op, cpn::Arc::Orientation::T2P);
    // get lhs value
    ::std::shared_ptr<cpn::Arc> arc7 = ::std::make_shared<cpn::Arc>(
        lhsResultPlace,
        op,
        cpn::Arc::Orientation::BD,
        ::std::vector<::std::string>({"x"}),
        [](::std::vector<cpn::Token> params) -> ::std::optional<cpn::Token>
        {
            PSM_ASSERT(params.size() == 1);
            return params[0];
        });

    // get rhs value
    ::std::shared_ptr<cpn::Arc> arc8 = ::std::make_shared<cpn::Arc>(
        rhsResultPlace,
        op,
        cpn::Arc::Orientation::BD,
        ::std::vector<::std::string>({"y"}),
        [](::std::vector<cpn::Token> params) -> ::std::optional<cpn::Token>
        {
            PSM_ASSERT(params.size() == 1);
            return params[0];
        });

    // remove old result value and send new value
    ::std::shared_ptr<cpn::Arc> arc9 = ::std::make_shared<cpn::Arc>(
        resultPlace,
        op,
        cpn::Arc::Orientation::T2P,
        ::std::vector<::std::string>({"x", "y"}),
        [&](::std::vector<cpn::Token> params) -> ::std::optional<cpn::Token>
        {
            PSM_ASSERT(params.size() == 2);

            auto calculate = []<typename T>(T val1, T val2, Token op) -> T
            {
                T result;
                switch (op)
                {
                case Token::Add:
                    result = val1 + val2;
                    break;
                case Token::Sub:
                    result = val1 - val2;
                    break;
                case Token::Mul:
                    result = val1 * val2;
                    break;
                case Token::Div:
                    if (val2 == 0)
                    {
                        throw std::runtime_error("Division by zero");
                    }
                    result = val1 / val2;
                    break;
                case Token::Mod:
                    result = val1 % val2;
                    break;
                case Token::And:
                    result = val1 && val2; // Logical AND
                    break;
                case Token::Or:
                    result = val1 || val2; // Logical OR
                    break;
                case Token::BitAnd:
                    result = val1 & val2; // Bitwise AND
                    break;
                case Token::BitOr:
                    result = val1 | val2; // Bitwise OR
                    break;
                case Token::BitXor:
                    result = val1 ^ val2; // Bitwise XOR
                    break;
                case Token::SHL:
                    result = val1 << val2; // Shift left
                    break;
                case Token::SHR:
                    result = val1 >> val2; // Shift right
                    break;
                case Token::Equal:
                    result = val1 == val2; // Equal
                    break;
                case Token::NotEqual:
                    result = val1 != val2; // Not equal
                    break;
                case Token::LessThan:
                    result = val1 < val2; // Less than
                    break;
                case Token::GreaterThan:
                    result = val1 > val2; // Greater than
                    break;
                case Token::LessThanOrEqual:
                    result = val1 <= val2; // Less than or equal
                    break;
                case Token::GreaterThanOrEqual:
                    result = val1 >= val2; // Greater than or equal
                    break;
                default:
                    throw std::invalid_argument("Unsupported operation type");
                }

                return result;
            };

            Token op = _node.getOperator();

            if(params[0].color() == "address")
            {
                ::std::string val1;
                ::std::string val2;
                if(params[0].value().type() == typeid(int))
                {
                    val1 = ::std::to_string(::std::any_cast<int>(params[0].value()));
                } else {
                    val1 = ::std::any_cast<::std::string>(params[0].value());
                }
                if(params[1].value().type() == typeid(int))
                {
                    val2 = ::std::to_string(::std::any_cast<int>(params[1].value()));
                } else {
                    val2 = ::std::any_cast<::std::string>(params[1].value());
                }
                // FIXME: calculate any types with calculate()
                // FIXME: support operator for address
                return cpn::Token(params[0].color(), (int)(val1 == val2));
            } else
            {
                auto val1 = ::std::any_cast<int>(params[0].value());
                auto val2 = ::std::any_cast<int>(params[1].value());
                auto result = calculate(val1, val2, op);
                return cpn::Token(params[0].color(), result);
            }

        });

    network_->addArc(arc1);
    network_->addArc(arc2);
    network_->addArc(arc3);
    network_->addArc(arc4);
    network_->addArc(arc5);
    network_->addArc(arc6);
    network_->addArc(arc7);
    network_->addArc(arc8);
    network_->addArc(arc9);
}

bool Generator::visit(FunctionCall const &_node)
{
    LOGT("Generator in %s", "FunctionCall");
    nodeTypes_.insert(::std::make_pair(_node.id(), "FunctionCall"));
    return true;
}

void Generator::endVisit(FunctionCall const &_node)
{
    // get function in place
    Expression *exp = const_cast<Expression *>(&_node.expression());
    ::std::string callee = "";
    // Normal function call
    if (auto identifier = dynamic_cast<Identifier *>(exp))
    {
        callee = identifier->name();
    }
    else if (auto elementTypenameExpression = dynamic_cast<ElementaryTypeNameExpression *>(exp))
    {
        callee = "typeConversion";
        auto resultType = elementTypenameExpression->type().typeName().toString();
        ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".in", cpn::CTRL_COLOR);
        ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".out", cpn::CTRL_COLOR);
        network_->addPlace(inPlace);
        network_->addPlace(outPlace);

        // create result place
        ::std::shared_ptr<cpn::Place> resultPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".result", resultType);
        network_->addPlace(resultPlace);

        // create con0
        ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con0");
        network_->addTransition(con0);

        // connect arcs
        ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
        ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(outPlace, con0, cpn::Arc::Orientation::T2P);
        network_->addArc(arc1);
        network_->addArc(arc2);

        for (auto const &param : _node.arguments())
        {
            auto paramResultPlace = network_->getPlaceByName(getFullNodeType(param->id()) + ".result");

            ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(
                paramResultPlace,
                con0,
                cpn::Arc::Orientation::BD,
                ::std::vector<::std::string>({"x"}),
                [](::std::vector<cpn::Token> params) -> ::std::optional<cpn::Token>
                {
                    PSM_ASSERT(params.size() == 1);
                    return params[0];
                });

            ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(
                resultPlace,
                con0,
                cpn::Arc::Orientation::T2P,
                ::std::vector<::std::string>({"x"}),
                [resultType](::std::vector<cpn::Token> params) -> ::std::optional<cpn::Token>
                {
                    PSM_ASSERT(params.size() == 1);
                    return cpn::Token(resultType, params[0].value());
                });

            network_->addArc(arc3);
            network_->addArc(arc4);
        }
        return;
    }
    else
    {
        LOGE("FunctionCall callee not found");
        return;
    }

    // create inout control places
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create function call transition and call ret transition
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con0");
    network_->addTransition(con0);
    ::std::shared_ptr<cpn::Transition> con1 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con1");
    network_->addTransition(con1);
    auto funcInPlace = network_->getPlaceByName(callee + ".in");
    auto funcOutPlace = network_->getPlaceByName(callee + ".out");

    if (funcInPlace == nullptr || funcOutPlace == nullptr)
    {
        LOGE("Function %s not found", callee.c_str());
        return;
    }

    // connect function call transition with in place
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(funcInPlace, con0, cpn::Arc::Orientation::T2P);
    network_->addArc(arc1);
    network_->addArc(arc2);

    // connect function call ret transition with out place
    ::std::shared_ptr<cpn::Arc> arc3 = ::std::make_shared<cpn::Arc>(funcOutPlace, con1, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc4 = ::std::make_shared<cpn::Arc>(outPlace, con1, cpn::Arc::Orientation::T2P);
    network_->addArc(arc3);
    network_->addArc(arc4);

    // connect function param places
    int cnt = 0;
    for (auto const &param : _node.arguments())
    {
        auto paramResultPlace = network_->getPlaceByName(getFullNodeType(param->id()) + ".result");
        auto paramName = functionParams_[callee][cnt++];
        auto functionParamPlace = network_->getPlaceByName(callee + "." + SCOPE_PARAM + paramName);
        ::std::shared_ptr<cpn::Arc> arc5 = ::std::make_shared<cpn::Arc>(
            paramResultPlace,
            con0,
            cpn::Arc::Orientation::BD,
            ::std::vector<::std::string>({paramName}),
            [](::std::vector<cpn::Token> params) -> ::std::optional<cpn::Token>
            {
                PSM_ASSERT(params.size() == 1);
                return params[0];
            });
        ::std::shared_ptr<cpn::Arc> arc6 = ::std::make_shared<cpn::Arc>(
            functionParamPlace,
            con0,
            cpn::Arc::Orientation::T2P,
            ::std::vector<::std::string>({paramName}),
            [](::std::vector<cpn::Token> params) -> ::std::optional<cpn::Token>
            {
                PSM_ASSERT(params.size() == 1);
                return params[0];
            });
        network_->addArc(arc5);
        network_->addArc(arc6);
    }

    // alias return value(if function has return value)
    auto funcRetPlace = network_->getPlaceByName(callee + "." + SCOPE_RET);

    if (funcRetPlace)
        network_->alias(funcRetPlace, getFullNodeType(_node.id()) + ".result");
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
    auto exprPlace = network_->getPlaceByName(getFullNodeType(_node.expression().id()) + ".result");
    auto exprInPlace = network_->getPlaceByName(getFullNodeType(_node.expression().id()) + ".in");
    auto exprOutPlace = network_->getPlaceByName(getFullNodeType(_node.expression().id()) + ".out");

    network_->alias(exprPlace, getFullNodeType(_node.id()) + ".result");
    network_->alias(exprInPlace, getFullNodeType(_node.id()) + ".in");
    network_->alias(exprOutPlace, getFullNodeType(_node.id()) + ".out");
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
    nodeTypes_.insert(::std::make_pair(_node.id(), "Identifier"));
    return true;
}

void Generator::endVisit(Identifier const &_node)
{
    // search for local var
    auto place = network_->getPlaceByName(scope() + _node.name());

    // check if identifier is function name
    auto typeString = _node.annotation().type->toString();

    if (typeString.find("function") != ::std::string::npos)
    {
        // Do nothing with function name identifier...
        return;
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
        return;
    }

    // create inout control places
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create transitions
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con0");
    network_->addTransition(con0);

    // create arcs
    ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(outPlace, con0, cpn::Arc::Orientation::T2P);
    network_->addArc(arc1);
    network_->addArc(arc2);

    network_->alias(place, getFullNodeType(_node.id()) + ".result");
}

bool Generator::visit(ElementaryTypeNameExpression const &_node)
{
    LOGT("Generator in %s", "ElementaryTypeNameExpression");
    nodeTypes_.insert(::std::make_pair(_node.id(), "ElementaryTypeNameExpression"));
    return true;
}

void Generator::endVisit(ElementaryTypeNameExpression const &_node)
{
    // Does literally nothing...
    // // create inout control places
    // ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".in", cpn::CTRL_COLOR);
    // ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".out", cpn::CTRL_COLOR);
    // network_->addPlace(inPlace);
    // network_->addPlace(outPlace);

    // // create ElementaryTypeNameExpression transition
    // ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con0");
    // network_->addTransition(con0);

    // // create arcs
    // ::std::shared_ptr<cpn::Arc> arc1 = ::std::make_shared<cpn::Arc>(inPlace, con0, cpn::Arc::Orientation::P2T);
    // ::std::shared_ptr<cpn::Arc> arc2 = ::std::make_shared<cpn::Arc>(outPlace, con0, cpn::Arc::Orientation::T2P);
    // network_->addArc(arc1);
    // network_->addArc(arc2);
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
    ::std::shared_ptr<cpn::Place> inPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".in", cpn::CTRL_COLOR);
    ::std::shared_ptr<cpn::Place> outPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".out", cpn::CTRL_COLOR);
    network_->addPlace(inPlace);
    network_->addPlace(outPlace);

    // create ifstatement transitions
    ::std::shared_ptr<cpn::Transition> con0 = ::std::make_shared<cpn::Transition>(getFullNodeType(_node.id()) + ".con0");
    network_->addTransition(con0);

    // add result place for literal(literal value in default token value)
    ::std::shared_ptr<cpn::Place> resultPlace = ::std::make_shared<cpn::Place>(getFullNodeType(_node.id()) + ".result", _node.annotation().type->toString());
    network_->addPlace(resultPlace);

    // add literal token
    auto tokenValue = _node.value();
    if (tokenValue.size())
    {
        if (tokenValue[0] >= '0' && tokenValue[0] <= '9')
            network_->addInitialMarking(resultPlace, cpn::Token(_node.annotation().type->toString(), ::std::stoi(_node.value())));
        else if (tokenValue[0] == 't') // "true"
            // FIXME: should be bool value
            network_->addInitialMarking(resultPlace, cpn::Token(_node.annotation().type->toString(), 1));
        else if (tokenValue[0] == 'f') // "false"
            // FIXME: should be bool value
            network_->addInitialMarking(resultPlace, cpn::Token(_node.annotation().type->toString(), 0));
    }
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
