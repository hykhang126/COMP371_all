#pragma once

#include "SceneObjs.h"
#include "Hittable.h"

class camera {
  public:
    double aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width  = 100;  // Rendered image width in pixel count
    int    image_height;   // Rendered image height
    Vector3f center;         // Camera center
    Vector3f pixel00_loc;    // Location of pixel 0, 0
    Vector3f pixel_delta_u;  // Offset to pixel to the right
    Vector3f pixel_delta_v;  // Offset to pixel below

    // Default render function
    void render(const hittable& world) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; ++j) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) {
                auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
                auto ray_direction = pixel_center - center;
                Ray r(center, ray_direction);

                color pixel_color = ray_color(r, world);
                // write_color(std::cout, pixel_color);
            }
        }

        std::clog << "\rDone.                 \n";
    }

    // Default Initialization for camera

    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = Vector3f(0, 0, 0);

        // Determine viewport dimensions.
        auto focal_length = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (static_cast<double>(image_width)/image_height);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        auto viewport_u = Vector3f(viewport_width, 0, 0);
        auto viewport_v = Vector3f(0, -viewport_height, 0);

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left =
            center - Vector3f(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    // Default ray_color function

    color ray_color(const Ray& r, const hittable& world) const {
        hit_record rec;

        if (world.hit(r, interval(0, infinity), rec, 0)) {
            return 0.5 * (rec.normal + color(1,1,1));
        }

        Vector3f unit_direction = r.direction().normalized();
        auto a = 0.5*(unit_direction.y() + 1.0);
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    }

    // My version of Initialzation
    void initialize(Vector3f centre, double dimx, double dimy)
    {
        // Camera
        image_width = dimx;
        image_height = dimy;
        aspect_ratio = dimx / dimy;

        auto focal_length = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (static_cast<double>(dimx)/dimy);
        this->center = centre;

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        auto viewport_u = Vector3f(viewport_width, 0, 0);
        auto viewport_v = Vector3f(0, -viewport_height, 0);

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / dimx;
        pixel_delta_v = viewport_v / dimy;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center
                                    - Vector3f(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }
};