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
// void test_outputImage()
// {
//     Film film(32,32);
//     //film.OutputImage();
//     return;
// }
// void test_triangle()
// {
//     Triangle tri;//(Point3f(-0.5f, -0.5f, -4.0), Point3f(0.5f, -0.5f, -8.0f), Point3f(0.0f, 0.5f , -4.0f));
//     Film film;
//     // default set
//     Camera camera(film);
//     Scene scene;
//     Object* obj = &tri;
//     scene.AddObject(obj);
    
    
//     auto p = camera.getPosition();
//     auto fov = camera.getFov();
//     auto asp = camera.getAspect();
//     float l,r,t,b;
//     float dis = 1.0f;

//     // Ray generation
//     t = dis * std::tan(deg2rad(fov * 0.5f));
//     r = t * asp; 
//     l = -r;
//     b = -t;
//     // Integrator
//     for(int j = 0; j < film.getHeight(); j++)
//     {
//         for(int i = 0; i < film.getWidth(); i++)
//         {
//             // center of pixels
//             float u = l + (float)(i+0.5)/(float)(film.getWidth()) * (r-l);
//             float v = b + (float)(j+0.5)/(float)(film.getHeight()) * (t-b);
//             v = -v;
//             // Ray generation
//             Ray ray(Point3f(p), Vec3f(u, v, -1));
            
//             if (Hit payload = tri.intersect(ray); payload.intersected){
                
//                 film.set(i, j, Color3f(255., 0.,0.));
//             }
//             else{
//                 film.set(i,j, Color3f(0., 0., 255.));
//             }
//         }
//     }
//     film.Write("../images/test_triangle.ppm");
// }
// void test_sphere()
// {
//     Sphere sph( Point3f(0.f, 0.f, -2.f), 0.5f);
//     Film film;
//     // default set
//     Camera camera(film);
//     Scene scene;
//     Object* obj = &sph;
//     scene.AddObject(obj);
    
    
//     auto p = camera.getPosition();
//     auto fov = camera.getFov();
//     auto asp = camera.getAspect();
//     float l,r,t,b;
//     float dis = 1.0f;

//     // Ray generation
//     t = dis * std::tan(deg2rad(fov * 0.5f));
//     r = t * asp; 
//     l = -r;
//     b = -t;
//     // Integrator
//     for(int j = 0; j < film.getHeight(); j++)
//     {
//         for(int i = 0; i < film.getWidth(); i++)
//         {
//             // center of pixels
//             float u = l + (float)(i+0.5)/(float)(film.getWidth()) * (r-l);
//             float v = b + (float)(j+0.5)/(float)(film.getHeight()) * (t-b);
//             v = -v;
//             // Ray generation
//             Ray ray(Point3f(p), Vec3f(u, v, -1));

//             if (Hit payload = sph.intersect(ray); payload.intersected){
//                 film.set(i, j, Color3f(255., 0.,0.));
//             }
//             else{
//                 film.set(i,j, Color3f(0., 0., 255.));
//             }
//         }
//     }
//     film.Write("../images/test_sphere.ppm");
// }

// void test_loadOBJ()
// {
//     Scene scene;
//     std::string path("../asset/bunny/bunny.obj");
//     scene.loadOBJ(path);
//     Bound b = scene.getObjects()[0]->getBound();
//     std::cout << b.Center() << std::endl;
//     std::cout << "Pmin : " << b.getPmin() << std::endl;
//     std::cout << "Pmax : " << b.getPmax() << std::endl;
//     Film film(64,64);
//     Point3f position(-0.016826f, 0.110153f, 0.4f);
//     // 视线右侧方向向量 (X轴正向)
//     Vec3f right(1.0f, 0.0f, 0.0f);

//     // 视线顶上的方向 (Y轴正向)
//     Vec3f up(0.0f, 1.0f, 0.0f);
//     Camera camera(position, right, up, film);
//     Renderer renderer(scene, film, camera);
//     renderer.RenderPipeline(scene, film);

//     film.Write("../images/test_OBJloadeAndRenderer.ppm");

//     std::cout << "Finish" << std::endl;
//     return;
// }

// void test_BVH()
// {
//     Scene scene;
//     std::string path("../asset/bunny/bunny.obj");
//     scene.loadOBJ(path);
//     Bound b = scene.getObjects()[0]->getBound();
    
//     Film film(512,512);
//     Point3f position(0.f, 0.f, 3.0f);
//     // 视线方向
//     Vec3f gaze(0.0f, -1.0f, -1.0f);

//     // 视线顶上的方向 (Y轴正向)
//     Vec3f up(0.0f, 1.0f, 0.0f);
//     Camera camera(position, gaze, up, film);
//     Renderer renderer(scene, film, camera, Mode::NormalMap);
//     renderer.RenderPipeline(scene, film);

//     film.Write("../images/test_BVH.ppm");

//     std::cout << "Finish" << std::endl;
//     return;
// }
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

    // Loading




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
#include"cudaHeader.h"
int main()
{
    const int size = 1024;
    const size_t bytes = size * sizeof(float);

    // 1. 初始化 Host (CPU) 数据
    std::vector<float> h_data(size);
    for (int i = 0; i < size; ++i) {
        h_data[i] = static_cast<float>(i);
    }

    // 2. 分配 Device (GPU) 内存
    float* d_data = nullptr;
    CUDA_CHECK(cudaMalloc(&d_data, bytes));

    // 3. 将数据从 Host 拷贝到 Device
    CUDA_CHECK(cudaMemcpy(d_data, h_data.data(), bytes, cudaMemcpyHostToDevice));

    // 4. 调用包装函数
    // 该函数在 math_kernel.cu 中实现，在 math_kernel.h 中声明
    std::cout << "Launching CUDA kernel via wrapper..." << std::endl;
    runCudaMathKernel(d_data, size);

    // 检查 Kernel 启动是否出错（异步错误捕获）
    CUDA_CHECK(cudaGetLastError());
    // 同步设备，确保计算完成
    CUDA_CHECK(cudaDeviceSynchronize());

    // 5. 将结果拷贝回 Host
    CUDA_CHECK(cudaMemcpy(h_data.data(), d_data, bytes, cudaMemcpyDeviceToHost));

    // 6. 验证部分结果
    std::cout << "Preview of results (first 5 elements):" << std::endl;
    for (int i = 0; i < 5; ++i) {
        std::cout << "Index [" << i << "]: " << h_data[i] << std::endl;
    }

    // 7. 释放资源
    CUDA_CHECK(cudaFree(d_data));

    std::cout << "Execution completed successfully." << std::endl;
    return 0;


    SetConsoleOutputCP(CP_UTF8);
    test_trace();
    return 0;
    
    std::random_device rd;
    unsigned int seed = rd();
    std::mt19937 gen(seed);

    std::uniform_real_distribution<> dis_real(0.0, 1.0);
    for(int i = 0; i < 10; i++)
    {
        std::cout << " " << gen() % 100;
    }
    std::cout << '\n';
    //return 0;
    std::cout << "Test_Finished" << std::endl;
    return 0;
}


