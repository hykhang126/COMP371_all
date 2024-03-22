#pragma once

#include "SceneObjs.h"

class Light 
{
public:
    string type;
    Vector3d centre;
    Vector3d id;
    Vector3d is;
    int n = 1;
    bool usecenter = true;
    bool visible = true;

    Light(string _type, Vector3d _centre, Vector3d _id, Vector3d _is)
    : type(_type), centre(_centre), id(_id), is(_is) {}
};