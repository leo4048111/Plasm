#include "cpn.hpp"

#include "logger.hpp"

_START_PSM_NM_

namespace cpn
{
    template <typename T>
    Token::Token(::std::string color, T value)
        : color_(color), value_(value)
    {
    }

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
    }
}

_END_PSM_NM_
