#pragma once

#include <iostream>
#include <string>
#include <cmath>
#include <limits>
#include <memory>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <algorithm>

// Usings

using Eigen::Vector3f;
using std::string;
using std::vector;
using color = Vector3f;

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max+1));
}

template <typename T>
T clip(const T& n, const T& lower, const T& upper) {
    return std::max(lower, std::min(n, upper));
}

// TODO: make the random be cosine weighted
inline Vector3f random_in_unit_sphere(Vector3f& normal) {
    while (true) {
        auto p = Vector3f(random_double(-1,1), random_double(-1,1), random_double(-1,1));
        if (p.squaredNorm() >= 1) continue;
        
        if (p.dot(normal) > 0) {
            return p;
        }
        else 
            return -p;
    }
}

// Common Headers

#include "Ray.h"
#include "Utilities.h"
