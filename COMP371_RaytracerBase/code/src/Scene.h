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

    vector<sphere*> spheres;
    vector<rectangle*> rectangles;
    vector<Geometry*> geometries;
    vector<Light*> lights;
    vector<Output*> outputs;

    ~Scene() 
    {
        for (auto var : spheres)
        {
            delete var;
        }
        spheres.clear();

        for (auto var : rectangles)
        {
            delete var;
        }
        rectangles.clear();

        for (auto var : geometries)
        {
            delete var;
        }
        geometries.clear();

        for (auto var : lights)
        {
            delete var;
        }
        lights.clear();

        for (auto var : outputs)
        {
            delete var;
        }
        outputs.clear();
    };
    Scene()
    {
        rectangles = vector<rectangle*>();
        spheres = vector<sphere*>();
        geometries = vector<Geometry*>();
        lights = vector<Light*>();
        outputs = vector<Output*>();
    }
};