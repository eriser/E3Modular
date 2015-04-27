
#include <e3_Exception.h>
#include "core/Settings.h"
#include "gui/Style.h"
#include "gui/Wire.h"


namespace e3 {

    Wire::Wire(const Point<int>& first, const Point<int>& last)
    {
        addPoint(first);
        addPoint(last);
    }


    //Wire::Wire(Wire* other)
    //{
    //    ASSERT(other);
    //    selected_ = other->selected_;

    //    for (size_t i = 0; i < other->size(); i++) {
    //        push_back(other->at(i));
    //    }
    //}


    void Wire::addPoint(const Point<int>& p)
    {
        push_back(p);
    }


    void Wire::paint(Graphics& g)
    {
        if (size() < 2) {
            return;
        }

        Style* style = Settings::getInstance().getStyle();
        Colour c = selected_ ? style->findColour(Style::kWire2ColourId) : style->findColour(Style::kWire1ColourId);
        g.setColour(c);
        g.drawLine(Line<float>(front().toFloat(), back().toFloat()), 1);
    }


    void Wire::getBoundingRect(Rectangle<int>& r)
    {
        if (empty() == false)
        {
            r = Rectangle<int>(front(), back());
        }
        else ASSERT(false);
    }


    bool Wire::segmentIntersectsRectangle(const Rectangle<int>& r, const Point<int>& a1, const Point<int>& a2)
    {
        // Find min and max X for the segment
        double minX = std::min<double>(a1.x, a2.x);
        double maxX = std::max<double>(a1.x, a2.x);

        // Find the intersection of the segment's and rectangle's x-projections
        maxX = std::min<double>(maxX, r.getRight());
        minX = std::max<double>(minX, r.getX());

        // If their projections do not intersect return false
        if (minX > maxX)  {
            return false;
        }

        // Find corresponding min and max Y for min and max X we found before
        double minY = a1.y;
        double maxY = a2.y;

        // TODO: try juce::Point::DotProduct
        double dx = a2.x - a1.x;
        if (abs(dx) > 0.0000001)
        {
            double a = (a2.y - a1.y) / dx;
            double b = a1.y - a * a1.x;
            minY = a * minX + b;
            maxY = a * maxX + b;
        }
        if (minY > maxY) {
            std::swap(minY, maxY);
        }
        // Find the intersection of the segment's and rectangle's y-projections
        maxY = std::min<double>(maxY, r.getBottom());
        minY = std::max<double>(minY, r.getY());

        return minY <= maxY;        // If Y-projections do not intersect return false
    }


    bool Wire::hitTest(const Rectangle<int>& area)
    {
        //selected_ = area.intersects(Line<int>(front(), back()));
        return segmentIntersectsRectangle(area, front(), back());
    }

} // namespace e3