#pragma once

#include "../external/json.hpp"

#include "../external/simpleppm.h"

using namespace std;

class RayTracer
{
private:

public:
    RayTracer(nlohmann::json& j);
    ~RayTracer();
    void run();
};
