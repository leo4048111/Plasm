#pragma once

#include "base.hpp"

#include <vector>
#include <memory>
#include <string>
#include <any>
#include <map>
#include <functional>
#include <optional>
#include <regex>

_START_PSM_NM_

namespace cpn
{
    static constexpr const char *CTRL_COLOR = "unit";

    class Token
    {
    public:
        Token(::std::string color, ::std::any value) : color_(color), value_(value){};
        Token() = default;
        ~Token() = default;

        ::std::any value() const
        {
            return value_;
        }

        ::std::string color() const
        {
            return color_;
        }

        bool control() const
        {
            return color_ == CTRL_COLOR;
        }

        bool mapping() const {
            ::std::regex mappingPattern(R"(mapping\((\w+)\s*=>\s*(\w+)\))");
            return ::std::regex_search(color_, mappingPattern);
        }

        bool array() const {
            ::std::regex arrayPattern(R"([a-zA-Z]+\[[0-9]*\])");
            return ::std::regex_search(color_, arrayPattern);
        }

        const ::std::type_info &type() const { return value_.type(); }

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

        bool empty() const
        {
            return tokens_.empty();
        }

        size_t size() const
        {
            return tokens_.size();
        }

        void push(Token token)
        {
            tokens_.push_back(token);
        }

        const ::std::vector<Token> &tokens() const { return tokens_; };

        const Token &top()
        {
            return tokens_.back();
        }

        void pop()
        {
            tokens_.pop_back();
        }

        void clear()
        {
            tokens_.clear();
        }

    private:
        ::std::string name_;
        ::std::string color_;
        ::std::vector<Token> tokens_;
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
        using ArcExpression = ::std::function<::std::optional<Token>(::std::vector<Token>)>;

        enum class Orientation
        {
            P2T, // place to transition
            T2P, // transition to place
            BD   // bidirectional
        };

        enum class Type
        {
            EXPRESSION,
            CONDITIONAL
        };

        Arc(::std::shared_ptr<Place> place,
                      ::std::shared_ptr<Transition> transition,
                      Orientation orientation,
                      ::std::vector<::std::string> arguments = {},
                      ArcExpression expression = nullptr);

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

        ::std::optional<Token> parse(::std::vector<Token> arguments)
        {
            return expression_(arguments);
        }

        const ::std::vector<::std::string> &arguments() const
        {
            return arguments_;
        }

    private:
        ::std::shared_ptr<Place> place_;
        ::std::shared_ptr<Transition> transition_;
        Orientation orientation_;
        ::std::vector<::std::string> arguments_;
        ArcExpression expression_;
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

        ::std::string hash() const;

        void track(::std::shared_ptr<Place> place) { tracked_places_.push_back(place); };

        void addEntryPoint(::std::shared_ptr<Place> place, ::std::vector<::std::shared_ptr<Place>> requiredParams)
        {
            entry_points_info_.insert(::std::make_pair(place->name(), requiredParams));
        }

        void addInitialMarking(::std::shared_ptr<Place> place, Token token)
        {
            place->push(token);
            initial_markings_[place->name()].push_back(token);
        }

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

        void reset();

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

        ::std::map<::std::string, ::std::vector<::std::shared_ptr<Place>>> getEntryPointsInfo() const { return entry_points_info_; };
        ::std::vector<::std::shared_ptr<Arc>> getPlaceOutDegree(::std::shared_ptr<Place> place) { return place_out_degree_map_[place->name()]; };
        ::std::vector<::std::shared_ptr<Arc>> getTransitionOutDegree(::std::shared_ptr<Transition> transition) { return trans_out_degree_map_[transition->name()]; };

        void alias(::std::shared_ptr<Place> place, ::std::string alias);

        void alias(::std::shared_ptr<Transition> transition, ::std::string alias);

    private:
        ::std::vector<::std::shared_ptr<Place>> places_;
        ::std::vector<::std::shared_ptr<Transition>> transitions_;
        ::std::vector<::std::shared_ptr<Arc>> arcs_;

        ::std::map<::std::string, ::std::shared_ptr<Place>> place_map_;                        // mapping place name to place
        ::std::map<::std::string, ::std::shared_ptr<Transition>> transition_map_;              // mapping place name to place
        ::std::map<::std::string, ::std::vector<::std::shared_ptr<Place>>> entry_points_info_; // network entry points and required params

        ::std::map<::std::string, ::std::vector<::std::shared_ptr<Arc>>> trans_out_degree_map_; // mapping transition out degrees
        ::std::map<::std::string, ::std::vector<::std::shared_ptr<Arc>>> trans_in_degree_map_;  // mapping transition in degrees
        ::std::map<::std::string, ::std::vector<::std::shared_ptr<Arc>>> place_out_degree_map_; // mapping place out degrees
        ::std::map<::std::string, ::std::vector<::std::shared_ptr<Arc>>> place_in_degree_map_;  // mapping place in degrees

        ::std::map<::std::string, ::std::vector<Token>> initial_markings_;

        ::std::vector<::std::shared_ptr<Place>> tracked_places_; // tracked places for hashing
    };
}

_END_PSM_NM_