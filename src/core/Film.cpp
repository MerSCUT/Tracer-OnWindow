#include"stat_render/core/Film.h"
#include<fstream>
#include<iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include"stb_image_write.h"

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

void Film::WritePNG(std::string path)
{
    // 1. 申请连续内存存储 8-bit RGB 数据
    // 总大小为 宽 x 高 x 3(RGB通道)
    std::vector<unsigned char> image_data(width * height * 3);

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            auto c = frame_buffer[j * width + i];

            // Clamp 限制在 [0, 1]
            c.x = std::clamp(c.x, 0.0f, 1.0f);
            c.y = std::clamp(c.y, 0.0f, 1.0f);
            c.z = std::clamp(c.z, 0.0f, 1.0f);

            // Gamma Correction (Gamma = 2.2)
            c.x = std::pow(c.x, 1.0f / 2.2f);
            c.y = std::pow(c.y, 1.0f / 2.2f);
            c.z = std::pow(c.z, 1.0f / 2.2f);

            // 计算当前像素在 1D 数组中的起始索引
            int index = (j * width + i) * 3;

            // 映射到 [0, 255] 并转换为 unsigned char
            // 建议加上 0.5f 或使用 255.99f 以防止浮点向下取整导致颜色丢失
            image_data[index + 0] = static_cast<unsigned char>(c.x * 255.99f);
            image_data[index + 1] = static_cast<unsigned char>(c.y * 255.99f);
            image_data[index + 2] = static_cast<unsigned char>(c.z * 255.99f);
        }
    }

    // 2. 调用 stb 接口写入文件
    // 参数含义: (文件路径, 宽度, 高度, 通道数(3表示RGB), 数据指针, 每行字节数(stride))
    int stride = width * 3;
    int success = stbi_write_png(path.c_str(), width, height, 3, image_data.data(), stride);

    if (!success) {
        std::cout << "Fail to write PNG file to " << path << std::endl;
    }
}