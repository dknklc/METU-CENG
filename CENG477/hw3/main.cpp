#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <GL/glew.h>   // The GL Header File
#include <GL/gl.h>   // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using namespace std;

GLuint gProgram[3];
GLint gIntensityLoc;
float gIntensity = 20;
int gWidth = 640, gHeight = 600;
string objPath;
int moves ;
int score ;
float gridX ;
float gridY ;
float gridWidth ;
float gridHeight ;
int bunnyCount;
bool isFrameFinished;
bool isSlidingFinished;
bool isScalingFinished;
float footerHeight = 2;
float objXmax = -1000;
float objYmax = -1000;
float objZmax = -1000;
float objXmin = 1000;
float objYmin = 1000;
float objZmin = 1000;
float objYoffset;
float objYscale;
float objXZscale;
bool matchingScale;
bool isMatchingFinished;
int slidedBunnyCounter;

bool animation = false;

struct bunny{
    float x,y,scale,angle;
    bool isBombed, isMatched;
    int colorId;
};


glm::vec3 colors[5] = {glm::vec3(1.0,0,0),glm::vec3(1.0,0.498,0.153),glm::vec3(0,0,1.0),glm::vec3(0.6,0.6,0.6),glm::vec3(0.4,0.4,0.0)};
vector<bunny> bunnies;
vector<vector<int>> matched;

struct Vertex
{
    Vertex(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
    GLfloat x, y, z;
};

struct Texture
{
    Texture(GLfloat inU, GLfloat inV) : u(inU), v(inV) { }
    GLfloat u, v;
};

struct Normal
{
    Normal(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
    GLfloat x, y, z;
};

struct Face
{
    Face(int v[], int t[], int n[]) {
        vIndex[0] = v[0];
        vIndex[1] = v[1];
        vIndex[2] = v[2];
        tIndex[0] = t[0];
        tIndex[1] = t[1];
        tIndex[2] = t[2];
        nIndex[0] = n[0];
        nIndex[1] = n[1];
        nIndex[2] = n[2];
    }
    GLuint vIndex[3], tIndex[3], nIndex[3];
};

vector<Vertex> gVertices;
vector<Texture> gTextures;
vector<Normal> gNormals;
vector<Face> gFaces;

GLuint gVertexAttribBuffer, gTextVBO, gIndexBuffer;
GLint gInVertexLoc, gInNormalLoc;
int gVertexDataSizeInBytes, gNormalDataSizeInBytes;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;


bool ParseObj(const string& fileName)
{
    fstream myfile;

    // Open the input
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open())
    {
        string curLine;

        while (getline(myfile, curLine))
        {
            stringstream str(curLine);
            GLfloat c1, c2, c3;
            GLuint index[9];
            string tmp;

            if (curLine.length() >= 2)
            {
                if (curLine[0] == '#') // comment
                {
                    continue;
                }
                else if (curLine[0] == 'v')
                {
                    if (curLine[1] == 't') // texture
                    {
                        str >> tmp; // consume "vt"
                        str >> c1 >> c2;
                        gTextures.push_back(Texture(c1, c2));
                    }
                    else if (curLine[1] == 'n') // normal
                    {
                        str >> tmp; // consume "vn"
                        str >> c1 >> c2 >> c3;
                        gNormals.push_back(Normal(c1, c2, c3));
                    }
                    else // vertex
                    {
                        str >> tmp; // consume "v"
                        str >> c1 >> c2 >> c3;
                        if(c2 < objYmin){
                            objYmin = c2;
                        }
                        if(c2 > objYmax){
                            objYmax = c2;
                        }
                        if(c1 < objXmin){
                            objXmin = c1;
                        }
                        if(c1 > objXmax){
                            objXmax = c1;
                        }
                        if(c3 < objZmin){
                            objZmin = c3;
                        }
                        if(c3 > objZmax){
                            objZmax = c3;
                        }
                        gVertices.push_back(Vertex(c1, c2, c3));
                    }
                }
                else if (curLine[0] == 'f') // face
                {
                    str >> tmp; // consume "f"
                    char c;
                    int vIndex[3],  nIndex[3], tIndex[3];
                    str >> vIndex[0]; str >> c >> c; // consume "//"
                    str >> nIndex[0];
                    str >> vIndex[1]; str >> c >> c; // consume "//"
                    str >> nIndex[1];
                    str >> vIndex[2]; str >> c >> c; // consume "//"
                    str >> nIndex[2];

                    assert(vIndex[0] == nIndex[0] &&
                           vIndex[1] == nIndex[1] &&
                           vIndex[2] == nIndex[2]); // a limitation for now

                    // make indices start from 0
                    for (int c = 0; c < 3; ++c)
                    {
                        vIndex[c] -= 1;
                        nIndex[c] -= 1;
                        tIndex[c] -= 1;
                    }

                    gFaces.push_back(Face(vIndex, tIndex, nIndex));
                }
                else
                {
                    cout << "Ignoring unidentified line in obj file: " << curLine << endl;
                }
            }

            //data += curLine;
            if (!myfile.eof())
            {
                //data += "\n";
            }
        }

        myfile.close();
    }
    else
    {
        return false;
    }

    /*
    for (int i = 0; i < gVertices.size(); ++i)
    {
        Vector3 n;

        for (int j = 0; j < gFaces.size(); ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                if (gFaces[j].vIndex[k] == i)
                {
                    // face j contains vertex i
                    Vector3 a(gVertices[gFaces[j].vIndex[0]].x,
                              gVertices[gFaces[j].vIndex[0]].y,
                              gVertices[gFaces[j].vIndex[0]].z);

                    Vector3 b(gVertices[gFaces[j].vIndex[1]].x,
                              gVertices[gFaces[j].vIndex[1]].y,
                              gVertices[gFaces[j].vIndex[1]].z);

                    Vector3 c(gVertices[gFaces[j].vIndex[2]].x,
                              gVertices[gFaces[j].vIndex[2]].y,
                              gVertices[gFaces[j].vIndex[2]].z);

                    Vector3 ab = b - a;
                    Vector3 ac = c - a;
                    Vector3 normalFromThisFace = (ab.cross(ac)).getNormalized();
                    n += normalFromThisFace;
                }

            }
        }

        n.normalize();

        gNormals.push_back(Normal(n.x, n.y, n.z));
    }
    */

    assert(gVertices.size() == gNormals.size());

    return true;
}

bool ReadDataFromFile(
        const string& fileName, ///< [in]  Name of the shader file
        string&       data)     ///< [out] The contents of the file
{
    fstream myfile;

    // Open the input
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open())
    {
        string curLine;

        while (getline(myfile, curLine))
        {
            data += curLine;
            if (!myfile.eof())
            {
                data += "\n";
            }
        }

        myfile.close();
    }
    else
    {
        return false;
    }

    return true;
}

void createVS(GLuint& program, const string& filename)
{
    string shaderSource;

    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &shader, &length);
    glCompileShader(vs);

    char output[1024] = {0};
    glGetShaderInfoLog(vs, 1024, &length, output);
    printf("VS compile log: %s\n", output);

    glAttachShader(program, vs);
}

void createFS(GLuint& program, const string& filename)
{
    string shaderSource;

    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &shader, &length);
    glCompileShader(fs);

    char output[1024] = {0};
    glGetShaderInfoLog(fs, 1024, &length, output);
    printf("FS compile log: %s\n", output);

    glAttachShader(program, fs);
}

void initShaders()
{
    gProgram[0] = glCreateProgram();
    gProgram[1] = glCreateProgram();
    gProgram[2] = glCreateProgram();

    createVS(gProgram[0], "vert0.glsl");
    createFS(gProgram[0], "frag0.glsl");

    createVS(gProgram[1], "vert1.glsl");
    createFS(gProgram[1], "frag1.glsl");

    createVS(gProgram[2], "vert_text.glsl");
    createFS(gProgram[2], "frag_text.glsl");

    glBindAttribLocation(gProgram[0], 0, "inVertex");
    glBindAttribLocation(gProgram[0], 1, "inNormal");
    glBindAttribLocation(gProgram[1], 0, "inVertex");
    glBindAttribLocation(gProgram[1], 1, "inNormal");
    glBindAttribLocation(gProgram[2], 2, "vertex");

    glLinkProgram(gProgram[0]);
    glLinkProgram(gProgram[1]);
    glLinkProgram(gProgram[2]);
    glUseProgram(gProgram[0]);

    gIntensityLoc = glGetUniformLocation(gProgram[0], "intensity");
    cout << "gIntensityLoc = " << gIntensityLoc << endl;
    glUniform1f(gIntensityLoc, gIntensity);
}

void initVBO()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    assert(glGetError() == GL_NONE);

    glGenBuffers(1, &gVertexAttribBuffer);
    glGenBuffers(1, &gIndexBuffer);

    assert(gVertexAttribBuffer > 0 && gIndexBuffer > 0);

    glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

    gVertexDataSizeInBytes = gVertices.size() * 3 * sizeof(GLfloat);
    gNormalDataSizeInBytes = gNormals.size() * 3 * sizeof(GLfloat);
    int indexDataSizeInBytes = gFaces.size() * 3 * sizeof(GLuint);
    GLfloat* vertexData = new GLfloat [gVertices.size() * 3];
    GLfloat* normalData = new GLfloat [gNormals.size() * 3];
    GLuint* indexData = new GLuint [gFaces.size() * 3];

    float minX = 1e6, maxX = -1e6;
    float minY = 1e6, maxY = -1e6;
    float minZ = 1e6, maxZ = -1e6;

    for (int i = 0; i < gVertices.size(); ++i)
    {
        vertexData[3*i] = gVertices[i].x;
        vertexData[3*i+1] = gVertices[i].y;
        vertexData[3*i+2] = gVertices[i].z;

        minX = std::min(minX, gVertices[i].x);
        maxX = std::max(maxX, gVertices[i].x);
        minY = std::min(minY, gVertices[i].y);
        maxY = std::max(maxY, gVertices[i].y);
        minZ = std::min(minZ, gVertices[i].z);
        maxZ = std::max(maxZ, gVertices[i].z);
    }

    //std::cout << "minX = " << minX << std::endl;
    //std::cout << "maxX = " << maxX << std::endl;
    //std::cout << "minY = " << minY << std::endl;
    //std::cout << "maxY = " << maxY << std::endl;
    //std::cout << "minZ = " << minZ << std::endl;
    //std::cout << "maxZ = " << maxZ << std::endl;

    for (int i = 0; i < gNormals.size(); ++i)
    {
        normalData[3*i] = gNormals[i].x;
        normalData[3*i+1] = gNormals[i].y;
        normalData[3*i+2] = gNormals[i].z;
    }

    for (int i = 0; i < gFaces.size(); ++i)
    {
        indexData[3*i] = gFaces[i].vIndex[0];
        indexData[3*i+1] = gFaces[i].vIndex[1];
        indexData[3*i+2] = gFaces[i].vIndex[2];
    }


    glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, vertexData);
    glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, normalData);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

    // done copying; can free now
    delete[] vertexData;
    delete[] normalData;
    delete[] indexData;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));

}

void initFonts(int windowWidth, int windowHeight)
{
    // Set OpenGL options
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(windowWidth), 0.0f, static_cast<GLfloat>(windowHeight));
    glUseProgram(gProgram[2]);
    glUniformMatrix4fv(glGetUniformLocation(gProgram[2], "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, "/usr/share/fonts/truetype/liberation/LiberationSerif-Italic.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    //
    // Configure VBO for texture quads
    //
    glGenBuffers(1, &gTextVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void matching(){
    //float x,y;
    int bunnyId;
    int count= 1;
    int curColorId;
    for(int j = 0 ; j < gridX ; j++){
        //x = -10 + gridWidth/2;
        curColorId= bunnies[j].colorId;
        count=1;
        for(int i = 1 ; i < gridY ; i++){
            bunnyId = i*gridX + j;
            if(i==gridY-1){
                if(bunnies[bunnyId].colorId== curColorId){
                    count++;
                    if(count>=3){
                        //DOşE
                        for(int k=0; k<count ; k++){
                            matched[j].insert(matched[j].begin(),i+k-count+1);
                        }
                    }
                }
                else if(count>=3){
                    //DOşE
                    for(int k=0; k<count ; k++){
                        matched[j].insert(matched[j].begin(),i+k-count);
                    }
                }
                count=1;
            }
            else{
                if(bunnies[bunnyId].colorId== curColorId){
                    count++;
                }
                else{
                    if(count>=3){
                        //DOşE
                        for(int k=0; k<count ; k++){
                            matched[j].insert(matched[j].begin(),i + k-count );
                        }
                    }
                    count=1;
                    curColorId= bunnies[bunnyId].colorId;
                }
            }


            //x+=gridWidth;

        }
        //y-=gridHeight;
    }
    count = 1;

    for(int i = 0 ; i < gridY ; i++){
        //x = -10 + gridWidth/2;
        curColorId= bunnies[i*gridX].colorId;
        count = 1;
        for(int j = 1 ; j < gridX ; j++){
            bunnyId = i*gridX + j;
            if(j == gridX-1){
                if(bunnies[bunnyId].colorId== curColorId){
                    count++;
                    if(count >= 3){
                        //cout << i << "th row : ";
                        for(int k=0; k<count ; k++){
                            //cout << j-k << " ";

                            for(int l= 0; l<=matched[j-k].size() ; l++){
                                if(l==matched[j-k].size()){
                                    matched[j-k].push_back(i);
                                    break;
                                }
                                else{
                                    if(matched[j-k][l] == i){
                                        break;
                                    }
                                    else{
                                        if(matched[j-k][l]<i){
                                            matched[j-k].insert(matched[j-k].begin()+l,i);
                                            break;
                                        }
                                    }
                                }

                            }


                        }
                        //cout << endl;
                    }
                }
                else if(count >= 3){
                    //cout << i << "th row : ";
                    for(int k=0; k<count ; k++){
                        //cout << j-k-1 << " ";

                        for(int l= 0; l<=matched[j-k-1].size() ; l++){
                            if(l==matched[j-k-1].size()){
                                matched[j-k-1].push_back(i);
                                break;
                            }
                            else{
                                if(matched[j-k-1][l] == i){
                                    break;
                                }
                                else{
                                    if(matched[j-k-1][l]<i){
                                        matched[j-k-1].insert(matched[j-k-1].begin()+l,i);
                                        break;
                                    }
                                }
                            }

                        }


                    }
                    //cout << endl;
                }
            }
            else{
                if(bunnies[bunnyId].colorId== curColorId){
                    count++;
                }
                else{
                    if(count>=3){
                        //cout << i << "th row : ";
                        //DOşE

                        for(int k=0; k<count ; k++){
                            //cout << j-1-k << " ";

                            for(int l= 0; l<=matched[j-1-k].size() ; l++){
                                if(l==matched[j-1-k].size()){
                                    matched[j-1-k].push_back(i);
                                    break;
                                }
                                else{
                                    if(matched[j-1-k][l] == i){
                                        break;
                                    }
                                    else{
                                        if(matched[j-1-k][l]<i){
                                            matched[j-1-k].insert(matched[j-1-k].begin()+l,i);
                                            break;
                                        }
                                    }
                                }

                            }

                        }

                        //cout << endl;
                    }
                    count=1;
                    curColorId= bunnies[bunnyId].colorId;
                }
            }


            //x+=gridWidth;

        }
        //y-=gridHeight;
    }

}

void init()
{
    //ParseObj("armadillo.obj");
    isFrameFinished = true;
    isSlidingFinished = true;
    isScalingFinished = true;
    matchingScale = false;
    isMatchingFinished=true;
    for(int u =0; u<gridX; u++){
        matched.push_back(vector<int>());
    }
    moves =0;
    score =0;
    gridWidth = 20/gridX;
    gridHeight = (20-footerHeight)/gridY;
    bunnyCount = gridX * gridY;
    animation = false;
    ParseObj(objPath);
    objYoffset = -(objYmin+objYmax) / 2;

    objYscale = 0.9*gridHeight / (objYmax - objYmin);
    float diag1,diag2,diag3,diag4;
    diag1 = objXmin*objXmin + objZmin*objZmin;
    diag2 = objXmin*objXmin + objZmax*objZmax;
    diag3 = objXmax*objXmax + objZmin*objZmin;
    diag4 = objXmax*objXmax + objZmax*objZmax;
    objXZscale = (gridWidth) / (sqrt(max(max(max(diag1,diag2),diag3),diag4))*2);
    //cout << "objYoffset : " << objYoffset << endl;
    //cout << "objYmin : " << objYmin << endl;
    //cout << "objYmax : " << objYmax << endl;
    //cout << "objYscale : " << objYscale << endl;
    //cout << "objXZscale : " << objXZscale << endl;

    float x,y;
    y = 10 - gridHeight/2;
    for(int i = 0 ; i < gridY ; i++){
        x = -10 + gridWidth/2;

        for(int j = 0 ; j < gridX ; j++){
            int bunnyId = i*gridX + j;
            bunnies.push_back(bunny());
            bunnies[bunnyId].colorId = rand() % 5;
            bunnies[bunnyId].isBombed = false;
            bunnies[bunnyId].isMatched = false;
            bunnies[bunnyId].scale = 1;
            bunnies[bunnyId].angle = 0;
            bunnies[bunnyId].y = y;
            bunnies[bunnyId].x = x;


            x+=gridWidth;

        }

        y-=gridHeight;

    }

    for(int i = 0 ; i < gridX ; i++){
        cout << i << ".column size : " <<matched[i].size() << endl;

    }
    for(int i=0; i< gridX; i++){
        cout << i << "th column : ";
        if(matched[i].size() < 9){
            for(int j=0; j< matched[i].size(); j++){
                cout << matched[i][j] << " " ;
            }
            cout << endl;
        }
        else{
            for(int j=0; j< 8; j++){
                cout << matched[i][j] << " " ;
            }
            cout << endl;
        }

    }

    glEnable(GL_DEPTH_TEST);
    initShaders();
    initFonts(gWidth, gHeight);
    initVBO();
}
void restart(){
    isFrameFinished = true;
    isSlidingFinished = true;
    isScalingFinished = true;
    matchingScale = false;
    isMatchingFinished=true;
    moves =0;
    score =0;
    gridWidth = 20/gridX;
    gridHeight = (20 - footerHeight)/gridY;
    bunnyCount = gridX * gridY;
    animation = false;
    for(int u =0; u<gridX; u++){
        matched[u].clear();
    }
    //ParseObj(objPath);
    float x,y;
    y = 10 - gridHeight/2;
    for(int i = 0 ; i < gridY ; i++){
        x = -10 + gridWidth/2;

        for(int j = 0 ; j < gridX ; j++){
            int bunnyId = i*gridX + j;
            bunnies[bunnyId].colorId = rand() % 5;
            bunnies[bunnyId].isBombed = false;
            bunnies[bunnyId].isMatched = false;
            bunnies[bunnyId].scale = 1;
            bunnies[bunnyId].angle = 0;
            bunnies[bunnyId].y = y;
            bunnies[bunnyId].x = x;

            x+=gridWidth;

        }

        y-=gridHeight;

    }

    //glEnable(GL_DEPTH_TEST);
    //initShaders();
    //initFonts(gWidth, gHeight);
    //initVBO();
}

void drawModel()
{
    glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));

    glDrawElements(GL_TRIANGLES, gFaces.size() * 3, GL_UNSIGNED_INT, 0);
}

void renderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state
    glUseProgram(gProgram[2]);
    glUniform3f(glGetUniformLocation(gProgram[2], "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        // Update VBO for each character
        GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }
        };

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        //glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)

        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}



void display()
{
    glClearColor(0, 0, 0, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    static float angle = 0;
    glm::mat4 T;
    glm::mat4 R;
    glm::mat4 SR;
    glm::mat4 SRMatInv;
    glm::mat4 perspMat;


    float x,y;

    x = -10 + gridWidth/2;
    y = 10 - gridHeight/2;
    int k;
    slidedBunnyCounter=0;
    for(int i = 0 ; i < gridY ; i++){
        x = -10 + gridWidth/2;
        for(int j = 0 ; j < gridX ; j++){
            int bunnyId = i*gridX + j;
            // float scale = objYscale; // will be modified !!!
            float scale = min(objYscale,objXZscale);
            glUseProgram(gProgram[0]);
            //glLoadIdentity();
            //glTranslatef(-2, 0, -10);
            //glRotatef(angle, 0, 1, 0);

            if(bunnies[bunnyId].isBombed){
                if(bunnies[bunnyId].scale < 1.5){
                    animation = true;
                    isScalingFinished = false;
                    bunnies[bunnyId].scale += 0.01;
                    scale *= bunnies[bunnyId].scale;
                }
                else {
                    for(int m = i; m>0; m--){
                        bunnies[m*gridX+j]= bunnies[(m-1)*gridX+j];
                    }
                    bunnies[j].colorId = rand() % 5;
                    bunnies[j].isBombed = false;
                    bunnies[j].scale = 1;
                    bunnies[j].angle = -angle;
                    bunnies[j].y = 10+gridHeight/2;

                    /*TODO  yeni fieldları EKLE */

                    isSlidingFinished = false;
                    isFrameFinished = false;
                    isScalingFinished = true;
                    /*
                    bunnies[bunnyId].scale  = 1;
                    bunnies[bunnyId].isBombed = false;
                    animation = false;
                     */
                }
            }

            if(bunnies[bunnyId].isMatched){
                if(bunnies[bunnyId].scale < 1.5){
                    animation = true;
                    //isScalingFinished = false;
                    bunnies[bunnyId].scale += 0.01;
                    scale *= bunnies[bunnyId].scale;
                }
                else {
                    bunnies[bunnyId].scale = 1;
                    //bunnies[bunnyId].isMatched = false;
                    matchingScale = false;
                }
            }

            if(bunnies[bunnyId].y - y > 0.0001 && isFrameFinished){
                bunnies[bunnyId].y -= 0.05;
                if(bunnies[bunnyId].y- y < 0.0001){
                    //isSlidingFinished= true;
                    slidedBunnyCounter++;
                }
            }
            else{
                slidedBunnyCounter++;
            }

            T = glm::translate(glm::mat4(1.f), glm::vec3(bunnies[bunnyId].x, bunnies[bunnyId].y + objYoffset * scale, 0.f));
            R = glm::rotate(glm::mat4(1.f), glm::radians(angle+bunnies[bunnyId].angle), glm::vec3(0, 1, 0));
            SR = glm::scale(glm::mat4(1.f), glm::vec3(scale,scale,scale)) * R;

            SRMatInv = glm::transpose(glm::inverse(SR));
            //glm::mat4 perspMat = glm::perspective(glm::radians(45.0f), 1.f, 1.0f, 100.0f);
            perspMat = glm::ortho(-10.f, 10.f, -10.f, 10.f, -20.f, 20.f);
            glUniformMatrix4fv(glGetUniformLocation(gProgram[0], "SRMat"), 1, GL_FALSE, glm::value_ptr(SR));
            glUniformMatrix4fv(glGetUniformLocation(gProgram[0], "SRMatInv"), 1, GL_FALSE, glm::value_ptr(SRMatInv));
            glUniformMatrix4fv(glGetUniformLocation(gProgram[0], "modelingMat"), 1, GL_FALSE, glm::value_ptr(T*SR));
            glUniformMatrix4fv(glGetUniformLocation(gProgram[0], "perspectiveMat"), 1, GL_FALSE, glm::value_ptr(perspMat));
            k = bunnies[bunnyId].colorId;

            glUniform3f(glGetUniformLocation(gProgram[0], "kd"),colors[k].x,colors[k].y,colors[k].z);

            drawModel();

            x+=gridWidth;

        }

        y-=gridHeight;

    }
    //cout<<"slided bunny count: "<<slidedBunnyCounter << endl;
    if(slidedBunnyCounter == bunnyCount){
        isSlidingFinished=true;
    }

    assert(glGetError() == GL_NO_ERROR);

    renderText("Moves: "+ std::to_string(moves) +" Score: "+ std::to_string(score), 0, 3, 1, glm::vec3(1, 1, 0));

    assert(glGetError() == GL_NO_ERROR);

    angle += 0.25;
    isFrameFinished = true;
    //cout<< "slidinfinished: " << isSlidingFinished << endl;
    //cout<< "scalingfinished: " << isScalingFinished << endl;
    //cout<< "matchingfinished: " << isMatchingFinished << endl;
    if(isSlidingFinished && isScalingFinished && isMatchingFinished){
        animation = false;
    }
    if(!isMatchingFinished && !matchingScale){
        //matching finished edilecek
        isSlidingFinished= false;
        isMatchingFinished= true;
        //
        int size;
        int lastUnmatchedIndex=gridY-1;
        int randomlyGeneratedCount = 0;
        for(int i =0; i<gridX; i++){
            lastUnmatchedIndex=gridY-1;
            randomlyGeneratedCount = 0;
            if(size=matched[i].size()){
                for(int j =gridY-1; j>=size; j--){
                    for(int k = lastUnmatchedIndex; k>=0; k--){
                        if(!bunnies[k*gridX+i].isMatched){
                            bunnies[j*gridX + i]= bunnies[k*gridX + i];
                            lastUnmatchedIndex=k-1;
                            cout<< "lastunmatched index =  " << k-1 << endl;
                            break;
                        }

                    }
                }
                for(int j = size-1; j>=0; j--){
                    bunnies[j*gridX+i].colorId = rand() % 5;
                    bunnies[j*gridX+i].isMatched=false;
                    bunnies[j*gridX+i].angle = -angle;
                    //bunnies[j*gridX+i].scale = 1;
                    bunnies[j*gridX+i].y = 10+gridHeight/2 +gridHeight*randomlyGeneratedCount;
                    randomlyGeneratedCount++;
                }

            }
            matched[i].clear();
        }

    }
}

void reshape(GLFWwindow* window, int w, int h)
{
    w = w < 1 ? 1 : w;
    h = h < 1 ? 1 : h;

    gWidth = w;
    gHeight = h;

    glViewport(0, 0, w, h);
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        restart();
    }

    /*
    else if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        cout << "F pressed" << endl;
        glUseProgram(gProgram[1]);

    }
    else if (key == GLFW_KEY_V && action == GLFW_PRESS)
    {
        cout << "V pressed" << endl;
        glUseProgram(gProgram[0]);
    }
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        cout << "D pressed" << endl;
        gIntensity /= 1.5;
        cout << "gIntensity = " << gIntensity << endl;
        glUseProgram(gProgram[0]);
        glUniform1f(gIntensityLoc, gIntensity);
    }
    else if (key == GLFW_KEY_B && action == GLFW_PRESS)
    {
        cout << "B pressed" << endl;
        gIntensity *= 1.5;
        cout << "gIntensity = " << gIntensity << endl;
        glUseProgram(gProgram[0]);
        glUniform1f(gIntensityLoc, gIntensity);
    }
     */
}

void mouse(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !animation){
        double xpos, ypos;
        //getting cursor position
        glfwGetCursorPos(window, &xpos, &ypos);
        if(ypos < (600*(20-footerHeight)/20)){
            int x = (xpos / 640)*gridX;
            int y = (ypos / (600*(20-footerHeight)/20))*gridY;
            bunnies[y * gridX + x].isBombed = true;

            animation = true;
            moves++;
            cout << "Cursor Position at (" << xpos << " : " << ypos << endl;
            cout << "Index value : " <<  y * gridX + x << endl;
        }

    }

}

void mainLoop(GLFWwindow* window)
{
    while (!glfwWindowShouldClose(window))
    {
        /*  Matching and Blowing*/
        if(!animation){
            matching();
            //cout<< "MATCHINNNNNGGG!!!!!";
            int size;
            for(int i =0; i<gridX; i++){
                if(size=matched[i].size()){
                    score+=size;
                    isMatchingFinished=false;
                    matchingScale=true;
                    animation=true;
                    for(int j =0; j<size; j++){
                        bunnies[gridX*matched[i][j]+i].isMatched=true;

                    }
                }
            }
        }

        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main(int argc, char** argv)   // Create Main Function For Bringing It All Together
{
    gridX = stoi(argv[1]);
    gridY = stoi(argv[2]);
    objPath = argv[3];

    GLFWwindow* window;
    if (!glfwInit())
    {
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(gWidth, gHeight, "Simple Example", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    char rendererInfo[512] = {0};
    strcpy(rendererInfo, (const char*) glGetString(GL_RENDERER));
    strcat(rendererInfo, " - ");
    strcat(rendererInfo, (const char*) glGetString(GL_VERSION));
    glfwSetWindowTitle(window, rendererInfo);

    init();

    glfwSetKeyCallback(window, keyboard);
    glfwSetWindowSizeCallback(window, reshape);
    glfwSetMouseButtonCallback(window, mouse);

    reshape(window, gWidth, gHeight); // need to call this once ourselves
    mainLoop(window); // this does not return unless the window is closed

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

/*
 * Color Matching
 * Updating Score with respect to Color Matching
 * */
