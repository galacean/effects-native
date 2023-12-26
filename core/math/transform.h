#ifndef MN_MATH_TRANSFORM_H_
#define MN_MATH_TRANSFORM_H_

#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4.hpp"
#include "util/util.hpp"

namespace mn {

// todo:
class TransformData  {
public:
    virtual ~TransformData() {
        MN_SAFE_DELETE(position_);
        MN_SAFE_DELETE(rotation_);
        MN_SAFE_DELETE(quat_);
        MN_SAFE_DELETE(scale_);
    }

    Vec3* position_ = nullptr;
    Vec4* quat_ = nullptr;
    Vec3* scale_ = nullptr;
    
    void SetRotationData(float x, float y, float z) {
        if (!rotation_) {
            rotation_ = new Vec3;
        }
        rotation_->Set(x, y, z);
    }
    
protected:
    // 禁止外部直接访问
    Vec3* rotation_ = nullptr;
};


// todo：refactor 实现太恶心；
class Transform : public TransformData , public std::enable_shared_from_this<Transform> {
public:
    Transform(TransformData* opt = nullptr);
    
    ~Transform();

    void SetName(const std::string& name) {
        name_ = name;
    }
    
    void SetTransform(TransformData* opt, bool reverse_euler = false);

    void SetPosition(float x, float y, float z);
    
    void SetPosition(const Vec3 pos);

    void SetQuat(float x, float y, float z, float w);
    
    void SetQuat(const Vec4& quat);

    void SetRotation(float x, float y, float z);

    void SetRotation(const Vec3& rot);
    
    void SetScale(float x, float y, float z);
    
    void SetScale(const Vec3& scale);

    void Invalid();

    void Mul(Transform* t);

    void Update();

    void FromMat4(const Mat4& m4, Vec3* scale = nullptr);

    void SetParentTransform(std::shared_ptr<Transform> t);
    
    void GetWorldMatrix(Mat4& mat4);
        
    void GetWorldTRS(Vec3* translate, Vec4* quat, Vec3* scale);

    void GetRotation(Vec3& out);

    bool Dirty();
    
private:
#ifdef UNITTEST
public:
#endif
    Mat4& GetMatrix();

    void GetWorldScale(Vec3& out);

private:
#ifdef UNITTEST
public:
#endif
    std::weak_ptr<Transform> pt_;
    
    std::vector<std::shared_ptr<Transform>> children_;

    Mat4* parent_mat_ = nullptr;

    Mat4* world_mat_ = nullptr;

    Vec3* world_scale_ = nullptr;

    bool dirty_ = true;

    // local matrix;
    Mat4 matrix_;
    
    std::string name_;
};

}

#endif
