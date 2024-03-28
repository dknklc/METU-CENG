#include <iostream>
#include <cmath>
#include "Helpers.h"
#include "Matrix4.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Camera.h"
#include "Color.h"
#include <vector>

using namespace std;

/*
 * Calculate cross product of vec3 a, vec3 b and return resulting vec3.
 */
Vec3 crossProductVec3(Vec3 a, Vec3 b)
{
    Vec3 result;

    result.x = a.y * b.z - b.y * a.z;
    result.y = b.x * a.z - a.x * b.z;
    result.z = a.x * b.y - b.x * a.y;

    return result;
}

/*
 * Calculate dot product of vec3 a, vec3 b and return resulting value.
 */
double dotProductVec3(Vec3 a, Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/*
 * Find length (|v|) of vec3 v.
 */
double magnitudeOfVec3(Vec3 v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

/*
 * Normalize the vec3 to make it unit vec3.
 */
Vec3 normalizeVec3(Vec3 v)
{
    Vec3 result;
    double d;

    d = magnitudeOfVec3(v);
    result.x = v.x / d;
    result.y = v.y / d;
    result.z = v.z / d;

    return result;
}

/*
 * Return -v (inverse of vec3 v)
 */
Vec3 inverseVec3(Vec3 v)
{
    Vec3 result;
    result.x = -v.x;
    result.y = -v.y;
    result.z = -v.z;

    return result;
}

/*
 * Add vec3 a to vec3 b and return resulting vec3 (a+b).
 */
Vec3 addVec3(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;

    return result;
}

/*
 * Subtract vec3 b from vec3 a and return resulting vec3 (a-b).
 */
Vec3 subtractVec3(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;

    return result;
}

/*
 * Multiply each element of vec3 with scalar.
 */
Vec3 multiplyVec3WithScalar(Vec3 v, double c)
{
    Vec3 result;
    result.x = v.x * c;
    result.y = v.y * c;
    result.z = v.z * c;

    return result;
}

/*
 * Prints elements in a vec3. Can be used for debugging purposes.
 */
void printVec3(Vec3 v)
{
    cout << "(" << v.x << "," << v.y << "," << v.z << ")" << endl;
}

/*
 * Check whether vec3 a and vec3 b are equal.
 * In case of equality, returns 1.
 * Otherwise, returns 0.
 */
int areEqualVec3(Vec3 a, Vec3 b)
{

    /* if x difference, y difference and z difference is smaller than threshold, then they are equal */
    if ((ABS((a.x - b.x)) < EPSILON) && (ABS((a.y - b.y)) < EPSILON) && (ABS((a.z - b.z)) < EPSILON))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
 * Returns an identity matrix (values on the diagonal are 1, others are 0).
*/
Matrix4 getIdentityMatrix()
{
    Matrix4 result;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (i == j)
            {
                result.val[i][j] = 1.0;
            }
            else
            {
                result.val[i][j] = 0.0;
            }
        }
    }

    return result;
}

/*
 * Multiply matrices m1 (Matrix4) and m2 (Matrix4) and return the result matrix r (Matrix4).
 */
Matrix4 multiplyMatrixWithMatrix(Matrix4 m1, Matrix4 m2)
{
    Matrix4 result;
    double total;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            total = 0;
            for (int k = 0; k < 4; k++)
            {
                total += m1.val[i][k] * m2.val[k][j];
            }

            result.val[i][j] = total;
        }
    }

    return result;
}

/*
 * Multiply matrix m (Matrix4) with vector v (vec4) and store the result in vector r (vec4).
 */
Vec4 multiplyMatrixWithVec4(Matrix4 m, Vec4 v)
{
    double values[4];
    double total;

    for (int i = 0; i < 4; i++)
    {
        total = 0;
        for (int j = 0; j < 4; j++)
        {
            total += m.val[i][j] * v.getElementAt(j);
        }
        values[i] = total;
    }

    return Vec4(values[0], values[1], values[2], values[3], v.colorId);
}

//  ***********************   OUR HELPERS  ************************

// ********************* MODELING TRANSFORMATIONS ***********************
Matrix4 modelingTransformation(Translation t){
    Matrix4 result = getIdentityMatrix();
    result.val[0][3] = t.tx;
    result.val[1][3] = t.ty;
    result.val[2][3] = t.tz;
    return result;
}

Matrix4 modelingTransformation(Rotation r){
    Vec3 u,v,w;
    Matrix4 M;
    Matrix4 R = getIdentityMatrix();
    Matrix4 MInverse;

    u = Vec3(r.ux,r.uy,r.uz,0);
    v = Vec3(0, -r.uz, r.uy,0);
    w = crossProductVec3(u,v);

    v = normalizeVec3(v);
    w = normalizeVec3(w);

    M.val[0][0] = u.x;
    M.val[0][1] = u.y;
    M.val[0][2] = u.z;
    M.val[0][3] = 0;
    M.val[1][0] = v.x;
    M.val[1][1] = v.y;
    M.val[1][2] = v.z;
    M.val[1][3] = 0;
    M.val[2][0] = w.x;
    M.val[2][1] = w.y;
    M.val[2][2] = w.z;
    M.val[2][3] = 0;
    M.val[3][3] = 1;

    R.val[1][1] = cos(r.angle * M_PI / 180);
    R.val[1][2] = -sin(r.angle * M_PI / 180);
    R.val[2][1] = -R.val[1][2];
    R.val[2][2] = R.val[1][1];

    MInverse.val[0][0] = u.x;
    MInverse.val[0][1] = v.x;
    MInverse.val[0][2] = w.x;
    MInverse.val[0][3] = 0;
    MInverse.val[1][0] = u.y;
    MInverse.val[1][1] = v.y;
    MInverse.val[1][2] = w.y;
    MInverse.val[1][3] = 0;
    MInverse.val[2][0] = u.z;
    MInverse.val[2][1] = v.z;
    MInverse.val[2][2] = w.z;
    MInverse.val[2][3] = 0;
    MInverse.val[3][3] = 1;

    return MInverse * R * M;
}

Matrix4 modelingTransformation(Scaling s){
    Matrix4 result;
    result.val[3][3] = 1;
    result.val[0][0] = s.sx;
    result.val[1][1] = s.sy;
    result.val[2][2] = s.sz;

    return result;
}
// ********************* END OF MODELING TRANSFORMATIONS ***********************

// *********************    VIEWING TRANSFORMATIONS  ***************************
Matrix4 cameraTransformation(const Camera &camera){
    Matrix4 R;
    Matrix4 T = getIdentityMatrix();
    T.val[0][3] = -camera.pos.x;
    T.val[1][3] = -camera.pos.y;
    T.val[2][3] = -camera.pos.z;

    R.val[0][0] = camera.u.x;
    R.val[0][1] = camera.u.y;
    R.val[0][2] = camera.u.z;

    R.val[1][0] = camera.v.x;
    R.val[1][1] = camera.v.y;
    R.val[1][2] = camera.v.z;

    R.val[2][0] = camera.w.x;
    R.val[2][1] = camera.w.y;
    R.val[2][2] = camera.w.z;

    R.val[3][3] = 1;

    return R * T;
}

Matrix4 orth(const Camera &camera){
    Matrix4 ort;
    ort.val[0][0] = 2 / (camera.right - camera.left);
    ort.val[1][1] = 2 / (camera.top - camera.bottom);
    ort.val[2][2] = -2 / (camera.far - camera.near);
    ort.val[3][3] = 1;
    ort.val[0][3] = - (camera.right + camera.left) / (camera.right - camera.left);
    ort.val[1][3] = - (camera.top + camera.bottom) / (camera.top - camera.bottom);
    ort.val[2][3] = - (camera.far + camera.near) / (camera.far - camera.near);
    return ort;

}

Matrix4 p2o(const Camera &camera){
    Matrix4 p2o;
    p2o.val[0][0] = camera.near;
    p2o.val[1][1] = camera.near;
    p2o.val[2][2] = camera.far + camera.near;
    p2o.val[2][3] = camera.far * camera.near;
    p2o.val[3][2] = -1;
    return p2o;
}

Matrix4 viewportTransformation(const Camera &camera){
    Matrix4 view;
    view.val[0][0] = camera.horRes / 2;
    view.val[1][1] = camera.verRes / 2;
    view.val[2][2] = 1/2;
    view.val[2][3] = 1/2;
    view.val[0][3] = (camera.horRes - 1) / 2;
    view.val[1][3] = (camera.verRes - 1) / 2;
    return view;
}

Vec3 twoPoint2vector(Vec3 o, Vec3 d){
    return Vec3(d.x - o.x, d.y - o.y, d.z -o.z,0);
}

// Rasterization

void lineRasterization(vector<vector<Color>> &image, Camera &camera , Vec3 p0 , Vec3 p1, Color c0, Color c1){
    double dx , dy, d ;
    int y0,y1,x0,x1;
    int x,y;
    Color c,dc;

    x0 = round(p0.x);
    if(x0 >= camera.horRes)
        x0 = camera.horRes - 1;
    if(x0 < 0)
        x0 = 0;

    y0 = round(p0.y);
    if(y0 >= camera.verRes)
        y0 = camera.verRes -1;
    if(y0 < 0)
        y0 = 0;

    x1 = round(p1.x);
    if(x1 >= camera.horRes)
        x1 = camera.horRes - 1;
    if(x1 < 0)
        x1 = 0;

    y1 = round(p1.y);
    if(y1 >= camera.verRes)
        y1 = camera.verRes -1;
    if(y1 < 0)
        y1 = 0;


    dx = x1 - x0;
    dy = y1 - y0;

    if(dx == 0){        // vertical
        y = y0;
        c = c0;
        dc.r = (c1.r - c0.r) / abs(y1 - y0);
        dc.g = (c1.g - c0.g) / abs(y1 - y0);
        dc.b = (c1.b - c0.b) / abs(y1 - y0);
        x = (y1 - y0) / abs(y1 - y0);
        while(y != y1){
          if(x < camera.horRes && x >= 0 && y < camera.verRes && y >= 0){
            image[x0][y] = c;
          }
            y = y + x;
            c.r = c.r + dc.r;
            c.g = c.g + dc.g;
            c.b = c.b + dc.b;
        }
        return;
    }
    else if(dy == 0){   // horizontal
        x = x0;
        c = c0;
        dc.r = (c1.r - c0.r) / abs(x1 - x0);
        dc.g = (c1.g - c0.g) / abs(x1 - x0);
        dc.b = (c1.b - c0.b) / abs(x1 - x0);
        y = (x1 - x0) / abs(x1 - x0);
        while(x != x1){
          if(x < camera.horRes && x >= 0 && y < camera.verRes && y >= 0){
            image[x][y0] = c;
          }
            x = x + y;
            c.r = c.r + dc.r;
            c.g = c.g + dc.g;
            c.b = c.b + dc.b;
        }
        return;
    }
    else {
        double m = dy / dx;
        if ( m > 0){
            /*  the line is like :  /  */

            if(p0.x > p1.x){
                int temp = x0;
                x0 = x1;
                x1 = temp;

                temp = y0;
                y0 = y1;
                y1 = temp;

                Color tempColor = Color(c0);
                c0 = c1;
                c1 = tempColor;
            }

            if(m > 1){
                // When the angle > 45.

                x = x0;
                d = (x1 - x0) + 0.5 * (y0 - y1);
                c = c0;
                dc.r = (c1.r - c0.r) / (y1 - y0);
                dc.g = (c1.g - c0.g) / (y1 - y0);
                dc.b = (c1.b - c0.b) / (y1 - y0);
                for(y = y0 ; y <= y1 ; y++){
                  if(x < camera.horRes && x >= 0 && y < camera.verRes && y >= 0){
                    image[x][y] = c;
                  }
                    if( d > 0){  // choose NE
                        x = x + 1;
                        d += (y0 - y1) + (x1 - x0);
                    }
                    else{       // choose N
                        d += (x1 - x0);
                    }
                    c.r = c.r + dc.r;
                    c.g = c.g + dc.g;
                    c.b = c.b + dc.b;
                }
            }
            else{
                // When the angle < 45. 0 < m <= 1.
                y = y0;
                d = (y0 - y1) + 0.5 * (x1 - x0);
                c = c0;
                dc.r = (c1.r - c0.r) / (x1 - x0);
                dc.g = (c1.g - c0.g) / (x1 - x0);
                dc.b = (c1.b - c0.b) / (x1 - x0);
                for(x = x0 ; x <= x1 ; x++){
                  if(x < camera.horRes && x >= 0 && y < camera.verRes && y >= 0){
                    image[x][y] = c;
                  }
                    if( d < 0){  // choose NE
                        y = y + 1;
                        d += (y0 - y1) + (x1 - x0);
                    }
                    else{       // choose E
                        d += (y0 - y1);
                    }
                    c.r = c.r + dc.r;
                    c.g = c.g + dc.g;
                    c.b = c.b + dc.b;
                }
            }
        }
        else {
            /*  the line is like :  \  */

            if(p0.x < p1.x){
                int temp = x0;
                x0 = x1;
                x1 = temp;

                temp = y0;
                y0 = y1;
                y1 = temp;

                Color tempColor = Color(c0);
                c0 = c1;
                c1 = tempColor;
            }

            if( m < -1){
                // When the angle < 135

                x = x0;
                d = (x1 - x0) - 0.5 * (y0 - y1);
                c = c0;
                dc.r = (c1.r - c0.r) / (y1 - y0);
                dc.g = (c1.g - c0.g) / (y1 - y0);
                dc.b = (c1.b - c0.b) / (y1 - y0);
                for(y = y0 ; y <= y1 ; y++){
                  if(x < camera.horRes && x >= 0 && y < camera.verRes && y >= 0){
                    image[x][y] = c;
                  }
                    if( d < 0){  // choose NW
                        x = x - 1;
                        d += -(y0 - y1) + (x1 - x0);
                    }
                    else{       // choose N
                        d += (x1 - x0);
                    }
                    c.r = c.r + dc.r;
                    c.g = c.g + dc.g;
                    c.b = c.b + dc.b;
                }
            }
            else{
                // When the angle > 135

                y = y0;
                d = -(y0 - y1) + 0.5 * (x1 - x0);
                c = c0;
                dc.r = (c1.r - c0.r) / (x0 - x1);
                dc.g = (c1.g - c0.g) / (x0 - x1);
                dc.b = (c1.b - c0.b) / (x0 - x1);
                for(x = x0 ; x >= x1 ; x--){
                  if(x < camera.horRes && x >= 0 && y < camera.verRes && y >= 0){
                    image[x][y] = c;
                  }
                    if( d > 0){  // choose NW
                        y = y + 1;
                        d += -(y0 - y1) + (x1 - x0);
                    }
                    else{       // choose W
                        d += -(y0 - y1);
                    }
                    c.r = c.r + dc.r;
                    c.g = c.g + dc.g;
                    c.b = c.b + dc.b;
                }
            }
        }
    }
}

void triangleRasterization(vector<vector<Color>> &image , Camera &camera, Vec3 p0 , Vec3 p1, Vec3 p2 ,Color c0, Color c1, Color c2){

    double y0,y1,x0,x1,x2,y2, alpha,beta,gamma;
    int x,y,xmin,ymin,xmax,ymax;
    Color c;
    x0 = round(p0.x);
    if(x0 >= camera.horRes)
        x0 = x0 - 1;
    if(x0 < 0)
        x0 = 0;

    y0 = round(p0.y);
    if(y0 >= camera.verRes)
        y0 = y0 -1;
    if(y0 < 0)
        y0 = 0;

    x1 = round(p1.x);
    if(x1 >= camera.horRes)
        x1 = x1 - 1;
    if(x1 < 0)
        x1 = 0;

    y1 = round(p1.y);
    if(y1 >= camera.verRes)
        y1 = y1 -1;
    if(y1 < 0)
        y1 = 0;

    x2 = round(p2.x);
    if(x2 >= camera.horRes)
        x2 = x2 - 1;
    if(x2 < 0)
        x2 = 0;

    y2 = round(p2.y);
    if(y2 >= camera.verRes)
        y2 = y2 - 1;
    if(y2 < 0)
        y2 = 0;

    xmin = min(min(x0,x1),x2);
    ymin = min(min(y0,y1),y2);
    xmax = max(max(x0,x1),x2);
    ymax = max(max(y0,y1),y2);

    for(x = xmin ; x <= xmax ; x++){
        for(y = ymin ; y <= ymax ; y++){
            alpha = (x * (y1 - y2) + y * (x2 - x1) + x1*y2 - y1*x2) /
                    (x0 * (y1 - y2) + y0 * (x2 - x1) + x1*y2 - y1*x2);

            beta = (x * (y2 - y0) + y * (x0 - x2) + x2*y0 - y2*x0) /
                    (x1 * (y2 - y0) + y1 * (x0 - x2) + x2*y0 - y2*x0);

            gamma = (x * (y0 - y1) + y * (x1 - x0) + x0*y1 - y0*x1) /
                    (x2 * (y0 - y1) + y2 * (x1 - x0) + x0*y1 - y0*x1);

            if(alpha >= 0 && beta >= 0 && gamma >= 0){
                c.r = alpha*c0.r + beta*c1.r + gamma*c2.r;
                c.g = alpha*c0.g + beta*c1.g + gamma*c2.g;
                c.b = alpha*c0.b + beta*c1.b + gamma*c2.b;
                if(x < camera.horRes && x >= 0 && y < camera.verRes && y >= 0){
                  image[x][y] = c;
                }
            }
        }
    }

}

bool visible(double den, double num, double & tE, double & tL){
    if(den > 0){                 // potentially entering
        double t = num / den;
        if(t > tL)
            return false;
        if(t > tE)
            tE = t;
    }
    else if(den < 0){           // potentially leaving
        double t = num / den;
        if(t < tE)
            return false;
        if(t < tL)
            tL = t;
    }
    else if(num > 0){          // line parallel to edge
        return false;
    }

    return true;
}

bool clipping(Vec4 & p0, Vec4 & p1, Color & c0, Color & c1, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax){
    double tE = 0;
    double tL = 1;
    bool visibility = false;
    if(visible(p1.x - p0.x, xmin -p0.x, tE, tL)){
        if(visible(p0.x - p1.x, p0.x - xmax, tE, tL)){
            if(visible(p1.y - p0.y, ymin -p0.y, tE, tL)){
                if(visible(p0.y - p1.y, p0.y - ymax, tE, tL)){
                    if(visible(p1.z - p0.z, zmin - p0.z, tE, tL)){
                        if(visible(p0.z - p1.z, p0.z - zmax, tE, tL)){
                            visibility = true;
                            if(tL < 1){
                                p1.x = p0.x + (p1.x - p0.x)*tL;
                                p1.y = p0.y + (p1.y - p0.y)*tL;
                                p1.z = p0.z + (p1.z - p0.z)*tL;

                                c1.r = tL * c0.r + (1 - tL) * c1.r;
                                c1.g = tL * c0.g + (1 - tL) * c1.g;
                                c1.b = tL * c0.b + (1 - tL) * c1.b;
                            }
                            if(tE > 0){
                                p0.x = p0.x + (p1.x - p0.x)*tE;
                                p0.y = p0.y + (p1.y - p0.y)*tE;
                                p0.z = p0.z + (p1.z - p0.z)*tE;

                                c0.r = tE * c0.r + (1 - tE) * c1.r;
                                c0.g = tE * c0.g + (1 - tE) * c1.g;
                                c0.b = tE * c0.b + (1 - tE) * c1.b;
                            }
                        }
                    }
                }
            }
        }
    }
    return visibility;
}









// COMPLETED
/* Modeling Transformations (Translation , Rotation , Scaling)
 * Viewing Transformations (Camera , Orthographic and Perspective , Viewport)
 * ForwardRenderingPipeline Function
 * Backface Culling
 * Line Rasterization (CHECK !!! ----> Büyük eşit , Küçük eşit , Azalttığımızdaki şarta bak).
 * Triangle Rasterization
 * Clipping
 * Forward Rendering Pipeline function
 * Testing
 */

// NOT COMPLETED
// Inek Testing


/*
 * 7 8 4
 * 7 4 3
 * 8 5 1
 * 8 1 4
 * 6 3 2
 * 6 7 3
 * 3 4 1
 * 3 1 2
 * 6 2 5
 * 2 1 5
 * 5 8 6
 * 7 6 8
 *
 *
 * */

/*
 *              lines[0] = hjTriangle[0];
                lines[1] = hjTriangle[1];
                lines[2] = hjTriangle[1];
                lines[3] = hjTriangle[2];
                lines[4] = hjTriangle[2];
                lines[5] = hjTriangle[0];

                lineColors[3] = lineColors[2];
                lineColors[4] = lineColors[2];
                lineColors[5] = lineColors[0];
                lineColors[2] = lineColors[1];
 *
 * */



/*
 *              if(this->cullingEnabled){
                // If backface, set triangle.culled = true, and then continue

                Vec3 a,b,v;
                a = twoPoint2vector(*this->vertices[this->meshes[m]->triangles[f].vertexIds[0] - 1], *this->vertices[this->meshes[m]->triangles[f].vertexIds[1] - 1]);
                b = twoPoint2vector(*this->vertices[this->meshes[m]->triangles[f].vertexIds[1] - 1],*this->vertices[this->meshes[m]->triangles[f].vertexIds[2] - 1]);
                v = twoPoint2vector(camera->pos,*this->vertices[this->meshes[m]->triangles[f].vertexIds[0] - 1]);
                if(dotProductVec3(crossProductVec3(a,b), v) > 0){
                    this->meshes[m]->triangles[f].culled = true;
                    continue;
                }



            }
 *
 * */
