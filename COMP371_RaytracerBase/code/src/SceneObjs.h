#pragma once

#include <vector>
#include <string>
#include <Eigen/Core>

using Eigen::Vector3f;
using std::string;
using std::vector;

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

class Light 
{
public:
    string type;
    Vector3f centre;
    Vector3f id;
    Vector3f is;

    Light(string _type, Vector3f _centre, Vector3f _id, Vector3f _is)
    : type(_type), centre(_centre), id(_id), is(_is) {}
};

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

    Output(string _filename, float _image_width, float _image_height,
        Vector3f _lookat, Vector3f _up, float _fov,
        Vector3f _centre, Vector3f _ai, Vector3f _bkc)
    : filename(_filename), image_width(_image_width), image_height(_image_height),
        lookat(_lookat), up(_up), fov(_fov),
        centre(_centre), ai(_ai), bkc(_bkc) {}
};

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
