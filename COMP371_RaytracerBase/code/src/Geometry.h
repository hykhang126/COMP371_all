#pragma once

#include "SceneObjs.h"
#include "Hittable.h"

class Geometry {
public:
    string type;
    int id;
    Vector3f ac;
    Vector3f dc;
    Vector3f sc;
    float ka;
    float kd;
    float ks;
    int pc;

    Geometry(string _type, int _id, Vector3f _ac, Vector3f _dc, Vector3f _sc,
              float _ka, float _kd, float _ks, int _pc)
        : type(_type), id(_id), ac(_ac), dc(_dc), sc(_sc),
          ka(_ka), kd(_kd), ks(_ks), pc(_pc) {}
};

class sphere : public Geometry {
public:
    Vector3f centre;
    float radius;

    sphere(string _type, int _id, Vector3f _ac, Vector3f _dc, Vector3f _sc,
           float _ka, float _kd, float _ks, int _pc,
           Vector3f _centre, float _radius)
        : Geometry(_type, _id, _ac, _dc, _sc, _ka, _kd, _ks, _pc),
          centre(_centre), radius(_radius) {}
};

class rectangle : public Geometry {
public:
    Vector3f p1;
    Vector3f p2;
    Vector3f p3;
    Vector3f p4;

    rectangle(string _type, int _id, Vector3f _ac, Vector3f _dc, Vector3f _sc,
              float _ka, float _kd, float _ks, int _pc,
              Vector3f _p1, Vector3f _p2, Vector3f _p3, Vector3f _p4)
        : Geometry(_type, _id, _ac, _dc, _sc, _ka, _kd, _ks, _pc),
          p1(_p1), p2(_p2), p3(_p3), p4(_p4) {}
};



/* SHAPES */



// Sphere shape class

class Sphere : public hittable {
public:

    Sphere(Vector3f _center, double _radius) : center(_center), radius(_radius) 
    {
        auto rvec = Vector3f(radius, radius, radius);
        bbox = aabb(center - rvec, center + rvec);
    }

    aabb bounding_box() const override { 
        return bbox; 
    }

    bool hit(const Ray& r, interval ray_t, hit_record& rec) const override {
        Vector3f oc = r.origin() - center;
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
        Vector3f outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

private:
    Vector3f center;
    double radius;
    aabb bbox;
};



// QUAD shape class

class quad : public hittable {
  public:
    quad(const Vector3f& _Q, const Vector3f& _u, const Vector3f& _v)
      : Q(_Q), u(_u), v(_v)
    {
        auto n = u.cross(v);
        normal = n.normalized();
        D = normal.dot(Q);
        w = n / n.dot(n);

        set_bounding_box();
    }

    virtual void set_bounding_box() {
        bbox = aabb(Q, Q + u + v).pad();
    }

    aabb bounding_box() const override { return bbox; }

    bool hit(const Ray& r, interval ray_t, hit_record& rec) const override {
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
        Vector3f planar_hitpt_vector = intersection - Q;
        auto alpha = w.dot(planar_hitpt_vector.cross(v));
        auto beta = w.dot(u.cross(planar_hitpt_vector));

        if (!is_interior(alpha, beta, rec))
            return false;

        // Ray hits the 2D shape; set the rest of the hit record and return true.

        rec.t = t;
        rec.p = intersection;
        rec.set_face_normal(r, normal);

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
    Vector3f Q;
    Vector3f u, v;
    aabb bbox;
    Vector3f normal;
    double D;
    Vector3f w;
};