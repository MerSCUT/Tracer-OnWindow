#include<iostream>
#include<random>
#include"stat_render/core/Film.h"
#include"stat_render/core/Ray.h"
#include"stat_render/shapes/Sphere.h"
#include"stat_render/core/Camera.h"
#include"stat_render/scenes/Scene.h"
#include"stat_render/shapes/Triangle.h"
#include"stat_render/renderers/Renderer.h"
#include <chrono> 

// For Cuda
#include <cuda_runtime.h>
#define CUDA_CHECK(call) \
    do { \
        cudaError_t err = call; \
        if (err != cudaSuccess) { \
            std::cerr << "CUDA Error: " << cudaGetErrorString(err) \
                      << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

// For Workdir
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#endif
const std::string PROJECT_ROOT = PROJECT_ROOT_DIR;
void test_trace()
{
    
    Film film(resolution, resolution);
    Scene scene;
    std::vector<std::string> paths;
    std::cout << "Asset Path : " << PROJECT_ROOT + "/assert/" << std::endl;

    paths.push_back(PROJECT_ROOT+"/asset/bunny/bunny.obj");
    paths.push_back(PROJECT_ROOT+"/asset/cornellbox/floor.obj");
    paths.push_back(PROJECT_ROOT + "/asset/cornellbox/left.obj");
    paths.push_back(PROJECT_ROOT + "/asset/cornellbox/light.obj");
    paths.push_back(PROJECT_ROOT + "/asset/cornellbox/right.obj");
    paths.push_back(PROJECT_ROOT + "/asset/cornellbox/shortbox.obj");
    paths.push_back(PROJECT_ROOT + "/asset/cornellbox/tallbox.obj");
    

    std::vector<Color3f> emissions(paths.size(), Color3f(0.f,0.f,0.f));
    emissions[3] = Color3f(10.f, 8.5f, 3.0f) ;

    std::vector<DiffuseColor> dcs {
        DiffuseColor::BLUE,
        DiffuseColor::WHITE,
        DiffuseColor::RED,
        DiffuseColor::WHITE,
        DiffuseColor::GREEN,
        DiffuseColor::WHITE,
        DiffuseColor::WHITE
    };




    std::cout << "[Info] 开始导入场景模型..." << std::endl;
    auto start_import = std::chrono::high_resolution_clock::now();
    //====================================================
    scene.loadOBJlist(paths, emissions, dcs);
    //====================================================
    auto end_import = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> import_time = end_import - start_import;
    std::cout << "[Timer] 导入耗时: " << import_time.count() << " 秒\n" << std::endl;

    // camera.transform(scene.getNormalizeMatrix());
    Camera camera(
        Point3f(0.f, 0.f, -3.5f),
        Vec3f(0.f, 0.f, 1.f),
        Vec3f(0.f, 1.f, 0.f),
        film, 45
    );
    Renderer r;

    std::cout << "[Info] 开始执行渲染管线..." << std::endl;
    auto start_render = std::chrono::high_resolution_clock::now();
    //====================================================
    // r.RenderPipeline(scene, film, camera);
    r.RenderMultiThreading(scene, film, camera);
    //====================================================
    auto end_render = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> render_time = end_render - start_render;
    std::cout << "[Timer] 渲染耗时: " << render_time.count() << " 秒\n" << std::endl;

    film.Write(PROJECT_ROOT + "/images/test_VS.ppm");
    std::cout << "[Info] 图片输出 : images/test_VS.ppm" << std::endl;
    return;
}


#include"cudaHeader.h"
int main()
{

    SetConsoleOutputCP(CP_UTF8);

    test_trace();
    return 0;
}

