
#include "components/RectTransform.h"

namespace ige::scene 
{
    RectTransform::RectTransform(std::shared_ptr<SceneObject> owner, const Vec3& pos, const Quat& rot, const Vec3& scale)
        : TransformComponent(owner, pos, rot, scale)
    {
    }

    RectTransform::~RectTransform()
    {
    }

    void RectTransform::setPosition(const Vec3& pos)
    {

    }

    void RectTransform::setRotation(const Quat& rot)
    {

    }

    void RectTransform::setScale(const Vec3& scale)
    {

    }

    void RectTransform::setAnchor(const Anchor& anchor) 
    { 
        m_anchor = anchor;
    }

    void RectTransform::setPivot(const Vec2& pivot) 
    { 
        m_pivot = pivot;
    }

    void RectTransform::setOffset(const Offset& offset) 
    { 
        m_offset = offset;
    }

    void RectTransform::setSize(const Vec2& size) 
    { 
        m_size = size;
    }    
}
