#include "components/gui/UIMask.h"
#include "components/gui/UIMaskable.h"

#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "utils/GraphicsHelper.h"

#include "utils/filesystem.h"

#include "event/EventContext.h"

#include "components/tween/Tween.h"
#include "components/tween/Tweener.h"



namespace fs = ghc::filesystem;

NS_IGE_SCENE_BEGIN

UIMask::UIMask(SceneObject& owner, const std::string& texture, const Vec2& size) :
	UIImage(owner, "", size, false, Vec4()), 
    m_bUseMask(true), m_maskDirty(true)
{
    m_bIsInteractable = false;
    getOwner()->setIsInteractable(m_bIsInteractable);
    getOwner()->setIsRaycastTarget(m_bIsInteractable);

    getOwner()->addEventListener((int)EventType::AddChild, std::bind(&UIMask::onAddChild, this, std::placeholders::_1), m_instanceId);
}

UIMask::~UIMask()
{
    getOwner()->removeEventListener((int)EventType::AddChild, m_instanceId);
}

void UIMask::setUseMask(bool value)
{
    if (value != m_bUseMask) {
        m_bUseMask = value;
        m_maskDirty = true;
        updateMask();
    }
}

void UIMask::updateMask()
{
    auto figure = getFigure();
    if (figure == nullptr || !m_maskDirty) return;
    if (m_bUseMask)
    {
        auto shaderDesc = pyxieResourceCreator::Instance().NewShaderDescriptor();
        shaderDesc->SetValue(figure->GetShaderName(0));
        shaderDesc->SetColorMask(false);
        figure->SetShaderName(0, shaderDesc->GetValue());

        int materialIdx = figure->GetMaterialIndex(GenerateNameHash("mate"));

        const ShaderParameterInfo* paramInfo = RenderContext::Instance().GetShaderParameterInfoByName("stencil_test_enable");
        uint32_t stencilEnableVal[4] = { 1,0,0,0 };
        figure->SetMaterialState(materialIdx, (ShaderParameterKey)paramInfo->key, stencilEnableVal);
        const ShaderParameterInfo* maskInfo = RenderContext::Instance().GetShaderParameterInfoByName("stencil_mask");
        uint32_t maskVal[4] = { 2,0,0,0 };
        figure->SetMaterialState(materialIdx, (ShaderParameterKey)maskInfo->key, maskVal);
        const ShaderParameterInfo* funcInfo = RenderContext::Instance().GetShaderParameterInfoByName("stencil_func");
        uint32_t funcVal[4] = { GL_ALWAYS, 2 ,255,0 };
        figure->SetMaterialState(materialIdx, (ShaderParameterKey)funcInfo->key, funcVal);
        const ShaderParameterInfo* opInfo = RenderContext::Instance().GetShaderParameterInfoByName("stencil_op");
        uint32_t opVal[4] = { GL_REPLACE,GL_REPLACE,GL_REPLACE,0 };
        figure->SetMaterialState(materialIdx, (ShaderParameterKey)opInfo->key, opVal);
        /*const ShaderParameterInfo* colorInfo = RenderContext::Instance().GetShaderParameterInfoByName("color_mask");
        uint32_t colorVal[4] = { 0,0,0,1 };
        figure->SetMaterialState(materialIdx, (ShaderParameterKey)colorInfo->key, colorVal);*/

        
    }
    else
    {

    }

    auto children = getOwner()->getChildren();
    int childrenSize = children.size();
    if (childrenSize > 0)
    {
        for (int i = 0; i < childrenSize; i++)
        {
            if(!children[i].expired())
                updateChildMask(children[i].lock());
        }
    }

    m_maskDirty = false;
}

void UIMask::updateChildMask(std::shared_ptr<SceneObject> child)
{
    if (child == nullptr) return;
    
    auto components = child->getComponents();
    for (auto it = components.begin(); it != components.end(); ++it)
    {
        auto markable = std::dynamic_pointer_cast<UIMaskable>(*it);
        if (markable)
            markable->setStencilMask(m_bUseMask ? 1 : -1);
    }

    auto children = child->getChildren();
    int childrenSize = children.size();
    if (childrenSize > 0)
    {
        for (int i = 0; i < childrenSize; i++)
        {
            if(!children[i].expired())
                updateChildMask(children[i].lock());
        }
    }

}

void UIMask::onAddChild(EventContext* context)
{
    context->stopPropagation();
    auto data = context->getDataValue();
    auto uuid = data.asString();
    auto child = getOwner()->findChild(uuid);

    if (child != nullptr) {
        updateChildMask(child);
    }
}

//! Update
void UIMask::onUpdate(float dt)
{
    UIImage::onUpdate(dt);
    updateMask();
}

//! Render
void UIMask::onRender()
{
    UIImage::onRender();
}

void UIMask::setPath(const std::string& path) {
    UIImage::setPath("");
}

//! Enable
void UIMask::onEnable()
{
    setUseMask(true);
}

//! Disable
void UIMask::onDisable() {
    setUseMask(false);
}

//! Json 
void UIMask::to_json(json& j) const
{
    UIImage::to_json(j);
    j["usemask"] = isUseMask();
}

//! Deserialize
void UIMask::from_json(const json& j)
{
    setUseMask((j.value("usemask", true)));
    UIImage::from_json(j);
}

//! Update property by key value
void UIMask::setProperty(const std::string& key, const json& val)
{
    if (key.compare("usemask") == 0) {
        setUseMask(val);
    }
    else {
        UIImage::setProperty(key, val);
    }
}

NS_IGE_SCENE_END
