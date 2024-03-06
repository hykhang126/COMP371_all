#pragma once

#include "../external/json.hpp"
#include "../external/simpleppm.h"
#include "Ray.h"
#include "SceneObjs.h"
#include "Scene.h"

#include <iostream>
#include <string>
#include <Eigen/Core>
#include <Eigen/Dense>

using namespace std;
using namespace nlohmann;
using Eigen::Vector3f;
using std::string;

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

    void process_ppm();
};
