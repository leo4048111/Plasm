#include "cpn.hpp"

#include "logger.hpp"

#include <sstream>

_START_PSM_NM_

namespace cpn
{
    Place::Place(::std::string name, ::std::string color)
        : name_(name), color_(color)
    {
    }

    Transition::Transition(::std::string name)
        : name_(name)
    {
    }

    Arc::Arc(::std::shared_ptr<Place> place,
             ::std::shared_ptr<Transition> transition,
             Orientation orientation) : place_(place), transition_(transition), orientation_(orientation)
    {
    }

    ExpressionArc::ExpressionArc(::std::shared_ptr<Place> place,
                                 ::std::shared_ptr<Transition> transition,
                                 Orientation orientation,
                                 ::std::vector<::std::string> arguments,
                                 ArcExpression expression)
        : Arc(place, transition, orientation)
        {
            if(expression == nullptr) {
            expression_ = [](::std::vector<::std::any>) -> Token
            {
                return Token(cpn::CTRL_COLOR, "()");
            };
            }
            if(arguments.empty())
                arguments_ = {"()"};
        }

    void Network::addPlace(::std::shared_ptr<Place> place)
    {
        places_.push_back(place);
        place_map_.insert(::std::make_pair(place->name(), place));
    }

    void Network::addTransition(::std::shared_ptr<Transition> transition)
    {
        transitions_.push_back(transition);
        transition_map_.insert(::std::make_pair(transition->name(), transition));
    }

    void Network::alias(::std::shared_ptr<Place> place, ::std::string alias)
    {
        place_map_.insert(::std::make_pair(alias, place));
    }

    void Network::alias(::std::shared_ptr<Transition> transition, ::std::string alias)
    {
        transition_map_.insert(::std::make_pair(alias, transition));
    }

    void Network::addArc(::std::shared_ptr<Arc> arc)
    {
        arcs_.push_back(arc);

        if (arc->orientation() == Arc::Orientation::P2T)
        {
            trans_in_degree_map_[arc->transition()->name()].push_back(arc);
            place_out_degree_map_[arc->place()->name()].push_back(arc);
        }
        else
        {
            trans_out_degree_map_[arc->transition()->name()].push_back(arc);
            place_in_degree_map_[arc->place()->name()].push_back(arc);
        }
    }

    bool Network::fire(::std::shared_ptr<Transition> transition)
    {
        // check tokens
        for (auto &arc : trans_in_degree_map_[transition->name()])
        {
            auto place = arc->place();
            if (place->empty())
                return false;
        }

        // mapping variable names to token value
        ::std::map<::std::string, Token> symbols;
        for (auto &arc : trans_in_degree_map_[transition->name()])
        {
            auto place = arc->place();
            auto token = place->front();

            if(auto exprArc = ::std::dynamic_pointer_cast<cpn::ExpressionArc>(arc))
            {
                auto arguments = exprArc->arguments();

                for(auto& symbol : arguments) 
                {
                    symbols[symbol] = token;
                    place->pop();
                }
            }
        }

        // check arc annotations
        for (auto &arc : trans_out_degree_map_[transition->name()])
        {
            auto place = arc->place();
            if(auto exprArc = ::std::dynamic_pointer_cast<cpn::ExpressionArc>(arc))
            {
            ::std::vector<::std::any> arguments;
            auto requiredArguments = exprArc->arguments();
            for(auto& requiredArgumentName : requiredArguments)
            {
                if(symbols.count(requiredArgumentName) == 0)
                    return false;
                
                arguments.push_back(symbols[requiredArgumentName]);
            }

            auto outToken = exprArc->parse(arguments);
            place->push(outToken);
            }
        }

        return true;
    }

    bool Network::revert(::std::shared_ptr<Transition> transition)
    {
        for (auto &arc : trans_out_degree_map_[transition->name()])
        {
            auto place = arc->place();
            if (place->empty())
                return false;
        }

        for (auto &arc : trans_out_degree_map_[transition->name()])
        {
            auto place = arc->place();
            place->pop();
        }

        for (auto &arc : trans_in_degree_map_[transition->name()])
        {
            auto place = arc->place();
            place->push(Token(cpn::CTRL_COLOR, "()"));
        }

        return true;
    }

    ::std::string Network::hash() const
    {
        ::std::string hash = "";

        for (auto &place : places_)
        {
            for (auto &token : place->tokens())
            {
                if(token.type() == typeid(::std::string))
                    hash += ::std::any_cast<::std::string>(token.value());
                else if(token.type() == typeid(int))
                    hash += ::std::any_cast<int>(token.value());
            }
        }

        ::std::stringstream ss;
        ss << "0x" << ::std::hex << ::std::hash<::std::string>{}(hash);
        return ss.str();
    }
}

_END_PSM_NM_
