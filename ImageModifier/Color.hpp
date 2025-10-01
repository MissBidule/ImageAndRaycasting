#ifndef _COLOR_HPP_
#define _COLOR_HPP_

#include <math.h>

//RGB color class
class Color {
    public :
        uint8_t r, g, b;
        inline Color(uint8_t _r=0, uint8_t _g=0, uint8_t _b=0) :
        r(_r), g(_g), b(_b) {};
    
    static inline uint8_t toGreyScale(Color color) {return (color.r + color.g + color.b) / 3;};
    static inline Color toRGB(uint8_t grey) {return Color(grey, grey, grey);};
        
        //Operator overloading
        friend Color operator*(double alpha, const Color& color);
        friend Color operator+(const Color& c1, const Color& c2);
        friend bool operator==(const Color& c1, const Color& c2);
        friend bool operator!=(const Color& c1, const Color& c2);
};

#endif // _COLOR_HPP_
