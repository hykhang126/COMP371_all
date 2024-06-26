#pragma once
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

class Ray 
{
  public:
    Ray() {}

    Ray(const Vector3d& origin, const Vector3d& direction) : orig(origin), dir(direction) {}

    Vector3d origin() const  { return orig; }
    Vector3d direction() const { return dir; }

    Vector3d at(double t) const {
        return orig + t*dir;
    }

  private:
    Vector3d orig;
    Vector3d dir;
};