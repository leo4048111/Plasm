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
            trans_in_degree_map_[arc->transition()->name()].push_back(arc->place());
            place_out_degree_map_[arc->place()->name()].push_back(arc->transition());
        }
        else
        {
            trans_out_degree_map_[arc->transition()->name()].push_back(arc->place());
        }
    }

    bool Network::fire(::std::shared_ptr<Transition> transition)
    {
        for (auto &place : trans_in_degree_map_[transition->name()])
        {
            if (!place->tokens().size())
                return false;
        }

        for (auto &place : trans_in_degree_map_[transition->name()])
        {
            place->removeToken(0);
        }

        for (auto &place : trans_out_degree_map_[transition->name()])
        {
            place->addToken(Token("Unknown", "Unknown"));
        }

        return true;
    }

    bool Network::revert(::std::shared_ptr<Transition> transition)
    {
        for (auto &place : trans_out_degree_map_[transition->name()])
        {
            if (!place->tokens().size())
                return false;
        }

        for (auto &place : trans_in_degree_map_[transition->name()])
        {
            place->addToken(Token("Unknown", "Unknown"));
        }

        for (auto &place : trans_out_degree_map_[transition->name()])
        {
            place->removeToken(0);
        }

        return true;
    }

    ::std::string Network::hash() const
    {
        ::std::string hash = "";

        for (auto &place : places_)
        {
            for(auto &token : place->tokens())
            {
                hash += token.value();
            }
        }

        ::std::stringstream ss;
        ss << "0x" << ::std::hex << ::std::hash<::std::string>{}(hash);
        return ss.str();
    }
}

_END_PSM_NM_
