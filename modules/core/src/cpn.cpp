#include "cpn.hpp"

#include "logger.hpp"

_START_PSM_NM_

namespace cpn
{
    template <typename T>
    Token::Token(::std::string color, T value, bool isControl)
        : color_(color), value_(value), isControl_(isControl)
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
    }

    void Network::addTransition(::std::shared_ptr<Transition> transition)
    {
        transitions_.push_back(transition);
    }

    void Network::addArc(::std::shared_ptr<Arc> arc)
    {
        arcs_.push_back(arc);
    }
}

_END_PSM_NM_
