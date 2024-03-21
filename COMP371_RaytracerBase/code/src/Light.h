#pragma once

#include "SceneObjs.h"

class Light 
{
public:
    string type;
    Vector3f centre;
    Vector3f id;
    Vector3f is;
    int n = 1;
    bool usecenter = true;
    bool visible = true;

    Light(string _type, Vector3f _centre, Vector3f _id, Vector3f _is)
    : type(_type), centre(_centre), id(_id), is(_is) {}
};