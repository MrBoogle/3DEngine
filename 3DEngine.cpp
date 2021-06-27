// 3DEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <fstream>
#include <strstream>
#include <algorithm>
#include "olcConsoleGameEngine.h"
#include "vec3d.h"
#include "mat4x4.h"




struct triangle {
    vec3d p[3];
    wchar_t sym;
    short col;
};

struct mesh {
    std::vector<triangle> tris;

    bool LoadFromOBJFile(std::string sFileName) {

        std::ifstream f(sFileName);
        if (!f.is_open()) {
            return false;
        }

        std::vector<vec3d> verts;

        while (!f.eof()) {
            char line[128];
            f.getline(line, 128);

            std::strstream s;
            s << line;

            char junk;

            if (line[0] == 'v') {
                vec3d v;
                s >> junk >> v.x >> v.y >> v.z;
                verts.push_back(v);
            }

            if (line[0] == 'f') {
                int face[3];
                s >> junk >> face[0] >> face[1] >> face[2];
                tris.push_back({verts[face[0]-1], verts[face[1] - 1] , verts[face[2] - 1] });
            }
        }


        return true;
    }
};



class olcEngine3D : public olcConsoleGameEngine {
public:
    olcEngine3D() {
        m_sAppName = L"#D Demo";
    }

private:
    mesh meshCube;
    mat4x4 matProj;
    vec3d vCamera;

    float fTheta;

    void MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m) {
        o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
        o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
        o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
        float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

        if (w != 0.0f)
        {
            o.x /= w; o.y /= w; o.z /= w;
        }
    }

    // Taken From Command Line Webcam Video of Javidx9
    CHAR_INFO GetColour(float lum)
    {
        short bg_col, fg_col;
        wchar_t sym;
        int pixel_bw = (int)(13.0f * lum);
        switch (pixel_bw)
        {
        case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

        case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
        case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
        case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
        case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

        case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
        case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
        case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
        case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

        case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
        case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
        case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
        case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
        default:
            bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID;
        }

        CHAR_INFO c;
        c.Attributes = bg_col | fg_col;
        c.Char.UnicodeChar = sym;
        return c;
    }


public:
    bool OnUserCreate() override {

        //meshCube.tris = {
        //    // SOUTH
        //{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
        //{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

        //// EAST                                                      
        //{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
        //{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

        //// NORTH                                                     
        //{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
        //{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

        //// WEST                                                      
        //{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
        //{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

        //// TOP                                                       
        //{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
        //{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

        //// BOTTOM                                                    
        //{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
        //{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

        //};

        meshCube.LoadFromOBJFile("ps5.obj");

        float fNear = 0.1f;
        float fFar = 1000.0f;
        float fFov = 90.0;
        float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();

        fTheta = 0.0f;

        float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

        matProj.m[0][0] = fAspectRatio * fFovRad;
        matProj.m[1][1] = fFovRad;
        matProj.m[2][2] = fFar / (fFar - fNear);
        matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
        matProj.m[2][3] = 1.0f;
        matProj.m[3][3] = 0.0f;


        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {

        //Clear screen
        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);


        mat4x4 matRotZ, matRotX;
        
        fTheta += 1.0f * fElapsedTime;

        // Rotation Z
        matRotZ.m[0][0] = cosf(fTheta);
        matRotZ.m[0][1] = sinf(fTheta);
        matRotZ.m[1][0] = -sinf(fTheta);
        matRotZ.m[1][1] = cosf(fTheta);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;

        // Rotation X
        matRotX.m[0][0] = 1;
        matRotX.m[1][1] = cosf(fTheta * 0.5f);
        matRotX.m[1][2] = sinf(fTheta * 0.5f);
        matRotX.m[2][1] = -sinf(fTheta * 0.5f);
        matRotX.m[2][2] = cosf(fTheta * 0.5f);
        matRotX.m[3][3] = 1;

        std::vector<triangle> trianglesToRaster;


        //Draw triangles
        for (auto tri : meshCube.tris) {
            triangle projected, translated, rotatedZ, rotatedZX;

            // Rotate in Z-Axis
            MultiplyMatrixVector(tri.p[0], rotatedZ.p[0], matRotZ);
            MultiplyMatrixVector(tri.p[1], rotatedZ.p[1], matRotZ);
            MultiplyMatrixVector(tri.p[2], rotatedZ.p[2], matRotZ);

            // Rotate in X-Axis
            MultiplyMatrixVector(rotatedZ.p[0], rotatedZX.p[0], matRotX);
            MultiplyMatrixVector(rotatedZ.p[1], rotatedZX.p[1], matRotX);
            MultiplyMatrixVector(rotatedZ.p[2], rotatedZX.p[2], matRotX);

            // Offset into the screen
            translated = rotatedZX;
            translated.p[0].z = rotatedZX.p[0].z + 10.0f;
            translated.p[1].z = rotatedZX.p[1].z + 10.0f;
            translated.p[2].z = rotatedZX.p[2].z + 10.0f;


            //Find normal vector to each surface 
            vec3d normal, line1, line2;
            line1.x = translated.p[1].x - translated.p[0].x;
            line1.y = translated.p[1].y - translated.p[0].y;
            line1.z = translated.p[1].z - translated.p[0].z;

            line2.x = translated.p[2].x - translated.p[0].x;
            line2.y = translated.p[2].y - translated.p[0].y;
            line2.z = translated.p[2].z - translated.p[0].z;

            //Find normal using cross product
            normal.x = line1.y * line2.z - line1.z * line2.y;
            normal.y = line1.z * line2.x - line1.x * line2.z;
            normal.z = line1.x * line2.y - line1.y * line2.x;


            //Calculate length to normalize normal vector
            float len = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);

            normal.x /= len;
            normal.y /= len;
            normal.z /= len;

            /*if (normal.z < 0)*/ 
            if (normal.x * (translated.p[0].x - vCamera.x)
                + normal.y * (translated.p[0].y - vCamera.y)
                + normal.z * (translated.p[0].z - vCamera.z)
                < 0
                )
            {
                //Lighting
                vec3d light_direction = { 0.0f, 0.0f, -1.0f };
                float l = sqrtf(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
                light_direction.x /= l;
                light_direction.y /= l;
                light_direction.z /= l;

                float dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;

                CHAR_INFO c = GetColour(dp);
                translated.col = c.Attributes;
                translated.sym = c.Char.UnicodeChar;

                // Project triangles from 3D --> 2D
                MultiplyMatrixVector(translated.p[0], projected.p[0], matProj);
                MultiplyMatrixVector(translated.p[1], projected.p[1], matProj);
                MultiplyMatrixVector(translated.p[2], projected.p[2], matProj);
                projected.col = c.Attributes;
                projected.sym = c.Char.UnicodeChar;

                //Scale into view
                projected.p[0].x += 1.0f;
                projected.p[0].y += 1.0f;

                projected.p[1].x += 1.0f;
                projected.p[1].y += 1.0f;

                projected.p[2].x += 1.0f;
                projected.p[2].y += 1.0f;

                projected.p[0].x *= 0.5f * (float)ScreenWidth();
                projected.p[0].y *= 0.5f * (float)ScreenHeight();

                projected.p[1].x *= 0.5f * (float)ScreenWidth();
                projected.p[1].y *= 0.5f * (float)ScreenHeight();

                projected.p[2].x *= 0.5f * (float)ScreenWidth();
                projected.p[2].y *= 0.5f * (float)ScreenHeight();


                trianglesToRaster.push_back(projected);
            }

        }

        //Sort triangles from farthest to closest to ensure closest triangles are drawn last
        std::sort(trianglesToRaster.begin(), trianglesToRaster.end(), [](triangle &t1, triangle &t2) {
            float z1 = t1.p[0].z + t1.p[1].z + t1.p[2].z;
            float z2 = t2.p[0].z + t2.p[1].z + t2.p[2].z;
            return z1 > z2;
            });


        for (auto& projected : trianglesToRaster) {
            
            FillTriangle(projected.p[0].x, projected.p[0].y,
                projected.p[1].x, projected.p[1].y,
                projected.p[2].x, projected.p[2].y,
                projected.sym, projected.col);

            /*DrawTriangle(projected.p[0].x, projected.p[0].y,
                projected.p[1].x, projected.p[1].y,
                projected.p[2].x, projected.p[2].y,
                PIXEL_SOLID, FG_WHITE);*/
        }


        return true;
    }
};


int main()
{
   
    //std::cout << "He;;p";
    /*std::string h;
    std::cin >> h;*/
    olcEngine3D demo;
    if (demo.ConstructConsole(256, 174, 4, 4)) {
        demo.Start();
    }
    else {
        std::cout << "Could not construct console\n";
        return -1;
    }
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
