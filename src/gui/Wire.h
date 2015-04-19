
#pragma once

#include <vector>
#include <map>

#include "JuceHeader.h"
//#include "gui/AudioEditor.h"
#include "core/ModuleBase.h"


namespace e3 {

    using juce::Rectangle;
    using juce::Point;

    class Wire : public std::vector < Point<float> >
    {
    public:
        Wire(const Point<float>& first, const Point<float>& last);
        Wire(Wire* wire);

        void addPoint(const Point<float>& p);
        bool hitTest(const Rectangle<float>& r);
        void getBoundingRect(Rectangle<float>& r);

        void paint(Graphics& g);

        bool selected_ = false;

    protected:
        bool Wire::segmentIntersectsRectangle(const Rectangle<float>& rc, const Point<float>& a1, const Point<float>& a2);
    };

    typedef std::map< const Link, Wire* > WireMap;

} // namespace e3