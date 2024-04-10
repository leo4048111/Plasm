#pragma once

#include "base.hpp"

#include <vector>
#include <memory>
#include <string>
#include <any>
#include <map>

_START_PSM_NM_

namespace cpn
{
    static constexpr const char *CTRL_COLOR = "unit";

    class Token
    {
    public:
        template <typename T>
        Token(::std::string color, T value) : color_(color), value_(value){};
        ~Token() = default;

        template <typename T>
        T value() const
        {
            try
            {
                return std::any_cast<T>(value_);
            }
            catch (const std::bad_any_cast &e)
            {
                throw;
            }
        }

        bool control() const
        {
            return color_ == CTRL_COLOR;
        }

    private:
        ::std::string color_;
        ::std::any value_;
    };

    class Place
    {
    public:
        Place(::std::string name, ::std::string color);
        ~Place() = default;

        ::std::string name() const
        {
            return name_;
        }

        ::std::string color() const
        {
            return color_;
        }

        bool control() const
        {
            return color_ == CTRL_COLOR;
        }

        const ::std::vector<Token> &tokens() const
        {
            return tokens_;
        }

        void addToken(Token token)
        {
            tokens_.push_back(token);
        }

        Token getToken(int idx)
        {
            return tokens_[idx];
        }

        void removeToken(int idx)
        {
            tokens_.erase(tokens_.begin() + idx);
        }

        bool entryPoint() const
        {
            return entryPoint_;
        }

        void setEntryPoint(bool entryPoint)
        {
            entryPoint_ = entryPoint;
        }

    private:
        ::std::string name_;
        ::std::string color_;
        ::std::vector<Token> tokens_;
        bool entryPoint_{false};
    };

    class Transition
    {
    public:
        Transition(::std::string name);
        ~Transition() = default;

        ::std::string name() const
        {
            return name_;
        }

    private:
        ::std::string name_;
    };

    class Arc
    {
    public:
        enum class Orientation
        {
            P2T, // place to transition
            T2P, // transition to place
            BD   // bidirectional
        };

        Arc(::std::shared_ptr<Place> place,
            ::std::shared_ptr<Transition> transition,
            Orientation orientation);

        ~Arc() = default;

        ::std::shared_ptr<Place> place() const
        {
            return place_;
        }

        ::std::shared_ptr<Transition> transition() const
        {
            return transition_;
        }

        Orientation orientation() const
        {
            return orientation_;
        }

    private:
        ::std::shared_ptr<Place> place_;
        ::std::shared_ptr<Transition> transition_;
        Orientation orientation_;
    };

    class Network
    {
    public:
        Network() = default;
        ~Network() = default;

        void addPlace(::std::shared_ptr<Place> place);
        void addTransition(::std::shared_ptr<Transition> transition);
        void addArc(::std::shared_ptr<Arc> arc);

        bool fire(::std::shared_ptr<Transition> transition);
        bool revert(::std::shared_ptr<Transition> transition);

        const ::std::vector<::std::shared_ptr<Place>> &places() const
        {
            return places_;
        }

        const ::std::vector<::std::shared_ptr<Transition>> &transitions() const
        {
            return transitions_;
        }

        const ::std::vector<::std::shared_ptr<Arc>> &arcs() const
        {
            return arcs_;
        }

        ::std::shared_ptr<Place> getPlaceByName(const ::std::string &name) const
        {
            if (place_map_.find(name) != place_map_.end())
            {
                return place_map_.at(name);
            }

            return nullptr;
        }

        ::std::shared_ptr<Transition> getTransitionByName(const ::std::string &name) const
        {
            if (transition_map_.find(name) != transition_map_.end())
            {
                return transition_map_.at(name);
            }

            return nullptr;
        }

        ::std::vector<::std::shared_ptr<Transition>> getPlaceOutDegree(::std::shared_ptr<Place> place) { return place_out_degree_map_[place->name()]; };
        ::std::vector<::std::shared_ptr<Place>> getTransitionOutDegree(::std::shared_ptr<Transition> place) { return trans_out_degree_map_[place->name()]; };

        void alias(::std::shared_ptr<Place> place, ::std::string alias);

        void alias(::std::shared_ptr<Transition> transition, ::std::string alias);

    private:
        ::std::vector<::std::shared_ptr<Place>> places_;
        ::std::vector<::std::shared_ptr<Transition>> transitions_;
        ::std::vector<::std::shared_ptr<Arc>> arcs_;

        ::std::map<::std::string, ::std::shared_ptr<Place>> place_map_;           // mapping place name to place
        ::std::map<::std::string, ::std::shared_ptr<Transition>> transition_map_; // mapping place name to place

        ::std::map<::std::string, ::std::vector<::std::shared_ptr<Place>>> trans_out_degree_map_; // mapping transition out degrees
        ::std::map<::std::string, ::std::vector<::std::shared_ptr<Place>>> trans_in_degree_map_; // mapping transition in degrees
        ::std::map<::std::string, ::std::vector<::std::shared_ptr<Transition>>> place_out_degree_map_; // mapping place out degrees
    };
}

_END_PSM_NM_