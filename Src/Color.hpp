#ifndef _COLOR_HPP_
#define _COLOR_HPP_

#include <math.h>
#include <cstdint>
#include "Utils.hpp"

//RGB color class
class Color {
    public :
        uint8_t r, g, b;
        Color(uint8_t _r=0, uint8_t _g=0, uint8_t _b=0);
    
        static Color colorFromFloat(float _r, float _g, float _b);
        static inline uint8_t toGreyScale(Color color) {return (color.r + color.g + color.b) / 3;};
        static inline Color toRGB(uint8_t grey) {return Color(grey, grey, grey);};
        static inline Color colorFromVec3f(Vec3f color) {return Color::colorFromFloat(color.x, color.y, color.z);}
    
        operator Vec3f() const;
            
        //Operator overloading
        Color operator+(const Color& c) const;
        Color operator&(const Color& c) const;
        Color operator|(const Color& c) const;
        bool operator==(const Color& c) const;
        bool operator!=(const Color& c) const;
        Color operator*(const double& alpha) const;
};

#endif // _COLOR_HPP_
