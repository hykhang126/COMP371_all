#pragma once

#include "SceneObjs.h"

class Output 
{
public:
    string filename;
    float image_width;
    float image_height;
    Vector3f lookat;
    Vector3f up;
    float fov;
    Vector3f centre;
    Vector3f ai;
    Vector3f bkc;
    bool globalillum;
    bool antialiasing;

    Output(string _filename, float _image_width, float _image_height,
        Vector3f _lookat, Vector3f _up, float _fov,
        Vector3f _centre, Vector3f _ai, Vector3f _bkc, bool _globalillum, bool _antialiasing)
    : filename(_filename), image_width(_image_width), image_height(_image_height),
        lookat(_lookat), up(_up), fov(_fov),
        centre(_centre), ai(_ai), bkc(_bkc), globalillum(_globalillum), antialiasing(_antialiasing) {}
};