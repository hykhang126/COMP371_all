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

color RayTracer::ray_color(const Ray& r, const hittable& world, Scene* scene, Output& out)
{
    hit_record rec;
    color color = Vector3f(0,0,0);

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
            Vector3f sur_norm = rec.normal;
            Vector3f ray_org = r.at(rec.t);
            Vector3f ray_dir = light->centre - ray_org;
            Ray light_ray = Ray(ray_org, ray_dir);

            // color = color + BlinnPhongShader(r, rec, *light, out, *g);

            // if (light_ray.direction().dot(sur_norm) < 0)
            // {
            //     // std::cout << "Light ray behind object" << endl;
            //     return Vector3f(0, 0, 0);
            // }


            // Attempt at shadow in direct illum
            if (!world.hit(light_ray, interval(0, infinity), light_rec, rec.hit_index)) 
            {
                color = color + BlinnPhongShader(r, rec, *light, out, *g);
            }
            else
            {
                Vector3f(0, 0, 0);
            }
        }
        
        color[0] = clip(color.x(), 0.0f, 1.0f);
        color[1] = clip(color.y(), 0.0f, 1.0f);
        color[2] = clip(color.z(), 0.0f, 1.0f);

        g->use = true;

        return color;
    }

    return out.bkc;
};

color RayTracer::ray_color_global_illum(const Ray& r, Output& out, const hittable& world, int depth, Scene* scene)
{
    hit_record rec;
    Vector3f color;

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
    Vector3f BRDF = BlinnPhongShader(r, rec, *scene->lights.at(0), out, *scene->geometries.at(rec.hit_index));

    // The cosine of the angle between the normal and the new ray direction
    float cos_theta = newRay.direction().dot(rec.normal);

    // Make the next recursive globalIllum call
    Vector3f incoming_light = ray_color_global_illum(newRay, out, world, depth-1, scene);

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
