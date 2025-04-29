#include "tgaimage.h"
#include <cmath>
#include <vector>
#include <array>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color) {
    bool steep = std::abs(ax-bx) < std::abs(ay-by);
    if (steep) { // if the line is steep, we transpose the image
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    if (ax>bx) { // make it left−to−right
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    int y = ay;
    int ierror = 0;
    for (int x=ax; x<=bx; x++) {
        if (steep) // if transposed, de−transpose
            framebuffer.set(y, x, color);
        else
            framebuffer.set(x, y, color);
        ierror += 2 * std::abs(by-ay);
        y += (by > ay ? 1 : -1) * (ierror > bx - ax);
        ierror -= 2 * (bx-ax)   * (ierror > bx - ax);
    }
}

struct Obj {
    vector<array<int, 3>> v;
    vector<array<int, 3>> f;
};


int main() {
    constexpr int width  = 1600;
    constexpr int height = 1600;
    TGAImage framebuffer(width, height, TGAImage::RGB);
    string path = "diablo3\\diablo3_pose.obj";
    string l;
    ifstream file(path);
    Obj obj;
    while (getline(file, l)) {
        if (l[0] == 'v' && l[1] == ' ') {
            istringstream iss(l.substr(2)); // line.substr(2) extract a substr starting from index 2
            float x, y, z;
            iss >> x >> y >> z;
            int xScaled = (x+1) * (width/2);
            int yScaled = (y+1) * (height/2);
            int zScaled = (z+1); // * (depth/2)
             
            obj.v.push_back({xScaled, yScaled, zScaled});
        } else if (l[0] == 'f') {
            istringstream iss(l.substr(2)); // line.substr(2) extract a substr starting from index 2
            int v1, v2, v3;
            char slash;
            int discard;
            iss >> v1 >> slash >> discard >> slash >> discard
                >> v2 >> slash >> discard >> slash >> discard
                >> v3;
            v1=v1-1; v2=v2-1; v3=v3-1;
            obj.f.push_back({v1,v2,v3});
        }
    }
    // Set the points in each vertex
    /*    
    for (auto i: obj.v) {
        framebuffer.set(i[0], i[1], white);
    }
    */
    for (auto i: obj.f) {
        line(obj.v[i[0]][0], obj.v[i[0]][1], obj.v[i[1]][0], obj.v[i[1]][1], framebuffer, red);
        line(obj.v[i[1]][0], obj.v[i[1]][1], obj.v[i[2]][0], obj.v[i[2]][1], framebuffer, red);
        line(obj.v[i[0]][0], obj.v[i[0]][1], obj.v[i[2]][0], obj.v[i[2]][1], framebuffer, red);  
    } 

    framebuffer.write_tga_file("hw1.tga");
    
    return 0;
}


