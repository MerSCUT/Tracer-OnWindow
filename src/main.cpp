#include<iostream>
#include<random>
#include"stat_render/core/Film.h"
#include"stat_render/core/Ray.h"
#include"stat_render/shapes/Sphere.h"
#include"stat_render/core/Camera.h"
#include"stat_render/scenes/Scene.h"
#include"stat_render/shapes/Triangle.h"
#include"stat_render/renderers/Renderer.h"
#include"stat_render/scenes/MeshLoader.h"
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

MeshLoader meshLoader;
void test_trace()
{
    // Render Configuration
    int tileSize = 32;
    int SPP = 16;
    int resolution = 256;


    // Material
    std::shared_ptr<Material> red_Diffuse = std::make_shared<Diffuse>(DiffuseColor::RED, SamplingStrategy::CosineWeighted);
    std::shared_ptr<Material> green_Diffuse = std::make_shared<Diffuse>(DiffuseColor::GREEN, SamplingStrategy::CosineWeighted);
    std::shared_ptr<Material> blue_Diffuse = std::make_shared<Diffuse>(DiffuseColor::BLUE, SamplingStrategy::CosineWeighted);
    std::shared_ptr<Material> white_Diffuse = std::make_shared<Diffuse>(DiffuseColor::WHITE, SamplingStrategy::CosineWeighted);
    std::shared_ptr<Material> emissive = std::make_shared<Emissive>(Color3f(1.f, 1.f, 1.f));
    std::shared_ptr<Material> emissive_brighter = std::make_shared<Emissive>(Color3f(10.f, 8.5f, 3.f));
    std::shared_ptr<Material> golden_Microfacet = std::make_shared<Microfacet>(0.5f, Color3f(1.00, 0.71, 0.29), Color3f(0.f));       // ?

    Film film(resolution, resolution);
    Scene scene;
    std::vector<std::string> paths;
    std::cout << "Asset Path : " << PROJECT_ROOT + "/assert/" << std::endl;

    
    
    
    // Load Bunny
    MeshGeometry bunnyGeo = meshLoader.LoadMeshFromOBJ(PROJECT_ROOT + "/asset/bunny/bunny.obj");
    // =======================Bunny Model Transform
    float bunnyScale = 4.f;
    Transform Scale = Transform::Scale(bunnyScale, bunnyScale, bunnyScale);
    // 2. 旋转变换 (实际代码值为绕 Y 轴旋转 180 度)
    // 注：此处传入的 180.0f 假定你的 Transform::Rotate 内部期望角度制 (Degrees)。
    // 如果你的实现期望弧度制 (Radians)，请改为 Pi 或 180.0f * Pi / 180.0f
    Transform Rotate = Transform::Rotate(180.0f * Pi / 180.f, Vec3f(0.0f, 1.0f, 0.0f));

    // 3. 平移变换 (tx = -0.28, ty = -0.1, tz = -0.3)
    Transform Translate = Transform::Translate(Vec3f(-0.38f, -0.6f, -0.3f));

    // 4. 组合变换：v_world = (T * R * S) * v_local
    Transform bunnyModelTransform = Translate * Rotate * Scale;
    auto bunny = std::make_unique<Mesh>(std::move(bunnyGeo), golden_Microfacet, bunnyModelTransform);
    // Load Cornel Box
    MeshGeometry floorGeo = meshLoader.LoadMeshFromOBJ(PROJECT_ROOT + "/asset/cornellbox/floor.obj");
    MeshGeometry leftGeo = meshLoader.LoadMeshFromOBJ(PROJECT_ROOT + "/asset/cornellbox/left.obj");
    MeshGeometry rightGeo = meshLoader.LoadMeshFromOBJ(PROJECT_ROOT + "/asset/cornellbox/right.obj");
    MeshGeometry lightGeo = meshLoader.LoadMeshFromOBJ(PROJECT_ROOT + "/asset/cornellbox/light.obj");
    MeshGeometry shortboxGeo = meshLoader.LoadMeshFromOBJ(PROJECT_ROOT + "/asset/cornellbox/shortbox.obj");
    MeshGeometry tallboxGeo = meshLoader.LoadMeshFromOBJ(PROJECT_ROOT + "/asset/cornellbox/tallbox.obj");
    
    // CornelBox Transform
    Bound cornelBound;
    cornelBound.Union(floorGeo.localBound);
    cornelBound.Union(leftGeo.localBound);
    cornelBound.Union(rightGeo.localBound);
    cornelBound.Union(lightGeo.localBound);
    cornelBound.Union(shortboxGeo.localBound);
    cornelBound.Union(tallboxGeo.localBound);

    // 先归一化到 -1, 1
    auto cornelM = cornelBound.getNormalizeMatrix();
    auto center = cornelBound.Center();
    float scale = (cornelBound.MaxExtent() > 1e-8f) ? (2.0f / cornelBound.MaxExtent()) : 1.0f;

    Transform cornelTransform = Transform::Scale(scale, scale, scale) * Transform::Translate(-center);



    auto floor = std::make_unique<Mesh>(std::move(floorGeo), white_Diffuse, cornelTransform);
    auto left = std::make_unique<Mesh>(std::move(leftGeo), red_Diffuse, cornelTransform);
    auto right = std::make_unique<Mesh>(std::move(rightGeo), green_Diffuse, cornelTransform);
    auto light = std::make_unique<Mesh>(std::move(lightGeo), emissive_brighter, cornelTransform);
    auto shortbox = std::make_unique<Mesh>(std::move(shortboxGeo), white_Diffuse, cornelTransform);
    auto tallbox = std::make_unique<Mesh>(std::move(tallboxGeo), white_Diffuse, cornelTransform);
    
    
	bunny->ObjectToWorld = bunnyModelTransform;
    // =======================Bunny Model Transform
    auto lightPtr = light.get();

    
    // Sent to Scene
    scene.pushObject(std::move(bunny));
    scene.pushObject(std::move(floor));
    scene.pushObject(std::move(left));
    scene.pushObject(std::move(right));
    scene.pushObject(std::move(light));
    scene.pushObject(std::move(shortbox));
    scene.pushObject(std::move(tallbox));
    
	auto areaLight = std::make_unique<AreaLight>(lightPtr, emissive_brighter->getEmission());
    scene.pushLight(std::move(areaLight));

    scene.BuildTLAS();




    std::cout << "[Info] 开始导入场景模型..." << std::endl;
    auto start_import = std::chrono::high_resolution_clock::now();
    //====================================================
    //scene.loadOBJlist(paths, emissions, dcs);
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
    Renderer r(Mode::PathTracing, tileSize, SPP, resolution);

    std::cout << "[Info] 开始执行渲染管线..." << std::endl;
    std::cout << "[Input] 请输入渲染优化配置 : 1 为普通管线, 2 为多线程分块渲染" << std::endl;
    int op = 1; std::cin >> op;
    
    auto start_render = std::chrono::high_resolution_clock::now();
    //====================================================
    if (op == 1)
    r.RenderPipeline(scene, film, camera);
    else
    r.RenderMultiThreading(scene, film, camera);
    //====================================================
    auto end_render = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> render_time = end_render - start_render;
    std::cout << "[Timer] 渲染耗时: " << render_time.count() << " 秒\n" << std::endl;
    
    film.Write(PROJECT_ROOT + "/images/test_VS.ppm");
    film.WritePNG(PROJECT_ROOT + "/images/test_VS.png");
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

