#include "Color.hpp"

Color Color::operator+(const Color& c) const
{
    return Color(r+c.r, g+c.g, b+c.b);
}

bool Color::operator==(const Color& c) const
{
    return (((r==c.r) && (g==c.g)) && (b==c.b));
}

bool Color::operator!=(const Color& c) const
{
    return (((r!=c.r) || (g!=c.g)) || (b!=c.b));
}

Color Color::operator*(const double& alpha) const
{
    return Color(r*alpha, g*alpha, b*alpha);
}
