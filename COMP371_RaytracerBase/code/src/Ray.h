#ifndef RAY_H
#define RAY_H
//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include <Eigen/Core>

using Eigen::Vector3f;

class Ray 
{
  public:
    Ray() {}

    Ray(const Vector3f& origin, const Vector3f& direction) : orig(origin), dir(direction) {}

    Vector3f origin() const  { return orig; }
    Vector3f direction() const { return dir; }

    Vector3f at(double t) const {
        return orig + t*dir;
    }

  private:
    Vector3f orig;
    Vector3f dir;
};

#endif