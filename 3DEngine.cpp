// 3DEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <fstream>
#include <strstream>
#include <algorithm>
#include "olcConsoleGameEngineGL.h"
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
    vec3d lookDir;
    float fTheta;

    float yaw;


    int clipTrianglePlane(vec3d point, vec3d normal, triangle &in, triangle &out1, triangle &out2) {
        //Insure normal vector is a unit vector
        normal = normalizeVec(normal);

        //Return signed (shortest) distance from point to plane
        auto getDist = [&](vec3d & p) {
            return (p * normal - point * normal);
        };

        //Group points depending on whether they lie above or below a plane (inside or outside the boundaries of the screen)
        vec3d* insidePoints[3];
        vec3d* outsidePoints[3];
        int numInside = 0;
        int numOutside = 0;

        //Calculate distances from each point of the triangle we passed in to the plane of reference
        float distances[3];
        for (int p = 0; p < 3; p++) {
            distances[p] = getDist(in.p[p]);
        }
        //Group points into different categories defined above (inside and outside)
        for (int d = 0; d < 3; d++) {
            if (distances[d] >= 0) {
                insidePoints[numInside++] = &in.p[d];
            }
            else {
                outsidePoints[numOutside++] = &in.p[d];
            }
        }

        //Classify triangle passed in (outside, inside, partially inside)
        if (!numInside) {
            return 0;
        }
        if (!numOutside) {
            //If all points are inside (the whole triangle is inside) set the output to equal the input (no clipping necessary)
            out1 = in;
            return 1;
        }
        if (numInside == 1) //Only 1 point inside, need to reconstruct triangle
        {
            //Copy color info into reconstructed triangle
            out1.sym = in.sym;
            out1.col = FG_RED;

            out1.p[0] = *insidePoints[0];
            out1.p[1] = vecPlaneInter(point, normal, *insidePoints[0], *outsidePoints[0]);
            out1.p[2] = vecPlaneInter(point, normal, *insidePoints[0], *outsidePoints[1]);
            
            return 1;
        }
        if (numInside == 2) //Two points inside, need to reconstruct triangle into 2 new triangles
        {
            //Copy color info into reconstructed triangles
            out2.sym = out1.sym = in.sym;
            out2.col =  FG_BLUE;
            out1.col = FG_GREEN;

            //Construct first new triangle
            out1.p[0] = *insidePoints[0];
            out1.p[1] = *insidePoints[1];
            out1.p[2] = vecPlaneInter(point, normal, *insidePoints[1], *outsidePoints[0]);

            //Construct second triangle
            out2.p[0] = out1.p[2];
            out2.p[1] = vecPlaneInter(point, normal, *insidePoints[0], *outsidePoints[0]);
            out2.p[2] = *insidePoints[0];

            return 2;

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


        meshCube.LoadFromOBJFile("3DModel.obj");

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

        //Move camera
        //Up/down
        if (GetKey(VK_UP).bHeld) {
            vCamera.y -= 8.0f * fElapsedTime;
        }
        if (GetKey(VK_DOWN).bHeld) {
            vCamera.y += 8.0f * fElapsedTime;
        }
        //Find relative left and right directions
        vec3d right = { 0, 1, 0 };
        right = right % lookDir;
        //Left/right
        if (GetKey(VK_LEFT).bHeld) {
            vCamera = vCamera - (8.0f * fElapsedTime * right);
        }
        if (GetKey(VK_RIGHT).bHeld) {
            vCamera = vCamera + (8.0f * fElapsedTime * right);
        }

        //Forward/backwards
        vec3d forward = lookDir * 8.0f * fElapsedTime;
        
        if (GetKey(L'W').bHeld) {
            vCamera = vCamera + forward;
        }
        if (GetKey(L'S').bHeld) {
            vCamera = vCamera - forward;
        }

        if (GetKey(L'A').bHeld) {
            yaw -= 2.0f * fElapsedTime;
        }
        if (GetKey(L'D').bHeld) {
            yaw += 2.0f * fElapsedTime;
        }
        if (GetKey(L'X').bHeld) {
            fTheta += 2.0f * fElapsedTime;
        }

        
        
        //Clear screen
        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_GREY);


        mat4x4 matRotZ, matRotX, translate;
        
        //fTheta += 1.0f * fElapsedTime;

        
        //Make translation matrix
        translate = makeTranslation(0, 0, 20);

        
        vec3d up = { 0, 1, 0 };
        vec3d target = { 0, 0, 1 };
        lookDir = makeRotY(yaw) * target;
        target = vCamera + lookDir;

        mat4x4 matCam = pointAt(vCamera, target, up);

        mat4x4 matView = Matrix_QuickInverse(matCam);

        std::vector<triangle> trianglesToRaster;

        //Create rotation around z and x axis and translate
        //std::cout << fTheta << std::endl;
        mat4x4 rot = makeRotX(180);
        //mat4x4 rot = makeRotZ(0) * makeRotX(0) * makeRotY(0);

        rot = rot * translate;

        //Draw triangles
        //#pragma omp parallel for
        for (auto const &tri : meshCube.tris) {
            triangle toProj;

            
            

            
            //Rotate triangles
            toProj.p[0] = rot * tri.p[0];
            toProj.p[1] = rot * tri.p[1];
            toProj.p[2] = rot * tri.p[2];


            


            //Find vector from points 
            vec3d normal, line1, line2;
            line1 = toProj.p[1] - toProj.p[0];
            line2 = toProj.p[2] - toProj.p[0];


            //Find normal using cross product
            normal = line1 % line2;


            //Create unit normal vector
            normal = normalizeVec(normal);


            //Only project triangles that are visible
            if (normal * (toProj.p[0] - vCamera)< 0)
            {
                //Lighting
                vec3d light_direction = { 0.0f, -1.0f, -1.0f };
                //light_direction = makeRotY(fTheta) * light_direction;
                light_direction = normalizeVec(light_direction);

                float dp = normal * light_direction;

                CHAR_INFO c = GetColour(dp);
                toProj.col = c.Attributes;
                toProj.sym = c.Char.UnicodeChar;

                //Convert world space to camera space
                toProj.p[0] = matView * toProj.p[0];
                toProj.p[1] = matView * toProj.p[1];
                toProj.p[2] = matView * toProj.p[2];

                //Clip triangles that get too close to camera to improve performance (no longer dividing by numbers approaching 0)
                triangle clipped[2];
                int numClipped = clipTrianglePlane({ 0, 0, 0.1f }, {0, 0, 1}, toProj, clipped[0], clipped[1]);

                for (int t = 0; t < numClipped; t++) {
                    toProj = clipped[t];
                    // Project triangles from 3D --> 2D
                    toProj.p[0] = matProj * toProj.p[0];
                    toProj.p[1] = matProj * toProj.p[1];
                    toProj.p[2] = matProj * toProj.p[2];

                    


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

        }

        //Sort triangles from farthest to closest to ensure closest triangles are drawn last
        std::sort(trianglesToRaster.begin(), trianglesToRaster.end(), [](triangle &t1, triangle &t2) {
            float z1 = t1.p[0].z + t1.p[1].z + t1.p[2].z;
            float z2 = t2.p[0].z + t2.p[1].z + t2.p[2].z;
            return z1 > z2;
            });


        /*for (auto& toProj : trianglesToRaster) {

            FillTriangle(toProj.p[0].x, toProj.p[0].y,
                toProj.p[1].x, toProj.p[1].y,
                toProj.p[2].x, toProj.p[2].y,
                toProj.sym, toProj.col);

            DrawTriangle(toProj.p[0].x, toProj.p[0].y,
                toProj.p[1].x, toProj.p[1].y,
                toProj.p[2].x, toProj.p[2].y,
                PIXEL_SOLID, FG_BLACK);
        }*/

        //Draw triangles to screen
        for (auto& toRaster : trianglesToRaster) {
            

            triangle clipped[2];
            std::list<triangle> q;
            q.push_back(toRaster);
            int newTris = 1;

            for (int edge = 0; edge < 4; edge++) {
                int trisToAdd = 0;

                while (newTris > 0) {
                    triangle test = q.front();
                    q.pop_front();
                    newTris--;

                    switch (edge) 
                    {
                        //Top Edge
                    case 0: trisToAdd = clipTrianglePlane({ 0, 0, 0 }, {0, 1, 0}, test, clipped[0], clipped[1]); 
                        
                        break;
                        //Right edge
                    case 1: trisToAdd = clipTrianglePlane({ (float)ScreenWidth() - 1, 0, 0 }, {-1, 0, 0}, test, clipped[0], clipped[1]);
                        
                        break;
                        //Bottom edge
                    case 2: trisToAdd = clipTrianglePlane({ 0, (float)ScreenHeight() - 1, 0}, {0, -1, 0}, test, clipped[0], clipped[1]);
                        
                        break;
                        //Left edge
                    case 3: trisToAdd = clipTrianglePlane({ 0, 0, 0 }, {1, 0, 0}, test, clipped[0], clipped[1]);
                         
                        break;
                    }
                    
                    for (int newT = 0; newT < trisToAdd; newT++) {
                        q.push_back(clipped[newT]);
                        
                    }
                    
                }
                newTris = q.size();
            }
            
            for (auto& toProj : q) {
                FillTriangle(toProj.p[0].x, toProj.p[0].y,
                    toProj.p[1].x, toProj.p[1].y,
                    toProj.p[2].x, toProj.p[2].y,
                    toProj.sym, toProj.col);

               /* DrawTriangle(toProj.p[0].x, toProj.p[0].y,
                    toProj.p[1].x, toProj.p[1].y,
                    toProj.p[2].x, toProj.p[2].y,
                    PIXEL_SOLID, FG_BLACK);*/
            }
            

            
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
