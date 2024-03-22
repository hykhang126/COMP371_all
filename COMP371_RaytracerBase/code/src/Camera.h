#pragma once

#include "SceneObjs.h"
#include "Hittable.h"

class camera {
  public:
    double aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width  = 100;  // Rendered image width in pixel count
    int    image_height;   // Rendered image height
    Vector3d center;         // Camera center
    Vector3d pixel00_loc;    // Location of pixel 0, 0
    Vector3d pixel_delta_u;  // Offset to pixel to the right
    Vector3d pixel_delta_v;  // Offset to pixel below
    int    samples_per_pixel = 1;   // Count of random samples for each pixel
    float vfov = 90;  // Vertical view angle (field of view)
    bool isAAon = false;

    Ray get_ray(int i, int j) const {
        // Get a randomly sampled camera ray for the pixel at location i,j.

        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        Vector3d pixel_sample = pixel_center;
        if (isAAon) pixel_sample = pixel_sample + pixel_sample_square();

        auto ray_origin = center;
        auto ray_direction = pixel_sample - ray_origin;

        return Ray(ray_origin, ray_direction);
    }

    Vector3d pixel_sample_square() const {
        // Returns a random point in the square surrounding a pixel at the origin.
        auto px = -0.5 + random_double();
        auto py = -0.5 + random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    // My version of Initialzation
    void initialize(Vector3d centre, double dimx, double dimy, float fov)
    {
        // Camera
        image_width = dimx;
        image_height = dimy;
        aspect_ratio = dimx / dimy;
        samples_per_pixel = 1;
        vfov = fov;

        auto focal_length = 1.0;
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta/2);
        auto viewport_height = 2 * h * focal_length;
        auto viewport_width = viewport_height * static_cast<double>(aspect_ratio);
        this->center = centre;

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        auto viewport_u = Vector3d(viewport_width, 0, 0);
        auto viewport_v = Vector3d(0, -viewport_height, 0);

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / dimx;
        pixel_delta_v = viewport_v / dimy;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - Vector3d(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }
};