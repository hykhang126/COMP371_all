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
    delete scene;
    delete hit_list;
}

void RayTracer::run()
{
    JSONParser parser = JSONParser();

    parser.process_json(j, *scene, *hit_list); 

    // Run save to ppm file?
    for (Output * out : scene->outputs)
    {
        process_ppm(*out);
    }
    

}





/* -------------------------------------------------------- */





color RayTracer::BlinnPhongShader(const Ray& r, const hit_record& rec, Light& light,
    Output& out, Geometry& geometry)
{
    Vector3d color;

    // Scene reflection components
    Vector3d ai = out.ai;

    // Light reflection components
    Vector3d id = light.id;
    Vector3d is = light.is;

    // Geometric reflection components
    Vector3d ac = geometry.ac;
    Vector3d dc = geometry.dc;
    Vector3d sc = geometry.sc;
    double ka = geometry.ka;
    double kd = geometry.kd;
    double ks = geometry.ks;
    double pc = geometry.pc;

    // get the light direction
    Vector3d l = light.centre - rec.p;

    // get the view vector
    Vector3d v = -1 * r.direction();

    // get the Normal
    Vector3d n = rec.normal;

    // get reflection direction
    Vector3d reflection = 2 * l.dot(n) * n - l;

    // Normalization
    l = l.normalized();
    v = v.normalized();
    n = n.normalized();
    reflection = reflection.normalized();

    // Ambient
    Vector3d ambient = ai.cwiseProduct(ac) * ka;

    // Diffuse 
    Vector3d diffuse = id.cwiseProduct(dc) * kd * max(n.dot(l), 0.0);

    // Specular
    Vector3d specular = is.cwiseProduct(sc) * ks * pow(max(reflection.dot(v), 0.0), pc);

    color = ambient + diffuse + specular;

    return color;
}

color RayTracer::ray_color(const Ray& r, const hittable& world, Scene* scene, Output& out)
{
    hit_record rec;
    color color = Vector3d(0,0,0);

    if (world.hit(r, interval(0, infinity), rec, -1)) {
        // return the obj at rec.hit_index
        Geometry* g = scene->geometries.at(rec.hit_index);
        g->use = false;
        

#ifdef TEST
        std::cout << rec.hit_index << endl;
        if (rec.hit_index == 1) {
            cout << g->ac << endl << g->dc <<endl << g->sc <<endl;
        };
#endif

        for (auto light : scene->lights)
        {
            hit_record light_rec;
            Vector3d sur_norm = rec.normal;
            Vector3d ray_org = r.at(rec.t);
            Vector3d ray_dir = light->centre - ray_org;
            Ray light_ray = Ray(ray_org, ray_dir);

            // color = color + BlinnPhongShader(r, rec, *light, out, *g);

            // if (light_ray.direction().dot(sur_norm) < 0)
            // {
            //     // std::cout << "Light ray behind object" << endl;
            //     return Vector3d(0, 0, 0);
            // }


            // Attempt at shadow in direct illum
            if (!world.hit(light_ray, interval(0, infinity), light_rec, rec.hit_index)) 
            {
                color = color + BlinnPhongShader(r, rec, *light, out, *g);
            }
            else
            {
                Vector3d(0, 0, 0);
            }
        }
        
        color[0] = clip(color.x(), 0.0, 1.0);
        color[1] = clip(color.y(), 0.0, 1.0);
        color[2] = clip(color.z(), 0.0, 1.0);

        g->use = true;

        return color;
    }

    return out.bkc;
};

color RayTracer::ray_color_global_illum(const Ray& r, Output& out, const hittable& world, int depth, Scene* scene)
{
    hit_record rec;
    Vector3d color;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
    {
        // return direct illum instead of black
        return ray_color(r, world, scene, out);
        // return color(0,0,0);
    }

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, interval(0, infinity), rec, -1)) {
        return out.bkc;
    }

    // Pick a random direction from here and keep going.
    Ray newRay = Ray(rec.p, random_in_unit_sphere(rec.normal));

    // Compute BRDF which is our Blinn-Phong shader
    Vector3d BRDF = BlinnPhongShader(r, rec, *scene->lights.at(0), out, *scene->geometries.at(rec.hit_index));

    // The cosine of the angle between the normal and the new ray direction
    double cos_theta = newRay.direction().dot(rec.normal);

    // Make the next recursive globalIllum call
    Vector3d incoming_light = ray_color_global_illum(newRay, out, world, depth-1, scene);

    /* 
    Rendering equation?:
    */
    color = incoming_light.cross( BRDF ) * cos_theta;

    return color;
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
    cam.samples_per_pixel = out.raysperpixel;
    cam.isAAon = out.antialiasing;

    // Buffer to send to ppm file
    vector<double>* buffer = new vector<double>(3*(dimx*dimy + dimx));

    for (int j = 0; j < dimy; ++j) {
        for (int i = 0; i < dimx; ++i) {
            color pixel_color(0, 0, 0);

            if (out.globalillum)
            {
                Ray r = cam.get_ray(i, j);
                for (int sample = 0; sample < cam.samples_per_pixel; ++sample) {
                    Ray r = cam.get_ray(i, j);
                    pixel_color += ray_color_global_illum(r, out, *hit_list, out.maxbounces, scene);
                }
            }
            else 
            {
                for (int sample = 0; sample < cam.samples_per_pixel; ++sample) {
                    Ray r = cam.get_ray(i, j);
                    pixel_color += ray_color(r, *hit_list, scene, out);
                }
            }
            
            color out;
            write_color(out, pixel_color, cam.samples_per_pixel);
            buffer->at(3*j*dimx+3*i+0)= out.x();
            buffer->at(3*j*dimx+3*i+1)= out.y();
            buffer->at(3*j*dimx+3*i+2)= out.z();
        }
    }

    vector<double>& bufferRef = *buffer;

    save_ppm(out.filename, bufferRef, dimx, dimy);
    cout << out.filename << " save succesfully in Raytracer.cpp" <<endl;

    delete buffer;
}
