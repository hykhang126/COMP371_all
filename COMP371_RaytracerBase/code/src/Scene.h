#pragma once

#include "SceneObjs.h"
#include "Geometry.h"
#include "Light.h"
#include "Output.h"

class Scene 
{
public:
    static const double ASPECT_RATIO;
    static const double IMAGE_WIDTH;
    static const double IMAGE_HEIGHT;

    vector<Geometry*> geometries;
    vector<sphere*> spheres;
    vector<rectangle*> rectangles;
    vector<Light*> lights;
    vector<Output*> outputs;

    ~Scene() 
    {
        geometries.clear();
        lights.clear();
        outputs.clear();
    };
    Scene()
    {
        geometries = vector<Geometry*>();
        spheres = vector<sphere*>();
        rectangles = vector<rectangle*>();
        lights = vector<Light*>();
        outputs = vector<Output*>();
    }

    
    static void addAreaLight(Scene& scene, Light* l, Vector3d p1, Vector3d p2, Vector3d p3, Vector3d p4)
    {
        Vector3d width = (p2 - p1).normalized();
        Vector3d height = (p4 - p1).normalized();

        for (int i = 0; i < l->n; i++)
        {
            for (size_t j = 0; j < l->n; j++)
            {
                Vector3d center = p1 + width * (i + 0.5) + height * (j + 0.5);
                Light* l2 = new Light(l->type, center, l->id, l->is);
                l2->visible = l->visible;
                l2->n = l->n;
                l2->usecenter = l->usecenter;
                scene.lights.push_back(l2);
            }
        }
        
    }
};