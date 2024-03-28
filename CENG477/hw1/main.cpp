/*
#include <iostream>
#include "parser.h"
#include "ppm.h"
#include <cmath>

using namespace parser;
using namespace std;

enum TypeOfObjects {
    Sphere,
    Face
};

Ray generateRay(int i, int j, Camera &camera, Vec3f u){
    Ray result;
    float su,sv;
    Vec3f m,q,s;

    su = (i + 0.5)*(camera.near_plane.y - camera.near_plane.x)/camera.image_width;
    sv = (j + 0.5)*(camera.near_plane.w - camera.near_plane.z)/camera.image_height;

    Vec3f gaze = camera.gaze.normalize(camera.gaze);
    m = camera.position + (gaze * camera.near_distance);

    q = m + (u * camera.near_plane.x) + (camera.up * camera.near_plane.w);
    s = q + (u * su) + (camera.up * -sv);

    result.o = camera.position;
    result.d = s - camera.position;

    return result;
}

float intersectSphere(Ray r, float radius, Vec3f c)
{
    float A,B,C; //constants for the quadratic equation
    float delta;
    float t,t1,t2;

    C = (r.o.x-c.x)*(r.o.x-c.x)+(r.o.y-c.y)*(r.o.y-c.y)+(r.o.z-c.z)*(r.o.z-c.z) - radius * radius;
    B = 2*r.d.x * (r.o.x-c.x) + 2*r.d.y * (r.o.y-c.y) + 2*r.d.z * (r.o.z-c.z);
    A = r.d.x*r.d.x+r.d.y*r.d.y+r.d.z*r.d.z;

    delta = B*B-4*A*C;

    if (delta < 0) return -1;
    else if (delta == 0)
    {
        t = -B / (2*A);
    }
    else
    {
        delta = sqrt(delta);
        A = 2 * A;
        t1 = (-B + delta) / A;
        t2 = (-B - delta) / A;

        if (t1 < t2) t=t1; else t=t2;
    }
    return t;
}

float calcDeterminant(Vec3f v1, Vec3f v2, Vec3f v3){
     return v1.x * (v2.y * v3.z - v2.z * v3.y) - v1.y * (v2.x * v3.z - v3.x * v2.y) + v1.z * (v2.x * v3.y - v3.x * v2.y);
}

float intersectTriangle(Ray r, Vec3f a, Vec3f b, Vec3f c){
    float detA = calcDeterminant(a - b, a - c , r.d);
    float Beta = calcDeterminant(a - r.o, a - c , r.d) / detA;
    float Gamma = calcDeterminant(a - b, a - r.o, r.d) / detA;
    float T = calcDeterminant(a - b, a - c, a - r.o) / detA;

    if(Beta + Gamma <= 1 && Beta >= 0 && Gamma >= 0 && T >= 1) {
        return T;
    }
    return -1;
}

int main(int argc, char* argv[]) {
    // Sample usage for reading an XML scene file

    parser::Scene scene;
    scene.loadFromXml(argv[1]);
    unsigned char * image;
    for(int i = 0 ; i < scene.cameras.size() ; i++){
        image = new unsigned char[scene.cameras[i].image_width * scene.cameras[i].image_height * 3];
        Vec3f u = parser::Vec3f::crossProduct(scene.cameras[i].up,scene.cameras[i].gaze); // normalized ?
        u = parser::Vec3f::normalize(u);
        for (int y = 0; y < scene.cameras[i].image_height; ++y)
        {
            int currRow = scene.cameras[i].image_width * y;
            for (int x = 0; x < scene.cameras[i].image_width; ++x)
            {
                Ray currRay = generateRay(y, x, scene.cameras[i], u);
                float minT = 90000;
                int materialTypeId;
                int objectId;
                TypeOfObjects object;
                for(int s = 0; s < scene.spheres.size() ; s++){

                    float t = intersectSphere(currRay, scene.spheres[s].radius, scene.vertex_data[scene.spheres[i].center_vertex_id] );
                    if(t < minT && t >= 0){
                        minT = t;
                        materialTypeId = scene.spheres[s].material_id;
                        objectId = s;
                        object =TypeOfObjects::Sphere;
                    }
                }

                for(int tr = 0; tr < scene.triangles.size() ; tr++){

                    Vec3f corner0 = scene.vertex_data[scene.triangles[tr].indices.v0_id];
                    Vec3f corner1 = scene.vertex_data[scene.triangles[tr].indices.v1_id];
                    Vec3f corner2 = scene.vertex_data[scene.triangles[tr].indices.v2_id];
                    float t = intersectTriangle(currRay, corner0, corner1, corner2);
                    if(t < minT && t >= 0){
                        minT = t;
                        materialTypeId = scene.triangles[tr].material_id;
                        objectId = tr;
                        object =TypeOfObjects::Face;
                    }
                }

                for(int iterator = 0 ; iterator < scene.meshes.size() ; iterator++){
                    for(int tr = 0; tr < scene.meshes[iterator].faces.size() ; tr++){
                        // Localication açısından bak !
                        Vec3f corner0 = scene.vertex_data[scene.meshes[iterator].faces[tr].v0_id];
                        Vec3f corner1 = scene.vertex_data[scene.meshes[iterator].faces[tr].v1_id];
                        Vec3f corner2 = scene.vertex_data[scene.meshes[iterator].faces[tr].v2_id];
                        float t = intersectTriangle(currRay, corner0, corner1, corner2);

                        if(t < minT && t >= 0){
                            minT = t;
                            materialTypeId = scene.meshes[iterator].material_id;
                            objectId = tr;
                            object =TypeOfObjects::Face;
                        }
                    }
                }


                int currAddress = (currRow+x) * 3;
                image[currAddress] = scene.materials[materialTypeId].ambient.x * scene.ambient_light.x ;
                image[currAddress+ 1] = scene.materials[materialTypeId].ambient.y * scene.ambient_light.y;
                image[currAddress + 2] = scene.materials[materialTypeId].ambient.z * scene.ambient_light.z;
            }
        }

        write_ppm(scene.cameras[i].image_name.data(),image, scene.cameras[i].image_width, scene.cameras[i].image_height);
        delete[] image;

    }






    Camera camera;
    camera.position = Vec3f{5, 0, 15};
    camera.gaze = Vec3f{0,0,-1};
    camera.up = Vec3f{0, 1, 0};
    camera.near_plane = Vec4f{-4, 4, -2, 2};
    camera.near_distance = 2;
    camera.image_width = 2000;
    camera.image_height = 1000;
    camera.image_name = "simple.ppm";

    Vec3f u = {1, 0, 0};
    for (int y = 0; y < camera.image_height; ++y)
    {
        int currRow = camera.image_width * y;
        for (int x = 0; x < camera.image_width; ++x)
        {
            Ray currRay = generateRay(x, y, camera, u);

            cout << currRay.d.x << " " << currRay.d.y << " " << currRay.d.z << endl;
            int currAddress = (currRow+x) * 3;

        }
    }



    return 0;
}
*/