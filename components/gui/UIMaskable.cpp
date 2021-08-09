#include "components/gui/UIMaskable.h"
#include "components/gui/RectTransform.h"
#include "components/gui/UIMask.h"
#include "scene/SceneObject.h"
#include "event/EventContext.h"

namespace ige::scene
{
    //! Constructor
    UIMaskable::UIMaskable()
    {
    }

    //! Destructor
    UIMaskable::~UIMaskable()
    {
        
    }

    
    //==================================

    
    void UIMaskable::onSetParent(EventContext* context)
    {
        context->stopPropagation();
        auto parent = getSceneObjectOwner()->getParent();
        if (parent)
        {
            auto mask = parent->getComponent<UIMask>();
            if (mask) {
                bool inMask = parent->isInMask() || (mask != nullptr && mask->isUseMask());
                setStencilMask(inMask ? 1 : -1);
            }
            else 
            {
                bool inMask = parent->isInMask();
                setStencilMask(inMask ? 1 : -1);
            }
        }
    }


    void UIMaskable::setStencilMask(int value)
    {
        auto figure = getCurrentFigure();
        if (figure == nullptr) return; 
        //! If Value < 0 : Disable stencil
        if (value < 0) 
        {
            int materialIdx = figure->GetMaterialIndex(GenerateNameHash("mate"));
            const ShaderParameterInfo* paramInfo = RenderContext::Instance().GetShaderParameterInfoByName("stencil_test_enable");
            uint32_t stencilEnableVal[4] = { 0,0,0,0 };
            figure->SetMaterialState(materialIdx, (ShaderParameterKey)paramInfo->key, stencilEnableVal);
            getSceneObjectOwner()->setInMask(false);
        }
        else
        {
            int materialIdx = figure->GetMaterialIndex(GenerateNameHash("mate"));
            const ShaderParameterInfo* paramInfo = RenderContext::Instance().GetShaderParameterInfoByName("stencil_test_enable");
            uint32_t stencilEnableVal[4] = { 1,0,0,0 };
            figure->SetMaterialState(materialIdx, (ShaderParameterKey)paramInfo->key, stencilEnableVal);
            const ShaderParameterInfo* maskInfo = RenderContext::Instance().GetShaderParameterInfoByName("stencil_mask");
            uint32_t maskVal[4] = { 2,0,0,0 };
            figure->SetMaterialState(materialIdx, (ShaderParameterKey)maskInfo->key, maskVal);
            const ShaderParameterInfo* funcInfo = RenderContext::Instance().GetShaderParameterInfoByName("stencil_func");
            uint32_t funcVal[4] = { GL_EQUAL,2,2,0 };
            figure->SetMaterialState(materialIdx, (ShaderParameterKey)funcInfo->key, funcVal);
            getSceneObjectOwner()->setInMask(true);
        }        
    }

} // namespace ige::scene
