#include "RayTracer.h"

// #define TEST

RayTracer::RayTracer(json& j) : j(j)
{
    // Extract info
    this->j = j;

    // Create the scene
    scene = new Scene();

    // Create the hit list
    hit_list = new hittable_list();
}

RayTracer::~RayTracer()
{
    j.~basic_json();
    delete scene;
    delete hit_list;
}

void RayTracer::run()
{
    process_json(j);

    // Run save to ppm file?
    for (Output * out : scene->outputs)
    {
        process_ppm(*out);
    }
    

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
        Vector3f ac(geom["ac"][0], geom["ac"][1], geom["ac"][2]);
        Vector3f dc(geom["dc"][0], geom["dc"][1], geom["dc"][2]);
        Vector3f sc(geom["sc"][0], geom["sc"][1], geom["sc"][2]);

        if (geom["type"] == "sphere")
        {
            sphere* g;
            Vector3f centre(geom["centre"][0], geom["centre"][1], geom["centre"][2]);
            g = new sphere(geom["type"], gc, 
                        ac, dc, sc, geom["ka"], geom["kd"], geom["ks"], geom["pc"], 
                        centre, geom["radius"]);

            scene->geometries.push_back(g);
            scene->spheres.push_back(g);

            hit_list->add(make_shared<Sphere>(g->centre, g->radius));
        }
        else if (geom["type"] == "rectangle")
        {
            rectangle* g;
            Vector3f p1(geom["p1"][0], geom["p1"][1], geom["p1"][2]);
            Vector3f p2(geom["p2"][0], geom["p2"][1], geom["p2"][2]);
            Vector3f p3(geom["p3"][0], geom["p3"][1], geom["p3"][2]);
            Vector3f p4(geom["p4"][0], geom["p4"][1], geom["p4"][2]);
            g = new rectangle(geom["type"], gc,
                        ac, dc, sc, geom["ka"], geom["kd"], geom["ks"], geom["pc"],
                        p1, p2, p3, p4);

            scene->geometries.push_back(g);
            scene->rectangles.push_back(g);

            auto P = g->p1;
            auto U = g->p2 - P;
            auto V = g->p4 - P;
            hit_list->add(make_shared<quad>(P, U, V));
        }

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
        cout<<"FOV: "<<fov<<endl;

        Vector3f lookat((*itr)["lookat"][0], (*itr)["lookat"][1], (*itr)["lookat"][2]);
        Vector3f up((*itr)["up"][0], (*itr)["up"][1], (*itr)["up"][2]);
        Vector3f ai((*itr)["ai"][0], (*itr)["ai"][1], (*itr)["ai"][2]);
        Vector3f bkc((*itr)["bkc"][0], (*itr)["bkc"][1], (*itr)["bkc"][2]);
        Output* o = new Output(filename, size[0], size[1], lookat, up, fov, centre, ai, bkc);
        scene->outputs.push_back(o);
        
        ++lc;
    }
    
    cout<<"We have: "<<lc<<" output objects!"<<endl;
    return true;
}



/* -------------------------------------------------------- */



color BlinnPhongShader(const Ray& r, const hit_record& rec, Light& light,
    Output& out, Geometry& geometry)
{
    Vector3f color;

    // Scene reflection components
    Vector3f ai = out.ai;

    // Light reflection components
    Vector3f id = light.id;
    Vector3f is = light.is;

    // Geometric reflection components
    Vector3f ac = geometry.ac;
    Vector3f dc = geometry.dc;
    Vector3f sc = geometry.sc;
    float ka = geometry.ka;
    float kd = geometry.kd;
    float ks = geometry.ks;
    float pc = geometry.pc;

    // get the light direction
    Vector3f l = light.centre - rec.p;

    // get the view vector
    Vector3f v = -1 * r.direction();

    // get the Normal
    Vector3f n = rec.normal;

    // get reflection direction
    Vector3f reflection = 2 * l.dot(n) * n - l;

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

    return color;
}

color ray_color(const Ray& r, const hittable& world, Scene* scene, Output& out)
{
    hit_record rec;
    color color;

    if (world.hit(r, interval(0, infinity), rec)) {
        // return the obj at rec.hit_index
        Geometry* g = scene->geometries.at(rec.hit_index);

#ifdef TEST
        std::cout << rec.hit_index << endl;
#endif

        for (auto light : scene->lights)
        {
            hit_record light_rec;
            Vector3f ray_org = rec.p;
            Vector3f ray_dir = light->centre - ray_org;
            Ray ray = Ray(ray_org, ray_dir);

#ifdef TEST
            if (rec.hit_index == 1) {
                cout << g->ac << endl << g->dc <<endl << g->sc <<endl;
            };
#endif

            // color = color + BlinnPhongShader(r, rec, *light, out, *g);

            if (!world.hit(ray, interval(rec.t, infinity), light_rec)) 
            {
                color = color + BlinnPhongShader(r, rec, *light, out, *g);
            }
            else
            {
                return Vector3f(0.0f, 0.0f, 0.0f);
            }
        }
        
        color[0] = clip(color.x(), 0.0f, 1.0f);
        color[1] = clip(color.y(), 0.0f, 1.0f);
        color[2] = clip(color.z(), 0.0f, 1.0f);
        return color;
    }

    return out.bkc;
}



/* -------------------------------------------------------- */



void write_color(color& out, color pixel_color, int samples_per_pixel) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples.
    auto scale = 1.0 / samples_per_pixel;
    r *= scale;
    g *= scale;
    b *= scale;

    // Write the translated [0,255] value of each color component.
    static const interval intensity(0.000, 1.0);
    out[0] = intensity.clamp(r);
    out[1] = intensity.clamp(g);
    out[2] = intensity.clamp(b);
}

void RayTracer::process_ppm(Output& out)
{
    int dimx = out.image_width;
    int dimy = out.image_height;

    camera cam;
    cam.initialize(out.centre, dimx, dimy, out.fov);

    // Buffer to send to ppm file
    vector<double>* buffer = new vector<double>(3*(dimx*dimy + dimx));

    for (int j = 0; j < dimy; ++j) {
        for (int i = 0; i < dimx; ++i) {
            color pixel_color(0, 0, 0);
            for (int sample = 0; sample < cam.samples_per_pixel; ++sample) {
                Ray r = cam.get_ray(i, j);
                pixel_color += ray_color(r, *hit_list, scene, out);
            }
            
            color out;
            write_color(out, pixel_color, cam.samples_per_pixel);
            buffer->at(3*j*dimx+3*i+0)= out.x();
            buffer->at(3*j*dimx+3*i+1)= out.y();
            buffer->at(3*j*dimx+3*i+2)= out.z();
        }
    }

    save_ppm(out.filename, *buffer, dimx, dimy);
    cout << out.filename << " save succesfully in Raytracer.cpp" <<endl;

    delete buffer;
}
