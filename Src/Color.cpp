#include "Color.hpp"

Color::Color(uint8_t _r, uint8_t _g, uint8_t _b) : r(_r), g(_g), b(_b) {}

Color Color::colorFromFloat(float _r, float _g, float _b) {
    //prevents to go over 255
    _r = _r < 0 ? 0 : _r > 1 ? 1 : _r;
    _g = _g < 0 ? 0 : _g > 1 ? 1 : _g;
    _b = _b < 0 ? 0 : _b > 1 ? 1 : _b;
    Color returnColor = Color(_r * 255, _g * 255, _b * 255);
    return returnColor;
}

Color Color::operator+(const Color& c) const
{
    return Color(r+c.r, g+c.g, b+c.b);
}

Color Color::operator&(const Color& c) const {
    return Color(r&c.r, g&c.g, b&c.b);
}

Color Color::operator|(const Color& c) const {
    return Color(r|c.r, g|c.g, b|c.b);
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

Color::operator Vec3f() const {
    return Vec3f{r/255.0f, g/255.0f, b/255.0f};
}
