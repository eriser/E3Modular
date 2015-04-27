
#pragma once

#include <vector>
#include "JuceHeader.h"
#include "gui/Selection.h"

namespace e3 {

    using juce::Rectangle;
    using juce::Point;

    class Wire : public std::vector < Point<int> >, public SelectableItem
    {
    public:
        Wire(const Point<int>& first, const Point<int>& last);
        //Wire(Wire* wire);

        void addPoint(const Point<int>& p);
        bool hitTest(const Rectangle<int>& area);
        void getBoundingRect(Rectangle<int>& r);

        void paint(Graphics& g);

    protected:
        bool Wire::segmentIntersectsRectangle(const Rectangle<int>& rc, const Point<int>& a1, const Point<int>& a2);
    };

} // namespace e3