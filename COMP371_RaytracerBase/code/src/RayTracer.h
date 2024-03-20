#pragma once

#include "../external/json.hpp"
#include "../external/simpleppm.h"
#include "SceneObjs.h"
#include "Scene.h"
#include "Geometry.h"
#include "Light.h"
#include "Output.h"
#include "Hittable.h"
#include "Camera.h"
#include "BHV.h"

using namespace std;
using namespace nlohmann;

class RayTracer
{
private:
    json& j;
    Scene* scene;

public:
    RayTracer(json& j);
    ~RayTracer();
    void run();

    void process_json(json& j);
    bool parse_geometry(json& j);
    bool parse_lights(json& j);
    bool parse_output(json& j);

    void process_ppm(Output& out);
};
