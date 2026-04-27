#include<stat_render/materials/Microfacet.h>

Vec3f Microfacet::sample(const Vec3f& wi, const Vec3f& n, SobolSampler& sampler){
    // 从 Sobol 序列获取两个伪随机数，假设接口为 nextFloat()
    float u1 = sampler.get1D();
    float u2 = sampler.get1D();

    float alpha = std::max(roughness * roughness, 0.001f);  // 避免极度光滑时的数值溢出

    // 1. 在切线空间中采样半角向量 h 的极角和方位角
    float phi = 2.0f * Pi * u1;

    // 基于 GGX 概率密度的重要性采样公式
    float cosTheta = std::sqrt((1.0f - u2) / (1.0f + (alpha * alpha - 1.0f) * u2));
    float sinTheta = std::sqrt(std::max(1.0f - cosTheta * cosTheta, 0.0f));

    // 切线空间下的 h
    Vec3f h_tangent(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);

    // 2. 将 h 从切线空间转换到世界空间
    Vec3f t, b;
    BuildTBN(n, t, b);
    Vec3f h = (t * h_tangent.x + b * h_tangent.y + n * h_tangent.z).normalized();

    // 3. 计算出射方向 wo (反射 wi 绕 h)
    float VdotH = dot(wi, h);
    if (VdotH <= 0.0f) {
        return Vec3f(0.0f); // 异常情况，返回零向量处理
    }

    Vec3f wo = (2.0f * VdotH * h - wi).normalized();

    // 确保采样的 wo 在宏观法线同侧 (避免透射)
    if (dot(n, wo) <= 0.0f) {
        return Vec3f(0.0f);
    }

    
    return wo;
}

float Microfacet::pdf(const Vec3f & wi, const Vec3f & wo, const Vec3f& n) {
    // 严格遵循约定：wo 是观察方向 (View)，wi 是采样/光照方向 (Light)
    float NdotV = dot(n, wo);
    float NdotL = dot(n, wi);

    // 检查是否在半球以下 (光线穿模或在背面)
    if (NdotV <= 0.0f || NdotL <= 0.0f) return 0.0f;

    // 安全地计算半角向量 h，避免 NaN
    Vec3f h = wi + wo;
    float lenSq = dot(h, h);
    if (lenSq < 1e-7f) return 0.0f; // 向量退化，防止除以零
    h = h / std::sqrt(lenSq);

    float NdotH = dot(n, h);
    float LdotH = dot(wi, h); // 即光照方向与半角向量的点乘，等同于传统公式中的 VdotH

    // 确保 h 在法线上半球，且光线不与微表面产生自相交
    if (NdotH <= 0.0f || LdotH <= 0.0f) return 0.0f;

    // 限制 alpha 避免过度光滑时 PDF 出现奇点 (趋近于无穷大)
    float alpha = std::max(roughness * roughness, 0.001f);
    
    // 计算法线分布 D
    float D = DistributionGGX(n, h, alpha);

    // 计算采样出半角向量 h 的 PDF
    float pdf_h = D * NdotH;

    // 转换为关于入射方向 wi 立体角的 PDF (除以雅可比行列式 4 * (wi·h))
    float pdf_wi = pdf_h / (4.0f * LdotH);

    return pdf_wi;
}

inline float VisibilitySmithCorrelated(const Vec3f& n, const Vec3f& wi, const Vec3f& wo, float alpha) {
    float NdotL = std::max(dot(n, wi), 0.0f); // wi 是光照方向
    float NdotV = std::max(dot(n, wo), 0.0f); // wo 是观察方向

    float alphaSq = alpha * alpha;

    float Lambda_V = NdotL * std::sqrt(alphaSq + (1.0f - alphaSq) * (NdotV * NdotV));
    float Lambda_L = NdotV * std::sqrt(alphaSq + (1.0f - alphaSq) * (NdotL * NdotL));

    // 返回 0.5 / (...)，完美避开分母趋近于 0 时的数值不稳定
    return 0.5f / (Lambda_V + Lambda_L + 1e-7f); 
}

// 2. 重构后的 eval 函数
Color3f Microfacet::eval(const Vec3f & wi, const Vec3f& wo, const Vec3f & n) {
    // 严格遵循：wo 是观察方向 (View)，wi 是光照方向 (Light)
    float NdotL = dot(n, wi);
    float NdotV = dot(n, wo);

    // 剔除无效光线
    if (NdotV <= 0.0f || NdotL <= 0.0f) {
        return Color3f(0.0f);
    }

    // 安全求半角向量 h
    Vec3f h = wi + wo;
    float lenSq = dot(h, h);
    if (lenSq < 1e-7f) {
        return Color3f(0.0f);
    }
    h = h / std::sqrt(lenSq);

    float VdotH = std::max(dot(wo, h), 0.0f); // V dot H
    float alpha = std::max(roughness * roughness, 0.001f);
    
    // 计算法线分布 D
    float D = DistributionGGX(n, h, alpha);
    
    // 计算合并后的可见性函数 V (替代了原先的 G / (4 * NdotL * NdotV))
    float V = VisibilitySmithCorrelated(n, wi, wo, alpha);
    
    // 计算菲涅尔 F
    Color3f F = FresnelSchlick(VdotH, F0);
    

    // 最终 BRDF 极其简洁，无需再除以任何极小值
    return F * D * V;
}
