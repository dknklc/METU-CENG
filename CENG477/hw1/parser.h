#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>
#include "math.h"

namespace parser
{
    //Notice that all the structures are as simple as possible
    //so that you are not enforced to adopt any style or design.
    struct Vec3f
    {
        float x, y, z;
        Vec3f operator*(float rhs) const {
            //Vec3f result{rhs*x, rhs*y , rhs*z};
            return Vec3f{rhs*x, rhs*y , rhs*z};
        }

        Vec3f operator/(float rhs) const {
            //Vec3f result{rhs*x, rhs*y , rhs*z};
            return Vec3f{x / rhs, y/rhs , z / rhs};
        }

        Vec3f operator+(Vec3f rhs) const {
            return Vec3f{x + rhs.x , + y + rhs.y , z + rhs.z};
        }

        Vec3f operator-(Vec3f rhs) const {
            return Vec3f{x - rhs.x, y - rhs.y, z - rhs.z};
        }

        static float dotProduct(Vec3f a, Vec3f b){
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }

        static Vec3f crossProduct(Vec3f a, Vec3f b) {
            Vec3f result{0,0,0};
            result.x = a.y * b.z - a.z * b.y;
            result.y = a.z * b.x - a.x * b.z;
            result.z = a.x * b.y - a.y * b.x;

            return result;
        }

        static Vec3f normalize(Vec3f a)
        {
            return a / sqrt(dotProduct(a,a));
        }

    };

    Vec3f operator*(float lhs, Vec3f rhs);

    struct Vec3i
    {
        int x, y, z;
    };

    struct Vec4f
    {
        float x, y, z, w;
    };

    struct Camera
    {
        Vec3f position;
        Vec3f gaze;
        Vec3f up;
        Vec4f near_plane;
        float near_distance;
        int image_width, image_height;
        std::string image_name;

    };

    struct Ray{
        Vec3f o;
        Vec3f d;
    };


    struct PointLight
    {
        Vec3f position;
        Vec3f intensity;
    };

    struct Material
    {
        bool is_mirror;
        Vec3f ambient;
        Vec3f diffuse;
        Vec3f specular;
        Vec3f mirror;
        float phong_exponent;
    };

    struct Face
    {
        int v0_id;
        int v1_id;
        int v2_id;
    };

    struct Mesh
    {
        int material_id;
        std::vector<Face> faces;
    };

    struct Triangle
    {
        int material_id;
        Face indices;
    };

    struct Sphere
    {
        int material_id;
        int center_vertex_id;
        float radius;
    };

    struct Vec3fTriangle{
        Vec3f v1;
        Vec3f v2;
        Vec3f v3;
    };

    struct Scene
    {
        //Data
        Vec3i background_color;
        float shadow_ray_epsilon;
        int max_recursion_depth;
        std::vector<Camera> cameras;
        Vec3f ambient_light;
        std::vector<PointLight> point_lights;
        std::vector<Material> materials;
        std::vector<Vec3f> vertex_data;
        std::vector<Mesh> meshes;
        std::vector<Triangle> triangles;
        std::vector<Sphere> spheres;

        //Functions
        void loadFromXml(const std::string &filepath);


    };
}

#endif
