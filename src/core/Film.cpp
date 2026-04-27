#include"stat_render/core/Film.h"
#include<fstream>
#include<iostream>

// 自定义输出路径
void Film::Write(std::string path)
{
    std::ofstream f(path, std::ios::out);
    if (!f) {std::cout << "Fail to open file !" << std::endl; return; }

    f << "P3\n" << width << ' ' << height << "\n255\n"; 
    for(int j = 0; j < height; j++)
    {
        for(int i = 0; i < width; i++)
        {
            auto c = frame_buffer[j*width + i];
            c.x = std::clamp(c.x, 0.0f, 1.0f);
            c.y = std::clamp(c.y, 0.0f, 1.0f);
            c.z = std::clamp(c.z, 0.0f, 1.0f);
            // Gamma Correction
            c.x = std::pow(c.x, 1.0f / 2.2f);
            c.y = std::pow(c.y, 1.0f / 2.2f);
            c.z = std::pow(c.z, 1.0f / 2.2f);
            f << c[0] * 255.f << ' ' << c[1]*255.f << ' ' << c[2]*255.f << '\n';
        }
    }
    f.close();
    return;
} 