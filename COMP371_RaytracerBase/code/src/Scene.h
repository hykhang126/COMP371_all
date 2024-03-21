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
};