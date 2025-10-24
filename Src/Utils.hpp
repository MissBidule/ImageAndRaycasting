#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <iostream>
#include "Color.hpp"

//basic vector3 for any use
template <typename t>
struct Vec3 {
    t x;
    t y;
    t z;

    friend std::ostream& operator<<(std::ostream& os, const Vec3<t> v) {
        os << "{" << v.x << ";" << v.y << ";" << v.z << "}";
        return os;
    };

    Vec3<t> operator+(const Vec3<t> v) const {
        return Vec3<t>{x+v.x, y+v.y, z+v.z};
    };

    Vec3<t> operator-(const Vec3<t> v) const {
        return Vec3<t>{x-v.x, y-v.y, z-v.z};
    };
    
    Vec3<t> operator-() const {
        return Vec3<t>{-x, -y, -z};
    };
    
    Vec3<t> operator*(const Vec3<t> v) const {
        return Vec3<t>{x*v.x, y*v.y, z*v.z};
    };

    template <typename u>
    Vec3<t> operator*(u a) const {
        return Vec3<t>{x * a, y * a, z * a};
    }

    template <typename u>
    Vec3<t> operator/(u a) const {
        return Vec3<t>{x / a, y / a, z / a};
    }

    bool operator==(const Vec3<t> v) const {
        return ((x == v.x && y == v.y) && z == v.z);
    };

    bool operator!=(const Vec3<t> v) const {
        return !operator==(v);
    };

    float dot(const Vec3<t> v) const {
        return x*v.x + y*v.y + z*v.z;
    };

    Vec3<t> cross(const Vec3<t> v) const {
        return Vec3<t>{y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x};
    };
    
    Vec3<t> reflect(const Vec3<t> v) const {
        return *this - v * 2 * this->dot(v);
    };

    Vec3<t> refract(const Vec3<t> v, double etaiOverEtao) const {
        float cosTheta = std::min(-(*this).dot(v), 1.0f);
        Vec3<t> rOutPerp = (*this + v * cosTheta) * (float)etaiOverEtao;
        Vec3<t> rOutPara = v * - (float)std::sqrt(std::abs(1.0 - rOutPerp.norm2()));
        return rOutPerp + rOutPara;
    };
    
    float norm2() const {
        return this->dot(*this);
    };

    float norm() const {
        return std::sqrt(norm2());
    };

    Vec3<float> normalize() const {
        const float normVec = norm();
        if (normVec == 0) {
            return {};
        }
        return Vec3<float>{(float)x / norm(), (float)y / norm(), (float)z / norm()};
    };
};

using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;
using Vec3u = Vec3<uint16_t>;

#endif //_utils_hpp
