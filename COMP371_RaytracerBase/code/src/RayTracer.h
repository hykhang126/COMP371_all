#pragma once

#include "../external/json.hpp"
#include "../external/simpleppm.h"
#include "JSONParser.h"
#include "SceneObjs.h"
#include "Scene.h"
#include "Geometry.h"
#include "Light.h"
#include "Output.h"
#include "Hittable.h"
#include "Camera.h"

using namespace std;
using namespace nlohmann;

class RayTracer
{
private:
    json& j;
    Scene* scene;
    hittable_list* hit_list;

public:
    RayTracer(json& j);
    ~RayTracer();
    void run();

    void process_ppm(Output& out);

    color ray_color(const Ray& r, const hittable& world, Scene* scene, Output& out);

    color ray_color_global_illum(const Ray& r, const hittable& world, Scene* scene, Output& out);
};
