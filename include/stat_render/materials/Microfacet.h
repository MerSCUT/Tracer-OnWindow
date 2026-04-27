#include<stat_render/materials/Material.h>
#include<stat_render/core/common.h>
class Microfacet : public Material{
private:
    float roughness = 0;        // [0,1]
    Color3f F0 = 0;             // 基础反射率
    Color3f albedo = Color3f(0.f);
public:
    Microfacet() = default;
    Microfacet(float roughness_, Color3f F0_, Color3f albedo_) : 
    roughness(roughness_), F0(F0_), albedo(albedo_) {}

    Vec3f sample(const Vec3f& wi, const Vec3f& n, SobolSampler& sampler) override;
    
    float pdf(const Vec3f & wi, const Vec3f & wo, const Vec3f& n) override;
    
    Color3f eval(const Vec3f & wi, const Vec3f& wo, const Vec3f & n) override;
private:
    // GGX : NDF
    inline float DistributionGGX(const Vec3f& n, const Vec3f& h, float alpha){
        float NdotH = std::max(dot(n, h), 0.0f);
        float a2 = alpha * alpha;
        float NdotH2 = NdotH * NdotH;

        float num = a2;
        float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);

        denom = Pi * denom * denom;

        return num / std::max(denom, 1e-7f);
    }

    inline float GeometrySchlickGGX(float NdotV, float alpha) {
        float k = alpha / 2.0f;
        float num = NdotV;
        float denom = NdotV * (1.0f - k) + k;
        return num / std::max(denom, 1e-7f);
    }

    // 3. Smith 几何函数 G(wi, wo, h)
    inline float GeometrySmith(const Vec3f& n, const Vec3f& wi, const Vec3f& wo, float alpha) {
        float NdotV = std::max(dot(n, wi), 0.0f);
        float NdotL = std::max(dot(n, wo), 0.0f);
        float ggx2 = GeometrySchlickGGX(NdotV, alpha);
        float ggx1 = GeometrySchlickGGX(NdotL, alpha);
        return ggx1 * ggx2;
    }

    inline float GeometrySmithCorrelated(const Vec3f& n, const Vec3f& wi, const Vec3f& wo, float alpha) {
        float NdotV = std::max(dot(n, wi), 0.0f);
        float NdotL = std::max(dot(n, wo), 0.0f);

        // 剔除光线与视线在表面以下的无效情况
        if (NdotV <= 0.0f || NdotL <= 0.0f) {
            return 0.0f;
        }

        float alphaSq = alpha * alpha;

        // 这里的 Lambda_V 和 Lambda_L 是高度相关公式分母的两部分
        float Lambda_V = NdotL * std::sqrt(alphaSq + (1.0f - alphaSq) * (NdotV * NdotV));
        float Lambda_L = NdotV * std::sqrt(alphaSq + (1.0f - alphaSq) * (NdotL * NdotL));

        // 计算完整的 G 项，加入 1e-7f 防止分母为 0 导致的 NaN
        return (2.0f * NdotL * NdotV) / (Lambda_V + Lambda_L + 1e-7f);
    }

    // 4. Schlick 菲涅尔近似 F(v, h)
    inline Color3f FresnelSchlick(float cosTheta, const Color3f& F0) {
        return F0 + (Color3f(1.0f) - F0) * std::pow(std::clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
    }

    inline void BuildTBN(const Vec3f& n, Vec3f& t, Vec3f& b) {
        if (std::abs(n.x) > 0.99f) {
            t = Vec3f(0.0f, 1.0f, 0.0f);
        } else {
            t = Vec3f(1.0f, 0.0f, 0.0f);
        }
        b = cross(t, n).normalized();
        t = cross(n, b).normalized();
    }

};