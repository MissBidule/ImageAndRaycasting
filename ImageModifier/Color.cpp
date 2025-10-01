#include "Color.hpp"

Color operator*(double alpha, const Color& color)
{
    return Color(color.r*alpha, color.g*alpha, color.b*alpha);
}

Color operator+(const Color& c1, const Color& c2)
{
    return Color(c1.r+c2.r, c1.g+c2.g, c1.b+c2.b);
}

bool operator==(const Color& c1, const Color& c2)
{
    return (((c1.r==c2.r) && (c1.g==c2.g)) && (c1.b==c2.b));
}

bool operator!=(const Color& c1, const Color& c2)
{
    return (((c1.r!=c2.r) || (c1.g!=c2.g)) || (c1.b!=c2.b));
}
