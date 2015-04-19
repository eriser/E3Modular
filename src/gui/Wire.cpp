
#include <e3_Exception.h>
#include "gui/Style.h"
#include "gui/Wire.h"


namespace e3 {

    Wire::Wire(const Point<float>& first, const Point<float>& last)
    {
        addPoint(first);
        addPoint(last);
    }


    Wire::Wire(Wire* other)
    {
        ASSERT(other);
        selected_ = other->selected_;

        for (uint16_t i = 0; i < other->size(); i++) {
            push_back(other->at(i));
        }
    }


    void Wire::addPoint(const Point<float>& p)
    {
        push_back(p);
    }


    void Wire::paint(Graphics& g)
    {
        if (size() < 2) {
            return;
        }

        Colour c = selected_ ? Colour(0xff5f5f60) : Colour(0xffd08930);
        g.setColour(c);
        g.drawLine(Line<float>(front(), back()), 1);
    }


    void Wire::getBoundingRect(Rectangle<float>& r)
    {
        if (empty() == false)
        {
            r = Rectangle<float>(front(), back());
        }
        else ASSERT(false);
    }


    bool Wire::segmentIntersectsRectangle(const Rectangle<float>& r, const Point<float>& a1, const Point<float>& a2)
    {
        // Find min and max X for the segment
        float minX = jmin(a1.x, a2.x);
        float maxX = jmax(a1.x, a2.x);

        // Find the intersection of the segment's and rectangle's x-projections
        if (maxX > r.getRight()) {
            maxX = r.getRight();
        }
        if (minX < r.getX()) {
            minX = r.getX();
        }

        // If their projections do not intersect return false
        if (minX > maxX)  {
            return false;
        }

        // Find corresponding min and max Y for min and max X we found before
        float minY = a1.y;
        float maxY = a2.y;

        // TODO: try juce::Point::DotProduct
        float dx = a2.x - a1.x;
        if (abs(dx) > 0.0000001)
        {
            float a = (a2.y - a1.y) / dx;
            float b = a1.y - a * a1.x;
            minY = a * minX + b;
            maxY = a * maxX + b;
        }
        if (minY > maxY) {
            float tmp = maxY;
            maxY = minY;
            minY = tmp;
        }
        // Find the intersection of the segment's and rectangle's y-projections
        if (maxY > r.getBottom()) {
            maxY = r.getBottom();
        }
        if (minY < r.getY()) {
            minY = r.getY();
        }
        if (minY > maxY) { // If Y-projections do not intersect return false
            return false;
        }
        return true;
    }


    bool Wire::hitTest(const Rectangle<float>& r)
    {
        selected_ = segmentIntersectsRectangle(r, front(), back());
        return selected_;
    }

} // namespace e3