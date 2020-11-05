#include <algorithm>

#include "components/FigureComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene
{
    //! Constructor
    FigureComponent::FigureComponent(SceneObject &owner, const std::string &path)
        : Component(owner), m_figure(nullptr)
    {
        setPath(path);
    }

    //! Destructor
    FigureComponent::~FigureComponent()
    {
        if (m_figure)
        {
            if (getOwner()->getScene())
                getOwner()->getScene()->getResourceRemovedEvent().invoke(m_figure);
            m_figure->DecReference();
            m_figure = nullptr;
        }
    }

    //! Update
    void FigureComponent::onUpdate(float dt)
    {
        if (m_figure == nullptr)
            return;

        // Update transform from transform component
        auto transform = getOwner()->getTransform();
        m_figure->SetPosition(transform->getWorldPosition());
        m_figure->SetRotation(transform->getWorldRotation());
        m_figure->SetScale(transform->getWorldScale());

        // Update
        m_figure->Step(dt);

        // Update transform back to transform component
        transform->setWorldPosition(m_figure->GetPosition());
        transform->setWorldRotation(m_figure->GetRotation());
        transform->setWorldScale(m_figure->GetScale());
    }

    //! Update
    void FigureComponent::onRender()
    {
        if (m_figure == nullptr)
            return;

        m_figure->Render();
    }

    void FigureComponent::setPath(const std::string &path)
    {
        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');

        if (strcmp(m_path.c_str(), relPath.c_str()) != 0)
        {
            m_path = relPath;

            if (m_figure != nullptr)
            {
                getOwner()->getScene()->getResourceRemovedEvent().invoke(m_figure);
                m_figure->DecReference();
                m_figure = nullptr;
            }

            m_figure = ResourceCreator::Instance().NewFigure(m_path.c_str());
            m_figure->WaitInitialize();

            // Setup lights shader
            for (int i = 0; i < m_figure->NumMaterials(); ++i)
            {
                auto shaderDesc = pyxieResourceCreator::Instance().NewShaderDescriptor();
                shaderDesc->SetValue(m_figure->GetShaderName(i));
                shaderDesc->SetNumPointLamp(MAX_POINT_LIGHT_NUMBER);
                shaderDesc->SetNumSpotLamp(MAX_SPOT_LIGHT_NUMBER);
                m_figure->SetShaderName(i, shaderDesc->GetValue());
            }

            getOwner()->getScene()->getResourceAddedEvent().invoke(m_figure);
        }
    }

    //! Enable fog
    void FigureComponent::setFogEnabled(bool enable)
    {
        if (m_bIsFogEnabled != enable)
        {
            m_bIsFogEnabled = enable;

            if (m_figure)
            {
                // Setup point lights shader
                for (int i = 0; i < m_figure->NumMaterials(); ++i)
                {
                    auto shaderDesc = pyxieResourceCreator::Instance().NewShaderDescriptor();
                    shaderDesc->SetValue(m_figure->GetShaderName(i));
                    shaderDesc->SetFog(m_bIsFogEnabled);
                    m_figure->SetShaderName(i, shaderDesc->GetValue());
                }
            }
        }
    }

    //! Enable cull face
    void FigureComponent::setCullFaceEnable(bool enable)
    {
        if (m_bIsCullFaceEnable != enable)
        {
            m_bIsCullFaceEnable = enable;
            if (m_figure)
            {
                // Setup point lights shader
                for (int i = 0; i < m_figure->NumMaterials(); ++i)
                {
                    auto shaderDesc = pyxieResourceCreator::Instance().NewShaderDescriptor();
                    shaderDesc->SetValue(m_figure->GetShaderName(i));
                    shaderDesc->SetFrontFaceCulling(m_bIsCullFaceEnable);
                    m_figure->SetShaderName(i, shaderDesc->GetValue());
                }
            }
        }
    }

    //! Enable depth testing
    void FigureComponent::setDepthTestEnable(bool enable)
    {
        if (m_bIsDepthTestEnable != enable)
        {
            m_bIsDepthTestEnable = enable;
            if (m_figure)
            {
                // Setup point lights shader
                for (int i = 0; i < m_figure->NumMaterials(); ++i)
                {
                    auto shaderDesc = pyxieResourceCreator::Instance().NewShaderDescriptor();
                    shaderDesc->SetValue(m_figure->GetShaderName(i));
                    shaderDesc->SetZTest(m_bIsDepthTestEnable);
                    m_figure->SetShaderName(i, shaderDesc->GetValue());
                }
            }
        }
    }

    //! Enable depth testing
    void FigureComponent::setDepthWriteEnable(bool enable)
    {
        if (m_bIsDepthWriteEnable != enable)
        {
            m_bIsDepthWriteEnable = enable;
            if (m_figure)
            {
                // Setup point lights shader
                for (int i = 0; i < m_figure->NumMaterials(); ++i)
                {
                    auto shaderDesc = pyxieResourceCreator::Instance().NewShaderDescriptor();
                    shaderDesc->SetValue(m_figure->GetShaderName(i));
                    shaderDesc->SetZWritee(m_bIsDepthWriteEnable);
                    m_figure->SetShaderName(i, shaderDesc->GetValue());
                }
            }
        }
    }

    //! Enable alpha blending
    void FigureComponent::setAlphaBlendingEnable(bool enable)
    {
        if (m_bIsAlphaBlendingEnable != enable)
        {
            m_bIsAlphaBlendingEnable = enable;
            if (m_figure)
            {
                // Setup point lights shader
                for (int i = 0; i < m_figure->NumMaterials(); ++i)
                {
                    auto shaderDesc = pyxieResourceCreator::Instance().NewShaderDescriptor();
                    shaderDesc->SetValue(m_figure->GetShaderName(i));
                    shaderDesc->SetAlphaBlend(m_bIsAlphaBlendingEnable);
                    m_figure->SetShaderName(i, shaderDesc->GetValue());
                }
            }
        }
    }

    //! Alpha blending operation
    void FigureComponent::setAlphaBlendingOp(int op)
    {
        // Only 4 operations supported now
        if (op < 0 || op > 4)
            op = m_alphaBlendingOp;

        if (m_alphaBlendingOp != op)
        {
            m_alphaBlendingOp = op;
            if (m_figure)
            {
                // Setup point lights shader
                for (int i = 0; i < m_figure->NumMaterials(); ++i)
                {
                    auto shaderDesc = pyxieResourceCreator::Instance().NewShaderDescriptor();
                    shaderDesc->SetValue(m_figure->GetShaderName(i));
                    shaderDesc->SetAlphaBlendOP(m_alphaBlendingOp);
                    m_figure->SetShaderName(i, shaderDesc->GetValue());
                }
            }
        }
    }

    //! Serialize
    void FigureComponent::to_json(json &j) const
    {
        j = json{
            {"path", getPath()},
            {"fog", isFogEnabled()},
            {"cull", isCullFaceEnable()},
            {"ztest", isDepthTestEnable()},
            {"zwrite", isDepthWriteEnable()},
            {"aBlend", isAlphaBlendingEnable()},
            {"aBlendOp", getAlphaBlendingOp()},
        };
    }

    //! Deserialize
    void FigureComponent::from_json(const json &j)
    {
        setPath(j.value("path", std::string()));
        setFogEnabled(j.value("fog", false));
        setCullFaceEnable(j.value("cull", true));
        setDepthTestEnable(j.value("ztest", true));
        setDepthWriteEnable(j.value("zwrite", true));
        setAlphaBlendingEnable(j.value("aBlend", true));
        setAlphaBlendingOp(j.value("aBlendOp", 2));
    }
} // namespace ige::scene
