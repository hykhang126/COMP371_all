#pragma once

#include <iostream>
#include <string>
#include <Eigen/Core>
#include <Eigen/Dense>

#include <cmath>
#include <limits>
#include <memory>

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

// Common Headers

#include "Utilities.h"
#include "Ray.h"
