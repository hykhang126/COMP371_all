#pragma once

#include "SceneObjs.h"

class Geometry 
{
public:
    string type;
    Vector3f centre;
    float radius;
    Vector3f ac;
    Vector3f dc;
    Vector3f sc;
    float ka;
    float kd;
    float ks;
    int pc;

    Geometry(string _type, Vector3f _centre, float _radius,
            Vector3f _ac, Vector3f _dc, Vector3f _sc,
            float _ka, float _kd, float _ks,
            int _pc)
    : type(_type), centre(_centre), radius(_radius),
        ac(_ac), dc(_dc), sc(_sc),
        ka(_ka), kd(_kd), ks(_ks),
        pc(_pc) {}
};