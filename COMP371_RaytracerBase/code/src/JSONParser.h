#include "../external/json.hpp"
#include "SceneObjs.h"
#include "Scene.h"

using namespace std;
using namespace nlohmann;

class JSONParser
{

public:

    /* FUNCTIONS */

    JSONParser() {};

    JSONParser(json* j, Scene* scene, hittable_list* hit_list)
    {
        // Extract info
        j = new json();

        // Create the scene
        scene = new Scene();

        // Create the hit list
        hit_list = new hittable_list();
    };

    ~JSONParser() {};

    void process_json(json& j, Scene& scene, hittable_list& hit_list)
    {
        // 1 - parse geometry
        parse_geometry(j, scene, hit_list);
        
        // 2 - parse lights
        parse_lights(j, scene);
        
        // 3 - parse lights
        parse_output(j, scene);
    }

    bool parse_geometry(json &j, Scene& scene, hittable_list& hit_list)
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

                scene.geometries.push_back(g);
                scene.spheres.push_back(g);

                hit_list.add(make_shared<Sphere>(g->centre, g->radius));
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

                scene.geometries.push_back(g);
                scene.rectangles.push_back(g);

                auto P = g->p1;
                auto U = g->p2 - P;
                auto V = g->p4 - P;
                hit_list.add(make_shared<quad>(P, U, V));
            }

            ++gc;
        }

        cout<<"We have: "<<gc<<" geometry objects!"<<endl;

        return true;
    }

    bool parse_lights(json &j, Scene& scene)
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
            scene.lights.push_back(l);
            ++lc;
        }
        
        cout<<"We have: "<<lc<<" light objects!"<<endl;
        
        return true;
    }

    bool parse_output(json &j, Scene& scene)
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
            float fov = (*itr)["fov"].get<float>();
            bool antialiasing = false;
            bool globalillum = false;

            if (((*itr)["antialiasing"]).is_boolean())
            {
                antialiasing = (*itr)["antialiasing"].get<bool>();
            }
            if (((*itr)["globalillum"]).is_boolean())
            {
                globalillum = (*itr)["globalillum"].get<bool>();
            }
            
            cout<<"Filename: "<<filename<<endl;
            cout<<"FOV: "<<fov<<endl;

            Vector3f lookat((*itr)["lookat"][0], (*itr)["lookat"][1], (*itr)["lookat"][2]);
            Vector3f up((*itr)["up"][0], (*itr)["up"][1], (*itr)["up"][2]);
            Vector3f ai((*itr)["ai"][0], (*itr)["ai"][1], (*itr)["ai"][2]);
            Vector3f bkc((*itr)["bkc"][0], (*itr)["bkc"][1], (*itr)["bkc"][2]);
            Output* o = new Output(filename, size[0], size[1], lookat, up, fov, centre, ai, bkc, 
                                globalillum, antialiasing);
            scene.outputs.push_back(o);
            
            ++lc;
        }
        
        cout<<"We have: "<<lc<<" output objects!"<<endl;
        return true;
    }
};
