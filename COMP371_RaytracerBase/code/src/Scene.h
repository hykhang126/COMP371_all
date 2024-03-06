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
    vector<Light*> lights;
    vector<Output*> outputs;

    ~Scene() = default;
    Scene()
    {
        geometries = vector<Geometry*>();
        lights = vector<Light*>();
        outputs = vector<Output*>();
    }
};