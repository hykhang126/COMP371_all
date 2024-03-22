#pragma once

#include "SceneObjs.h"

class Output 
{
public:
    string filename;
    double image_width;
    double image_height;
    Vector3d lookat;
    Vector3d up;
    double fov;
    Vector3d centre;
    Vector3d ai;
    Vector3d bkc;
    bool globalillum;
    bool antialiasing;
    int raysperpixel;
    double probterminate = 1 / (2 * pi);
    bool twosiderender = true;
    int maxbounces = 1;

    Output(string _filename, double _image_width, double _image_height,
        Vector3d _lookat, Vector3d _up, double _fov,
        Vector3d _centre, Vector3d _ai, Vector3d _bkc, bool _globalillum, bool _antialiasing, int _raysperpixel)
    : filename(_filename), image_width(_image_width), image_height(_image_height),
        lookat(_lookat), up(_up), fov(_fov),
        centre(_centre), ai(_ai), bkc(_bkc), globalillum(_globalillum), antialiasing(_antialiasing), raysperpixel(_raysperpixel) {}
};