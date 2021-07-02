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

        //Make projection matrix based on screen size, aspect ratio, and fov
        matProj = makeProj(fFov, fAspectRatio, fNear, fFar);


        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {

        //Clear screen
        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);


        mat4x4 matRotZ, matRotX;
        
        fTheta += 1.0f * fElapsedTime;

        // Rotation Z
        matRotZ = makeRotZ(fTheta);

        // Rotation X
        matRotX = makeRotX(fTheta * 0.5);

        std::vector<triangle> trianglesToRaster;

        

        //Create rotation around z and x axis
        mat4x4 rot = (makeRotZ(fTheta) * makeRotX(fTheta * 0.5) )* makeTranslation(0, 0, 10);
        //Draw triangles
        for (auto tri : meshCube.tris) {
            triangle toProj;

            
            

            
            //Rotate triangles
            toProj.p[0] = rot * tri.p[0];
            toProj.p[1] = rot * tri.p[1];
            toProj.p[2] = rot * tri.p[2];


            ////Offset into the screen
            //toProj = toProj;
            //toProj.p[0].z += 10.0f;
            //toProj.p[1].z += 10.0f; 
            //toProj.p[2].z += 10.0f; 


            //Find vector from points 
            vec3d normal, line1, line2;
            line1 = toProj.p[1] - toProj.p[0];
            line2 = toProj.p[2] - toProj.p[0];

            

            //Find normal using cross product
            normal = line1 % line2;


            //Calculate length to normalize normal vector
            normal = normalizeVec(normal);

            //Only project triangles that are visible
            if (normal.x * (toProj.p[0].x - vCamera.x)
                + normal.y * (toProj.p[0].y - vCamera.y)
                + normal.z * (toProj.p[0].z - vCamera.z)
                < 0
                )
            {
                //Lighting
                vec3d light_direction = { 0.0f, 0.0f, -1.0f };
                light_direction = normalizeVec(light_direction);

                float dp = normal * light_direction;

                CHAR_INFO c = GetColour(dp);
                toProj.col = c.Attributes;
                toProj.sym = c.Char.UnicodeChar;

                // Project triangles from 3D --> 2D
                toProj.p[0] = matProj * toProj.p[0];
                toProj.p[1] = matProj * toProj.p[1];
                toProj.p[2] = matProj * toProj.p[2];

                toProj.col = c.Attributes;
                toProj.sym = c.Char.UnicodeChar;

                //Scale into view
                toProj.p[0].x += 1.0f;
                toProj.p[0].y += 1.0f;

                toProj.p[1].x += 1.0f;
                toProj.p[1].y += 1.0f;

                toProj.p[2].x += 1.0f;
                toProj.p[2].y += 1.0f;

                toProj.p[0].x *= 0.5f * (float)ScreenWidth();
                toProj.p[0].y *= 0.5f * (float)ScreenHeight();

                toProj.p[1].x *= 0.5f * (float)ScreenWidth();
                toProj.p[1].y *= 0.5f * (float)ScreenHeight();

                toProj.p[2].x *= 0.5f * (float)ScreenWidth();
                toProj.p[2].y *= 0.5f * (float)ScreenHeight();


                trianglesToRaster.push_back(toProj);
            }

        }

        //Sort triangles from farthest to closest to ensure closest triangles are drawn last
        std::sort(trianglesToRaster.begin(), trianglesToRaster.end(), [](triangle &t1, triangle &t2) {
            float z1 = t1.p[0].z + t1.p[1].z + t1.p[2].z;
            float z2 = t2.p[0].z + t2.p[1].z + t2.p[2].z;
            return z1 > z2;
            });


        for (auto& toProj : trianglesToRaster) {
            
            FillTriangle(toProj.p[0].x, toProj.p[0].y,
                toProj.p[1].x, toProj.p[1].y,
                toProj.p[2].x, toProj.p[2].y,
                toProj.sym, toProj.col);

            /*DrawTriangle(toProj.p[0].x, toProj.p[0].y,
                toProj.p[1].x, toProj.p[1].y,
                toProj.p[2].x, toProj.p[2].y,
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
