#include"stat_render/scenes/Scene.h"
#include<iostream>
Mat4f getModelMatrix();

Hit Scene::intersect(const Ray& ray) const
{
    Hit payload;
    for(auto& obj : objects)
    {
        if (Hit temp = obj->intersect(ray); 
            temp.intersected && temp.tmin <= payload.tmin)
        {
            payload = temp;
        }
    }
    return payload;
}
void Scene::loadOBJlist(const std::vector<std::string>& paths, const std::vector<Color3f>& emissions, const std::vector<DiffuseColor>& dcs)
{
    Bound bound;
    assert(paths.size() == emissions.size() && paths.size() == dcs.size());
    // Load all objects
    
    for(int n = 1; n < paths.size(); n++)
    {
        this->loadOBJ(paths[n], bound, emissions[n], dcs[n]);
    }
    // Normalize the entire scene to canonical cubic;
    M_normalize = bound.getNormalizeMatrix();
    bound = Bound();
    for(auto& obj : objects)
    {
        obj->transform(M_normalize);
        
        bound.Union(obj->getBound());
    }
    // std::cout << bound.getPmin().x << ' ' << bound.getPmin().y << ' ' << bound.getPmin().z << std::endl;
    this->loadBunny(paths[0], bound, emissions[0], dcs[0]);
    
    return;
}


void Scene::loadOBJ(const std::string& path,  Bound& boundbox, const Color3f& emission, const DiffuseColor dc)
{
    std::shared_ptr<Material> m;
    if (emission == Color3f(0.f))
    {
        m = std::make_shared<Diffuse>(dc
            // , SamplingStrategy::Uniform
        );
    }
    else
    {
        m = std::make_shared<Emissive>(emission);
    }
    auto mesh = Parser::loadOBJ(path, m);
    AddObject(mesh);
    boundbox.Union(mesh->getBound());
    if (!(emission == Color3f(0.f))) 
    {
        AddLight(std::make_shared<AreaLight>(mesh.get(),emission));
    }
    material_pool.push_back(m);
    return;
}
void Scene::loadBunny(const std::string& path,  Bound& boundbox, const Color3f& emission, const DiffuseColor dc){
    // auto m = std::make_shared<Diffuse>(dc
    //         // , SamplingStrategy::Uniform
    // );

    auto m = std::make_shared<Microfacet>(0.5f, Color3f(1.00, 0.71, 0.29), Color3f(0.f));
    auto mesh = Parser::loadOBJ(path, m);

    auto M = mesh->getBound().getNormalizeMatrix();      // Normalize
    mesh->transform(M);

    mesh->transform(getModelMatrix());
    AddObject(mesh);
    boundbox.Union(mesh->getBound());
    material_pool.push_back(m);
    return;
}


LightSample Scene::sampleLight(SobolSampler& sampler) const
{
    LightSample ls;
    // 假设场景中只有一个光源
    assert(lights.size() == 1);
    auto l = lights[0];
    Vec2f u = sampler.get2D();
    ls = l->sampleLight(u.x, u.y);
    ls.pdf = std::max(ls.pdf, 1e-5f);
    return ls;
    
    // ... 多个光源 :
}

Mat4f getModelMatrix() {
    // 1. 缩放矩阵 (Scale: 0.1)
    float s = 0.3f;
    Mat4f scaleMat(
        s, 0.0f, 0.0f, 0.0f,
        0.0f,    s, 0.0f, 0.0f,
        0.0f, 0.0f,    s, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // 2. 旋转矩阵 (Rotation: 绕 Y 轴旋转 10 度)
    // C++ 中 <cmath> 的三角函数需要传入弧度制
    float angleRad = 180.0f * Pi / 180.0f;
    float cosTheta = std::cos(angleRad);
    float sinTheta = std::sin(angleRad);
    
    // 绕 Y 轴旋转的标准矩阵
    Mat4f rotMat(
         cosTheta, 0.0f, sinTheta, 0.0f,
             0.0f, 1.0f,     0.0f, 0.0f,
        -sinTheta, 0.0f, cosTheta, 0.0f,
             0.0f, 0.0f,     0.0f, 1.0f
    );

    // 3. 平移矩阵 (Translate: 无平移)
    // 根据你的类定义，默认构造函数即为单位矩阵 (Identity)
    // t_z = -0.3 填入第三行、第四列
    float t_x = -0.28f;
    float t_y = -0.1f;
    float t_z = -0.3f;
    Mat4f transMat(
        1.0f, 0.0f, 0.0f,  t_x,
        0.0f, 1.0f, 0.0f,  t_y,
        0.0f, 0.0f, 1.0f,  t_z,
        0.0f, 0.0f, 0.0f,  1.0f
    );

    // 4. 计算并返回最终的模型矩阵 (Model Matrix)
    // 顶点着色时：v_world = (T * R * S) * v_local
    return transMat * rotMat * scaleMat; 
}