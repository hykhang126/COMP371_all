#pragma once

#include "SceneObjs.h"
#include "Hittable.h"

// #define TEST

class Geometry {
public:
    string type;
    int id;
    Vector3d ac;
    Vector3d dc;
    Vector3d sc;
    float ka;
    float kd;
    float ks;
    int pc;
    bool use = true;

    Geometry(string _type, int _id, Vector3d _ac, Vector3d _dc, Vector3d _sc,
              float _ka, float _kd, float _ks, int _pc)
        : type(_type), id(_id), ac(_ac), dc(_dc), sc(_sc),
          ka(_ka), kd(_kd), ks(_ks), pc(_pc) {}
};

class sphere : public Geometry {
public:
    Vector3d centre;
    float radius;

    sphere(string _type, int _id, Vector3d _ac, Vector3d _dc, Vector3d _sc,
           float _ka, float _kd, float _ks, int _pc,
           Vector3d _centre, float _radius)
        : Geometry(_type, _id, _ac, _dc, _sc, _ka, _kd, _ks, _pc),
          centre(_centre), radius(_radius) {}
};

class rectangle : public Geometry {
public:
    Vector3d p1;
    Vector3d p2;
    Vector3d p3;
    Vector3d p4;

    rectangle(string _type, int _id, Vector3d _ac, Vector3d _dc, Vector3d _sc,
              float _ka, float _kd, float _ks, int _pc,
              Vector3d _p1, Vector3d _p2, Vector3d _p3, Vector3d _p4)
        : Geometry(_type, _id, _ac, _dc, _sc, _ka, _kd, _ks, _pc),
          p1(_p1), p2(_p2), p3(_p3), p4(_p4) {}
};



/* SHAPES */



// Sphere shape class

class Sphere : public hittable {
public:

    Sphere(Vector3d _center, double _radius) : center(_center), radius(_radius) 
    {
        auto rvec = Vector3d(radius, radius, radius);
    }

    bool hit(const Ray& r, interval ray_t, hit_record& rec, int ignored_index) const override {
        Vector3d oc = r.origin() - center;
        auto a = r.direction().dot(r.direction());
        auto half_b = oc.dot(r.direction());
        auto c = oc.dot(oc) - radius*radius;

        auto discriminant = half_b*half_b - a*c;
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        Vector3d outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

#ifdef TEST
        std::cout << "Sphere intersect! t = " << rec.t << std::endl;
#endif

        return true;
    }

private:
    Vector3d center;
    double radius;
};



// QUAD shape class

class quad : public hittable {
  public:
    quad(const Vector3d& _Q, const Vector3d& _u, const Vector3d& _v)
      : Q(_Q), u(_u), v(_v)
    {
        auto n = u.cross(v);
        normal = n.normalized();
        D = normal.dot(Q);
        w = n / n.dot(n);
    }

    bool hit(const Ray& r, interval ray_t, hit_record& rec, int ignored_index) const override {
        auto denom = normal.dot(r.direction());

        // No hit if the ray is parallel to the plane.
        if (fabs(denom) < 1e-8)
            return false;

        // Return false if the hit point parameter t is outside the ray interval.
        auto t = (D - normal.dot(r.origin())) / denom;
        if (!ray_t.contains(t))
            return false;

        // Determine the hit point lies within the planar shape using its plane coordinates.
        auto intersection = r.at(t);
        Vector3d planar_hitpt_vector = intersection - Q;
        auto alpha = w.dot(planar_hitpt_vector.cross(v));
        auto beta = w.dot(u.cross(planar_hitpt_vector));

        if (!is_interior(alpha, beta, rec))
            return false;

        // Ray hits the 2D shape; set the rest of the hit record and return true.

        rec.t = t;
        rec.p = intersection;
        rec.set_face_normal(r, normal);
        rec.hit_index = 0;

#ifdef TEST
        std::cout << "Sphere intersect! t = " << rec.t << std::endl;
#endif

        return true;
    }

    virtual bool is_interior(double a, double b, hit_record& rec) const {
        // Given the hit point in plane coordinates, return false if it is outside the
        // primitive, otherwise set the hit record UV coordinates and return true.

        if ((a < 0) || (1 < a) || (b < 0) || (1 < b))
            return false;

        rec.u = a;
        rec.v = b;
        return true;
    }

  private:
    Vector3d Q;
    Vector3d u, v;
    Vector3d normal;
    double D;
    Vector3d w;
};