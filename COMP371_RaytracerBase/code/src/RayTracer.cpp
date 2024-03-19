#include "RayTracer.h"

template <typename T>
constexpr const T& myClamp(const T& value, const T& low, const T& high) {
    return (value < low) ? low : ((high < value) ? high : value);
}

int hitCount = 0;

// The Blinn Phong shader
color BlinnPhongShader(const Ray& r, const hit_record& rec, Scene* scene)
{
    Vector3f color;

    // Scene reflection components
    Vector3f ai = scene->outputs[0]->ai;

    // Light reflection components
    Vector3f id = scene->lights[0]->id;
    Vector3f is = scene->lights[0]->is;

    // Geometric reflection components
    Vector3f ac = scene->geometries[0]->ac;
    Vector3f dc = scene->geometries[0]->dc;
    Vector3f sc = scene->geometries[0]->sc;
    float ka = scene->geometries[0]->ka;
    float kd = scene->geometries[0]->kd;
    float ks = scene->geometries[0]->ks;
    float pc = scene->geometries[0]->pc;

    // get the light direction
    Vector3f l = scene->lights[0]->centre - rec.p;

    // get the view vector
    Vector3f v = scene->outputs[0]->lookat;

    // get the Normal
    Vector3f n = scene->geometries[0]->centre - rec.p;

    // get reflection direction
    Vector3f reflection = rec.p - 2*rec.normal.cross(n);

    // Normalization
    l = l.normalized();
    v = v.normalized();
    n = n.normalized();
    reflection = reflection.normalized();

    // Ambient
    Vector3f ambient = ai.cwiseProduct(ac) * ka;

    // Diffuse 
    Vector3f diffuse = id.cwiseProduct(dc) * kd * max(n.dot(l), 0.0f);

    // Specular
    Vector3f specular = is.cwiseProduct(sc) * ks * pow(max(reflection.dot(v), 0.0f), pc);

    color = ambient + diffuse + specular;

    // myClamp(color[0], 0.0f, 1.0f);
    // myClamp(color[1], 0.0f, 1.0f);
    // myClamp(color[2], 0.0f, 1.0f);
    
    return color;
}

// color ray_color(const Ray& r, const hittable& world, Scene* scene) 
// {
//     hit_record rec;
//     if (world.hit(r, interval(0, infinity), rec)) {
//         return BlinnPhongShader(r, rec, scene);
//     }

//     return scene->outputs[0]->bkc;
// }

color ray_color(const Ray& r, const hittable& world) {
    hit_record rec;

    if (world.hit(r, interval(0, infinity), rec)) {
        hitCount++;
        return 0.5 * (rec.normal + color(1,1,1));
    }

    Vector3f unit_direction = r.direction().normalized();
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

/* -------------------------------------------------------- */

RayTracer::RayTracer(json& j) : j(j)
{
    // Extract info
    this->j = j;

    // Create the scene
    scene = new Scene();
}

RayTracer::~RayTracer()
{
    j.~basic_json();
    delete scene;
}

void RayTracer::run()
{
    process_json(j);

    // Run save to ppm file?
    process_ppm(*scene->outputs[0]);

}

void RayTracer::process_json(json &j)
{
    // 1 - parse geometry
    parse_geometry(j);
    
    // 2 - parse lights
    parse_lights(j);
    
    // 3 - parse lights
    parse_output(j);
}

bool RayTracer::parse_geometry(json &j)
{
    cout<<"Geometry: "<<endl;
    int gc = 0;
    
    // Parse geometry
    for (const auto& geom : j["geometry"]) 
    {
        Vector3f centre(geom["centre"][0], geom["centre"][1], geom["centre"][2]);
        Vector3f ac(geom["ac"][0], geom["ac"][1], geom["ac"][2]);
        Vector3f dc(geom["dc"][0], geom["dc"][1], geom["dc"][2]);
        Vector3f sc(geom["sc"][0], geom["sc"][1], geom["sc"][2]);
        Geometry* g = new Geometry(geom["type"], centre, geom["radius"], ac, dc, sc,
                   geom["ka"], geom["kd"], geom["ks"], geom["pc"]);
        scene->geometries.push_back(g);
        ++gc;
    }

    cout<<"We have: "<<gc<<" geometry objects!"<<endl;

    return true;
}

bool RayTracer::parse_lights(json &j)
{
    cout<<"Light: "<<endl;
    int lc = 0;
    
    // Parse lights
    for (const auto& light : j["light"]) 
    {
        Vector3f centre(light["centre"][0], light["centre"][1], light["centre"][2]);
        Vector3f id(light["id"][0], light["id"][1], light["id"][2]);
        Vector3f is(light["is"][0], light["is"][1], light["is"][2]);
        Light* l = new Light(light["type"], centre, id, is);
        scene->lights.push_back(l);
        ++lc;
    }
    
    cout<<"We have: "<<lc<<" light objects!"<<endl;
    
    return true;
}

bool RayTracer::parse_output(json &j)
{
    cout<<"Outputs: "<<endl;
    int lc = 0;
    
    // use iterators to read-in array types
    for (auto itr = j["output"].begin(); itr!= j["output"].end(); itr++){
        
        string filename;
        if(itr->contains("filename")){
          //  filename = static_cast<std::string>((*itr)["filename"]);
            filename = (*itr)["filename"].get<string>();
        } else {
            cout<<"Fatal error: output shoudl always contain a filename!!!"<<endl;
            return false;
        }
        
        
        int size[2];
        int i = 0;
        for (auto itr2 =(*itr)["size"].begin(); itr2!= (*itr)["size"].end(); itr2++){
            if(i<2){
                size[i++] = (*itr2).get<float>();
            } else {
                cout<<"Warning: Too many entries in size"<<endl;
            }
        }
        
        Vector3f centre(0,0,0);
        
         i = 0;
        for (auto itr2 =(*itr)["centre"].begin(); itr2!= (*itr)["centre"].end(); itr2++){
            if(i<3){
                centre[i++] = (*itr2).get<float>();
            } else {
                cout<<"Warning: Too many entries in centre"<<endl;
            }
        }
        
        // Similarly to the centre array you need to read the lookat and up
        //Maybe create a separate functiomn to read arrays - ythey are pretty common
        
        
        // I am retrieving the field of view
        // this is mandatory field here, but if I dont check if it exists,
        // the code will throw an exception which if not caught will end the execution of yoru program
        cout<<"-Raytracer.cpp getting FOV"<<endl;
        float fov = (*itr)["fov"].get<float>();
        
        cout<<"Filename: "<<filename<<endl;
        cout<<"Camera centre: "<<centre<<endl;
        cout<<"FOV: "<<fov<<endl;

        Vector3f lookat((*itr)["lookat"][0], (*itr)["lookat"][1], (*itr)["lookat"][2]);
        Vector3f up((*itr)["up"][0], (*itr)["up"][1], (*itr)["up"][2]);
        Vector3f ai((*itr)["ai"][0], (*itr)["ai"][1], (*itr)["ai"][2]);
        Vector3f bkc((*itr)["bkc"][0], (*itr)["bkc"][1], (*itr)["bkc"][2]);
        Output* o = new Output(filename, size[0], size[1], lookat, up, fov, centre, ai, bkc);
        scene->outputs.push_back(o);
        
        ++lc;
    }
    
    cout<<"We have: "<<lc<<" objects!"<<endl;
    return true;
}

void RayTracer::process_ppm(Output& out)
{
    int dimx = out.image_width;
    int dimy = out.image_height;

    // World

    hittable_list world;
    world.add(make_shared<Sphere>(scene->geometries[0]->centre, scene->geometries[0]->radius));

    // Camera
    camera cam;
    cam.initialize(out.centre, dimx, dimy);

    // Buffer to send to ppm file
    std::vector<double> buffer(3*(dimx*dimy));

    // Render is still in RayTracer.cpp
    for (int j = 0; j < cam.image_height; ++j) {
        for (int i = 0; i < cam.image_width; ++i) {
            auto pixel_center = cam.pixel00_loc + (i * cam.pixel_delta_u) + (j * cam.pixel_delta_v);
            auto ray_direction = pixel_center - cam.center;
            Ray r(cam.center, ray_direction);

            // color pixel_color = ray_color(r, world, scene);
            color pixel_color = ray_color(r, world);
            // cout << pixel_color << endl;
            buffer.at(3*j*cam.image_height+3*i+0)= pixel_color[0];
            buffer.at(3*j*cam.image_height+3*i+1)= pixel_color[1];
            buffer.at(3*j*cam.image_height+3*i+2)= pixel_color[2];
        }
    }

    save_ppm(out.filename, buffer, dimx, dimy);
    cout << out.filename << " save succesfully in Raytracer.cpp" <<endl;

    // delete buffer;
}
