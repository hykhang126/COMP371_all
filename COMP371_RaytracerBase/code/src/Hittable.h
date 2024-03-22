#pragma once

#include "Ray.h"

class hit_record {
  public:
    Vector3d p;
    Vector3d normal;
    double t;
    bool front_face;
    double u;
    double v;

    int hit_index;

    void set_face_normal(const Ray& r, const Vector3d& outward_normal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        front_face = r.direction().dot(outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
  public:
    virtual ~hittable() = default;

    virtual bool hit(const Ray& r, interval ray_t, hit_record& rec, int ignored_index) const = 0;

};

class hittable_list : public hittable {
public:
    vector<shared_ptr<hittable>> objects;

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<hittable> object) {
        objects.push_back(object);
    }

    void remove(vector<shared_ptr<hittable>>::iterator itr) {
        objects.erase(itr);
    }

    bool hit(const Ray& r, interval ray_t, hit_record& rec, int ignored_index) const override {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        int hit_index = 0;
        
        for (const auto& object : objects) {
            if (hit_index == ignored_index) continue;
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec, ignored_index)) {
                
                hit_anything = true;

                closest_so_far = temp_rec.t;

                rec = temp_rec;
                rec.hit_index = hit_index;
            }
            hit_index++;
        }

        return hit_anything;
    }
};