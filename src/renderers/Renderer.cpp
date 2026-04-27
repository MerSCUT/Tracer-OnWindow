#include"stat_render/renderers/Renderer.h"
#include"stat_render/core/transform.h"
#include<iostream>
#include<thread>
#include<mutex>
#include<atomic>
#include<stat_render/samplers/QMC.h>
const int tile_size = 32;        // Tile size
const int SPP = 32;              // samples number per pixel

float PowerHeuristic(float f_pdf, float g_pdf) {
    float f2 = f_pdf * f_pdf;
    float g2 = g_pdf * g_pdf;
    return f2 / (f2 + g2);
}

void Renderer::RenderMultiThreading(const Scene& scene, Film& film, const Camera& camera)
{
    float dis = 1.0f;
    float t = dis * std::tan(deg2rad(camera.getFov() * 0.5f));
    float r = t * camera.getAspect(); 
    float l = -r;
    float b = -t;

    int spp = SPP;                                  
    int w = film.getWidth();
    int h = film.getHeight();
    int total = w * h;
    std::cout << "[Info] Mode : Multi-Threading" << std::endl;
    std::cout << "[Info] Tile size : " << tile_size << std::endl;
    std::cout << "[Info] SPP : " << SPP << std::endl;
    std::cout << "[Info] Resolution : " << resolution << " * " << resolution << std::endl; 

    struct Tile{ 
        int row_st, col_st, row_ed, col_ed;         // [st, ed)
    };
    // 渲染分块
    std::vector<Tile> tiles;
    for(int j = 0; j < h; j += tile_size)
    {
        for(int i = 0; i < w; i += tile_size)
        {
            tiles.push_back(
                { i, j, std::min(i + tile_size, w), std::min(j + tile_size, h) }
            );
        }
    }

    std::atomic<int> globalProgress{0};
    int nextTile = 0;
    std::mutex tileMutex;

    auto worker = [&]()
    {
        while(1)
        {
            // 从 tiles 中取出下一个
            Tile tile;
            {
                std::unique_lock<std::mutex> lock{tileMutex};
                if (nextTile >= tiles.size()) break;
                tile = tiles[nextTile++];
            }
            int localProgress = 0;

            for(int j = tile.col_st; j < tile.col_ed; j++)
            {
                for(int i = tile.row_st; i < tile.row_ed; i++)
                {
                    float u = l + (float)(i+0.5f)/(float)(film.getWidth()) * (r-l);
                    float v = -b - (float)(j+0.5f)/(float)(film.getHeight()) * (t-b);
                    // Ray generation in Camera Standard Space
                    Vec3f dir = Vec3f(u, v, -1);
                    // Transform to world space;
                    Mat4f M = CameraToWorldTransform(camera.getPosition(), camera.getGaze(), camera.getTop());
                    Vec4f ret = M * toVec4D(dir);
                    dir = Vec3f(ret.x, ret.y, ret.z).normalized();
                    Ray ray(Point3f(camera.getPosition()), dir);
                    // Start rendering
                    Color3f result(0.f);
                    for(int s = 0; s < spp; s++)
                    {
                        SobolSampler sampler(s + 1, i, j);
                        Color3f res = CastRay(ray, scene, 0, sampler)/spp;
                        result += res;
                    }
                    film.add(i,j, result);
                    localProgress++;
                }
                
            }
            globalProgress += localProgress;
            localProgress = 0;
        }
        
    };

    int numThreads = std::thread::hardware_concurrency() - 10;
    std::cout << "[Info] Find " << numThreads << " kernels" << std::endl;
    std::vector<std::thread> threads;
    for(int k = 0; k < numThreads; k++)
    {
        threads.emplace_back(worker);
    }

    while (globalProgress < total) {
        int current = globalProgress.load();
        float percent = (float)current / total * 100.0f;
        std::cout << "\r进度: " << current << " / " << total 
                  << " (" << percent << "%)    " << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 0.1s interval
    }
    std::cout << "\r进度: " << total << " / " << total 
                  << " (" << 100.00 << "%)    " << std::endl;
    for(int i = 0; i < numThreads; i++)
    {
        threads[i].join();
    }
    
    return;
}

void Renderer::RenderPipeline(const Scene& scene, Film& film, const Camera& camera)
{
    float dis = 1.0f;
    float t = dis * std::tan(deg2rad(camera.getFov() * 0.5f));
    float r = t * camera.getAspect(); 
    float l = -r;
    float b = -t;

    int spp = SPP;          // from common.h
    int progress = 0;
    std::cout << "[Info] Mode : Single-Threading" << std::endl;
    std::cout << "[Info] SPP : " << SPP << std::endl;
    std::cout << "[Info] Resolution : " << resolution << " * " << resolution << std::endl; 
    
    for(int j = 0; j < film.getHeight(); j++)
    {
        for(int i = 0; i < film.getWidth(); i++)
        {
            
            float u = l + (float)(i+0.5f)/(float)(film.getWidth()) * (r-l);
            float v = -b - (float)(j+0.5f)/(float)(film.getHeight()) * (t-b);
            // Ray generation
            Vec3f dir = Vec3f(u, v, -1);
            auto M = CameraToWorldTransform(camera.getPosition(), camera.getGaze(), camera.getTop());
            Vec4f ret = M * toVec4D(dir);
            dir = Vec3f(ret.x, ret.y, ret.z).normalized();
            Ray ray(Point3f(camera.getPosition()), dir);

            for(int s = 0; s < spp; s++)
            {
                SobolSampler sampler(s + 1, i, j);
                auto res = CastRay(ray, scene, 0, sampler);
                
                film.add(i,j, res/(float)spp);
            }
            
            
            progress++;
            std::cout << "\r进度: " << progress << " / " << film.getHeight() * film.getWidth() << std::flush;
        }
    }
    std::cout << std::endl;
}



Color3f Renderer::CastRay(const Ray& ray, const Scene& scene, int depth, SobolSampler& sampler)
{
    if (depth > 25) return Color3f(0.f, 0.f, 0.f);
    if (Hit payload = scene.intersect(ray);  
    payload.intersected)
    {
        switch (mode)
        {
        case Mode::PathTracing :
        {
            // 光源
            Object* ob = payload.obj;
            ob->getBound();
            Material* w = payload.obj->getMaterial();
            if (payload.obj->getMaterial()->isEmissive())
            {
                if (depth == 0)
                return payload.obj->getMaterial()->getEmission();
                else 
                return Color3f(0.f,0.f,0.f);
            }
            
            // 非光源
            // 1. 直接光照
            Color3f L_dir(0.0f, 0.0f, 0.0f);
            LightSample ls = scene.sampleLight(sampler);
            auto p = payload.position;
            auto l = ls.position;
            Vec3f wi = (l - p).normalized();    // outwards
            auto n_p = payload.normal;
            auto n_l = ls.normal;
            // 相交测试
            Ray test(p + Epsilon * n_p, wi);

            auto hit_test = scene.intersect(test);
            // if ((test(hit_test.tmin) - payload.position).norm() > (ls.position - payload.position).norm())
            
            if (!hit_test.intersected || (test(hit_test.tmin) - p).norm() + Epsilon >= (l-p).norm()) 
            {
                
                Vec3f wo = payload.incident.normalized();       // outwards
                
                // 根据 payload.material 计算 L_o
                // L_o = fr * L_i * dot(n, wi) * dot(-n_l, wi) / dot(p-l, p-l) / ls.pdf_A
                auto fr = payload.material->eval(wi, wo, n_p);
                auto Li = ls.radiance;

                
                auto cos_thetai = dot(n_p, wi);
                cos_thetai = cos_thetai > 0.0f ? cos_thetai : 0.0f;
                auto cos_thetaip = std::abs(dot(n_l, wi));
                

                //assert(cos_thetaip >= 0.f);
                auto dis = dot(p-l, p-l);
                //assert(cos_thetai >= 0.0f);
                //assert(cos_thetaip >= 0.0f);
                assert(ls.pdf > 0.0f && "光源采样信息错误");
                L_dir = fr * Li * cos_thetai * cos_thetaip / (dis * ls.pdf) ;
                int kkk;
            }

            Color3f L_indir(0.f, 0.f, 0.f);
            // Russian Roulette
            float p_rr = 0.8f;
            
            auto u = sampler.get1D(); 
            if (u >= p_rr) return L_dir;
            Vec3f wo = (ray.origin - p);
            if (wo.norm2() <= 0.f){
                return L_dir;
            }
            wo = wo.normalized();
            // 采样入射方向 wi (局部)
            Vec3f wi_ind = payload.material->sample(wo, n_p, sampler);
            if (wi_ind.norm2() <= 0.f){
                return L_dir;
            }
            float pdf_ind = payload.material->pdf(wi_ind, wo, n_p);
            pdf_ind = std::max(pdf_ind, 1e-3f);

            Ray ray_ind(p + Epsilon * n_p, wi_ind);
            
            auto fr = payload.material->eval(wi_ind, wo, n_p);
            auto costhetai = dot(n_p, wi_ind);
            auto Li = CastRay(ray_ind, scene, depth+1, sampler);
            L_indir = fr * Li * costhetai / (pdf_ind * p_rr);
            
            
            return L_dir + L_indir;
            
            
            break;
        }
            
        // normal
        case (Mode::NormalMap) :
        {
            auto n = payload.normal;
            // [-1,1] -> [0,1]
            
            float R = n.x * 0.5f + 0.5f;
            float G = n.y * 0.5f + 0.5f;
            float B = n.z * 0.5f + 0.5f;
            
            return Color3f(255. * R, 255. * G,255. * B);

            break;
        }
            
            
        }
        return Color3f(1.f, 0.f, 0.f);
    }
    return Color3f(0.f, 0.f, 0.f);
    return Color3f(199.f/255.f,233.f/255.f,233.f/255.f);
}

