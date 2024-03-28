#include <iostream>
#include "parser.h"
#include "ppm.h"

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

    Vec3f gaze = Vec3f::normalize(camera.gaze);
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
    //cout << "inside intersect t : " << t << std::endl;
    return t;
}

float calcDeterminant(Vec3f v1, Vec3f v2, Vec3f v3){
    return v1.x * (v2.y * v3.z - v2.z * v3.y) - v1.y * (v2.x * v3.z - v3.x * v2.z) + v1.z * (v2.x * v3.y - v3.x * v2.y);
}

float intersectTriangle(Ray r, Vec3f a, Vec3f b, Vec3f c){
    float detA = calcDeterminant(a - b, a - c , r.d);
    float Beta = calcDeterminant(a - r.o, a - c , r.d) / detA;
    float Gamma = calcDeterminant(a - b, a - r.o, r.d) / detA;
    float T = calcDeterminant(a - b, a - c, a - r.o) / detA;

    if(Beta + Gamma <= 1 && Beta >= 0 && Gamma >= 0) {
        return T;
    }
    return -1;
}

// check
Vec3f calcIrradiance(const PointLight &light, Vec3f intersectionPoint){
    Vec3f D = light.position - intersectionPoint;
    float D_S = Vec3f::dotProduct(D,D);
    return light.intensity / D_S;
}
// check
Vec3f calcSphereNormal(Vec3f intersectionPoint, Vec3f center){
    return Vec3f::normalize(intersectionPoint - center);
}
// check
Vec3f calcTriangleNormal(Vec3f a, Vec3f b, Vec3f c){
    return Vec3f::normalize(Vec3f::crossProduct((b-a), (c-a)));
}

// check
/* Arguments:
    toLight --> should be normalized before given.
    normal --> could be in both directions, and it should be checked in this function.
    irradiance --> it is already calculated by calcIrradiance function.
*/

Vec3f calcDiffuseComponent(Vec3f toLight, Vec3f normal, Vec3f irradiance, const Material &material){

    Vec3f diffuse{0,0,0};
    float cosAlpha = Vec3f::dotProduct(normal, toLight);
    cosAlpha = cosAlpha >= 0 ? cosAlpha : 0;
    diffuse = irradiance * cosAlpha;
    diffuse.x = diffuse.x * material.diffuse.x;
    diffuse.y = diffuse.y * material.diffuse.y;
    diffuse.z = diffuse.z * material.diffuse.z;

    return diffuse;
}

// check
/* Arguments:
    toLight --> should be normalized before given.
    normal --> could be in both directions, and it should be checked in this function.
    irradiance --> it is already calculated by calcIrradiance function.
    toCamera --> should be normalized before given.
*/

Vec3f calcSpecularComponent(Vec3f toLight, Vec3f normal, Vec3f irradiance, const Material &material, Vec3f toCamera){
    Vec3f specular{0,0,0};
    Vec3f H = Vec3f::normalize(toLight + toCamera);

    float cosBeta = Vec3f::dotProduct(normal,H);
    cosBeta = cosBeta >= 0 ? cosBeta : 0;

    specular = irradiance * pow(cosBeta, material.phong_exponent);
    specular.x = specular.x * material.specular.x;
    specular.y = specular.y * material.specular.y;
    specular.z = specular.z * material.specular.z;

    return specular;


}
Vec3f recursiveRayTracer(const Scene &scene, Ray currRay, int currentRecursionDepth){
        float minT = 90000;
        int materialTypeId;
        int objectId;
        TypeOfObjects objectType;
        Vec3fTriangle triangle = {{0,0,0},{0,0,0},{0,0,0}};
        for(int s = 0; s < scene.spheres.size() ; s++){
            float t = intersectSphere(currRay, scene.spheres[s].radius, scene.vertex_data[scene.spheres[s].center_vertex_id - 1]);

            // on the image plane when t == 1
            if(t < minT && t >= scene.shadow_ray_epsilon){
                minT = t;
                materialTypeId = scene.spheres[s].material_id;
                objectId = s;
                objectType =TypeOfObjects::Sphere;
            }
        }

        for(int tr = 0; tr < scene.triangles.size() ; tr++){

            Vec3f corner0 = scene.vertex_data[scene.triangles[tr].indices.v0_id -1 ];
            Vec3f corner1 = scene.vertex_data[scene.triangles[tr].indices.v1_id -1 ];
            Vec3f corner2 = scene.vertex_data[scene.triangles[tr].indices.v2_id -1 ];
            float t = intersectTriangle(currRay, corner0, corner1, corner2);

            // on the image plane when t == 1
            if(t < minT && t >= scene.shadow_ray_epsilon){
                minT = t;
                materialTypeId = scene.triangles[tr].material_id;
                objectId = tr;
                objectType =TypeOfObjects::Face;
                triangle.v1 = corner0;
                triangle.v2 = corner1;
                triangle.v3 = corner2;
            }
        }

        for(int iterator = 0 ; iterator < scene.meshes.size() ; iterator++){
            for(int tr = 0; tr < scene.meshes[iterator].faces.size() ; tr++){
                // Localication açısından bak !
                Vec3f corner0 = scene.vertex_data[scene.meshes[iterator].faces[tr].v0_id -1];
                Vec3f corner1 = scene.vertex_data[scene.meshes[iterator].faces[tr].v1_id -1];
                Vec3f corner2 = scene.vertex_data[scene.meshes[iterator].faces[tr].v2_id -1];
                float t = intersectTriangle(currRay, corner0, corner1, corner2);

                // on the image plane when t == 1
                if(t < minT && t >= scene.shadow_ray_epsilon){
                    minT = t;
                    materialTypeId = scene.meshes[iterator].material_id;
                    objectId = tr;
                    objectType =TypeOfObjects::Face;
                    triangle.v1 = corner0;
                    triangle.v2 = corner1;
                    triangle.v3 = corner2;
                }
            }
        }

        if(minT < 90000){
            Vec3f pixelColor = {0,0,0};

            Vec3f intersectionPoint = currRay.o + currRay.d * minT;

            Vec3f normalVector;
            if(objectType == TypeOfObjects::Sphere){
                normalVector = calcSphereNormal(intersectionPoint, scene.vertex_data[scene.spheres[objectId].center_vertex_id - 1]);
            }
            else{
                normalVector = calcTriangleNormal(triangle.v1, triangle.v2, triangle.v3);
            }

            Vec3f toCamera = Vec3f::normalize(Vec3f{0,0,0} - currRay.d);

            // Ambient part
            pixelColor.x += scene.materials[materialTypeId - 1].ambient.x * scene.ambient_light.x;
            pixelColor.y += scene.materials[materialTypeId - 1].ambient.y * scene.ambient_light.y;
            pixelColor.z += scene.materials[materialTypeId - 1].ambient.z * scene.ambient_light.z;

            for(int l = 0; l < scene.point_lights.size() ; l++){
                // ************ Shadows need to be considered here **************

                Vec3f toLightVector = scene.point_lights[l].position - intersectionPoint;

                // ************ Shadow CHECK ************

                Ray toLightRay = {intersectionPoint, toLightVector};
                bool shadowIntersectionFound = false;

                for(int s = 0; s < scene.spheres.size() && !shadowIntersectionFound ; s++){
                    float t = intersectSphere(toLightRay, scene.spheres[s].radius, scene.vertex_data[scene.spheres[s].center_vertex_id - 1]);

                    // Epsilon issue
                    if(t > scene.shadow_ray_epsilon && t < 1 ){
                        shadowIntersectionFound = true;
                    }
                }

                for(int tr = 0; tr < scene.triangles.size() && !shadowIntersectionFound ; tr++){

                    Vec3f corner0 = scene.vertex_data[scene.triangles[tr].indices.v0_id -1 ];
                    Vec3f corner1 = scene.vertex_data[scene.triangles[tr].indices.v1_id -1 ];
                    Vec3f corner2 = scene.vertex_data[scene.triangles[tr].indices.v2_id -1 ];
                    float t = intersectTriangle(toLightRay, corner0, corner1, corner2);

                    // Epsilon issue
                    if(t > scene.shadow_ray_epsilon && t < 1 ){
                        shadowIntersectionFound = true;
                    }

                }

                for(int iterator = 0 ; iterator < scene.meshes.size() && !shadowIntersectionFound ; iterator++){
                    for(int tr = 0; tr < scene.meshes[iterator].faces.size() && !shadowIntersectionFound ; tr++){
                        // Localication açısından bak !
                        Vec3f corner0 = scene.vertex_data[scene.meshes[iterator].faces[tr].v0_id -1];
                        Vec3f corner1 = scene.vertex_data[scene.meshes[iterator].faces[tr].v1_id -1];
                        Vec3f corner2 = scene.vertex_data[scene.meshes[iterator].faces[tr].v2_id -1];
                        float t = intersectTriangle(toLightRay, corner0, corner1, corner2);

                        // Epsilon issue
                        if(t > scene.shadow_ray_epsilon && t < 1 ){
                            shadowIntersectionFound = true;
                        }
                    }
                }

                // ************ END of CHECK *************
                if(!shadowIntersectionFound){
                    toLightVector = Vec3f::normalize(toLightVector);


                    Vec3f irradiance = calcIrradiance(scene.point_lights[l],intersectionPoint);


                    // Diffuse part

                    pixelColor = pixelColor + calcDiffuseComponent(toLightVector, normalVector, irradiance,
                                                                   scene.materials[materialTypeId - 1]);

                    // Specular part
                    pixelColor = pixelColor + calcSpecularComponent(toLightVector, normalVector, irradiance,
                                                                    scene.materials[materialTypeId - 1],toCamera);
                }

            }

            // Reflection
            if(currentRecursionDepth + 1 < scene.max_recursion_depth && scene.materials[materialTypeId -1].is_mirror != 0){
                Ray reflectionRay = {intersectionPoint, (normalVector * (Vec3f::dotProduct(normalVector,toCamera) * 2)) - toCamera};

                Vec3f reflectance = recursiveRayTracer(scene,reflectionRay,currentRecursionDepth + 1);

                reflectance.x *= scene.materials[materialTypeId -1].mirror.x;
                reflectance.y *= scene.materials[materialTypeId -1].mirror.y;
                reflectance.z *= scene.materials[materialTypeId -1].mirror.z;

                pixelColor = pixelColor + reflectance;

            }

            // Rounding and Clamping

            /* **************** RECURSION DOES NOT DO ROUNDING AND CLAMPING **********************
            pixelColor = pixelColor + Vec3f{0.5,0.5,0.5};

            if(pixelColor.x > 255) pixelColor.x = 255;
            if(pixelColor.y > 255) pixelColor.y = 255;
            if(pixelColor.z > 255) pixelColor.z = 255;
            */

            return pixelColor;
        }
        else{
            return Vec3f{0,0 ,0};
        }
}





























// ********************************************** MAIN ****************************************************

int main(int argc, char* argv[]) {

    Vec3f v1 = {1,2,3};
    Vec3f v2 = {4,5,6};
    Vec3f v3 = {7,8,9};

    parser::Scene scene;
    scene.loadFromXml(argv[1]);
    unsigned char * image;
    for(int i = 0 ; i < scene.cameras.size() ; i++){
        int imageIterator=0;
        image = new unsigned char[scene.cameras[i].image_width * scene.cameras[i].image_height * 3];
        Vec3f u = parser::Vec3f::crossProduct(scene.cameras[i].gaze, scene.cameras[i].up);
        u = parser::Vec3f::normalize(u);

        for (int y = 0; y < scene.cameras[i].image_height; ++y)
        {

            for (int x = 0; x < scene.cameras[i].image_width; ++x)
            {
                Ray currRay = generateRay(x, y, scene.cameras[i], u);

                float minT = 90000;
                int materialTypeId;
                int objectId;
                TypeOfObjects objectType;
                Vec3fTriangle triangle = {{0,0,0},{0,0,0},{0,0,0}};
                for(int s = 0; s < scene.spheres.size() ; s++){
                    float t = intersectSphere(currRay, scene.spheres[s].radius, scene.vertex_data[scene.spheres[s].center_vertex_id - 1]);

                    // on the image plane when t == 1
                    if(t < minT && t >= 1){
                        minT = t;
                        materialTypeId = scene.spheres[s].material_id;
                        objectId = s;
                        objectType =TypeOfObjects::Sphere;
                    }
                }

                for(int tr = 0; tr < scene.triangles.size() ; tr++){

                    Vec3f corner0 = scene.vertex_data[scene.triangles[tr].indices.v0_id -1 ];
                    Vec3f corner1 = scene.vertex_data[scene.triangles[tr].indices.v1_id -1 ];
                    Vec3f corner2 = scene.vertex_data[scene.triangles[tr].indices.v2_id -1 ];
                    float t = intersectTriangle(currRay, corner0, corner1, corner2);

                    // on the image plane when t == 1
                    if(t < minT && t >= 1){
                        minT = t;
                        materialTypeId = scene.triangles[tr].material_id;
                        objectId = tr;
                        objectType =TypeOfObjects::Face;
                        triangle.v1 = corner0;
                        triangle.v2 = corner1;
                        triangle.v3 = corner2;
                    }
                }

                for(int iterator = 0 ; iterator < scene.meshes.size() ; iterator++){
                    for(int tr = 0; tr < scene.meshes[iterator].faces.size() ; tr++){
                        // Localication açısından bak !
                        Vec3f corner0 = scene.vertex_data[scene.meshes[iterator].faces[tr].v0_id -1];
                        Vec3f corner1 = scene.vertex_data[scene.meshes[iterator].faces[tr].v1_id -1];
                        Vec3f corner2 = scene.vertex_data[scene.meshes[iterator].faces[tr].v2_id -1];
                        float t = intersectTriangle(currRay, corner0, corner1, corner2);

                        // on the image plane when t == 1
                        if(t < minT && t >= 1){
                            minT = t;
                            materialTypeId = scene.meshes[iterator].material_id;
                            objectId = tr;
                            objectType =TypeOfObjects::Face;
                            triangle.v1 = corner0;
                            triangle.v2 = corner1;
                            triangle.v3 = corner2;
                        }
                    }
                }


                if(minT < 90000){
                    Vec3f pixelColor = {0,0,0};

                    Vec3f intersectionPoint = currRay.o + currRay.d * minT;

                    Vec3f normalVector;
                    if(objectType == TypeOfObjects::Sphere){
                        normalVector = calcSphereNormal(intersectionPoint, scene.vertex_data[scene.spheres[objectId].center_vertex_id - 1]);
                    }
                    else{
                        normalVector = calcTriangleNormal(triangle.v1, triangle.v2, triangle.v3);
                        /* LAST UPDATED PLACE */
                        /*
                        if(Vec3f::dotProduct(normalVector,currRay.d) > 0){
                            normalVector = Vec3f{0,0,0} - normalVector;
                        }
                        */
                    }

                    Vec3f toCamera = Vec3f::normalize(Vec3f{0,0,0} - currRay.d);

                    // Ambient part
                    pixelColor.x += scene.materials[materialTypeId - 1].ambient.x * scene.ambient_light.x;
                    pixelColor.y += scene.materials[materialTypeId - 1].ambient.y * scene.ambient_light.y;
                    pixelColor.z += scene.materials[materialTypeId - 1].ambient.z * scene.ambient_light.z;

                    for(int l = 0; l < scene.point_lights.size() ; l++){
                        // ************ Shadows need to be considered here **************


                        Vec3f toLightVector = scene.point_lights[l].position - intersectionPoint;

                        // ************ Shadow CHECK ************

                        Ray toLightRay = {intersectionPoint, toLightVector};
                        bool shadowIntersectionFound = false;

                        for(int s = 0; s < scene.spheres.size() && !shadowIntersectionFound ; s++){
                            float t = intersectSphere(toLightRay, scene.spheres[s].radius, scene.vertex_data[scene.spheres[s].center_vertex_id - 1]);

                            // Epsilon issue
                            if(t > scene.shadow_ray_epsilon && t < 1 ){
                                shadowIntersectionFound = true;
                            }
                        }

                        for(int tr = 0; tr < scene.triangles.size() && !shadowIntersectionFound ; tr++){

                            Vec3f corner0 = scene.vertex_data[scene.triangles[tr].indices.v0_id -1 ];
                            Vec3f corner1 = scene.vertex_data[scene.triangles[tr].indices.v1_id -1 ];
                            Vec3f corner2 = scene.vertex_data[scene.triangles[tr].indices.v2_id -1 ];
                            float t = intersectTriangle(toLightRay, corner0, corner1, corner2);

                            // Epsilon issue
                            if(t > scene.shadow_ray_epsilon && t < 1 ){
                                shadowIntersectionFound = true;
                            }

                        }

                        for(int iterator = 0 ; iterator < scene.meshes.size() && !shadowIntersectionFound ; iterator++){
                            for(int tr = 0; tr < scene.meshes[iterator].faces.size() && !shadowIntersectionFound ; tr++){
                                // Localication açısından bak !
                                Vec3f corner0 = scene.vertex_data[scene.meshes[iterator].faces[tr].v0_id -1];
                                Vec3f corner1 = scene.vertex_data[scene.meshes[iterator].faces[tr].v1_id -1];
                                Vec3f corner2 = scene.vertex_data[scene.meshes[iterator].faces[tr].v2_id -1];
                                float t = intersectTriangle(toLightRay, corner0, corner1, corner2);

                                // Epsilon issue
                                if(t > scene.shadow_ray_epsilon && t < 1 ){
                                    shadowIntersectionFound = true;
                                }
                            }
                        }

                        // ************ END of SHADOW CHECK ************

                        if(!shadowIntersectionFound){
                            toLightVector = Vec3f::normalize(toLightVector);


                            Vec3f irradiance = calcIrradiance(scene.point_lights[l],intersectionPoint);


                            // Diffuse part

                            pixelColor = pixelColor + calcDiffuseComponent(toLightVector, normalVector, irradiance,
                                                                           scene.materials[materialTypeId - 1]);

                            // Specular part

                            pixelColor = pixelColor + calcSpecularComponent(toLightVector, normalVector, irradiance,
                                                                            scene.materials[materialTypeId - 1],toCamera);
                        }
                    }

                    // Reflection
                    if(scene.max_recursion_depth > 0 && scene.materials[materialTypeId - 1].is_mirror != 0){
                        Ray reflectionRay = {intersectionPoint, (normalVector * (Vec3f::dotProduct(normalVector,toCamera) * 2))  - toCamera};

                        Vec3f reflectance = recursiveRayTracer(scene,reflectionRay,0);

                        reflectance.x *= scene.materials[materialTypeId -1].mirror.x;
                        reflectance.y *= scene.materials[materialTypeId -1].mirror.y;
                        reflectance.z *= scene.materials[materialTypeId -1].mirror.z;

                        pixelColor = pixelColor + reflectance;
                    }

                    // Rounding and Clamping
                    pixelColor = pixelColor + Vec3f{0.5,0.5,0.5};

                    if(pixelColor.x > 255) pixelColor.x = 255;
                    if(pixelColor.y > 255) pixelColor.y = 255;
                    if(pixelColor.z > 255) pixelColor.z = 255;


                    image[imageIterator++] =pixelColor.x;
                    image[imageIterator++] =pixelColor.y;
                    image[imageIterator++] =pixelColor.z;


                }
                else{
                    image[imageIterator++] = scene.background_color.x;
                    image[imageIterator++] = scene.background_color.y;
                    image[imageIterator++] = scene.background_color.z;
                }
            }
        }

        write_ppm(scene.cameras[i].image_name.data(),image, scene.cameras[i].image_width, scene.cameras[i].image_height);
        delete[] image;

    }

    return 0;
}
