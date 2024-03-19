#pragma once

#include "SceneObjs.h"
#include "Hittable.h"

class Geometry 
{
public:
    string type;
    Vector3f centre;
    float radius;
    Vector3f ac;
    Vector3f dc;
    Vector3f sc;
    float ka;
    float kd;
    float ks;
    int pc;

    Geometry(string _type, Vector3f _centre, float _radius,
            Vector3f _ac, Vector3f _dc, Vector3f _sc,
            float _ka, float _kd, float _ks,
            int _pc)
    : type(_type), centre(_centre), radius(_radius),
        ac(_ac), dc(_dc), sc(_sc),
        ka(_ka), kd(_kd), ks(_ks),
        pc(_pc) {}
};

class Sphere : public hittable {
  public:
        // Sphere(string _type, Vector3f _centre, float _radius,
        //         Vector3f _ac, Vector3f _dc, Vector3f _sc,
        //         float _ka, float _kd, float _ks,
        //         int _pc)
        // : Geometry(_type, _centre, _radius, _ac, _dc, _sc, _ka, _kd, _ks, _pc) {};

        Sphere(Vector3f _center, double _radius) : center(_center), radius(_radius) {}

    bool hit(const Ray& r, interval ray_t, hit_record& rec, int index) const override {
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

        rec.index = 0;

        return true;
    }

  private:
    Vector3f center;
    double radius;
};