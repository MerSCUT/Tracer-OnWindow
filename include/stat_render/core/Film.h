#pragma once

#include"stat_render/core/common.h"


class Film
{
private:
    int width, height;
    std::vector<Color3f> frame_buffer;
public:
    Film() : width(64), height(64) { frame_buffer.resize(width * height); }
    Film(int w, int h) : width(w), height(h)
    {
        frame_buffer.resize(width * height);
    }
    ~Film() = default;
    void add(int i, int j, Color3f color) { frame_buffer[j*width + i] += color; }
    void set(int i, int j, Color3f color) { frame_buffer[j*width + i] = color; }

    // Getter
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    // Write in .ppm
    void Write() { Write(output_path); }
    void Write(std::string path);
};


