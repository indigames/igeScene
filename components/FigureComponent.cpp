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
            onResourceRemoved(m_figure);
            m_figure->DecReference();
            m_figure = nullptr;
        }
        if (getOwner() && getOwner()->getTransform())
            getOwner()->getTransform()->makeDirty();
    }

    //! Update
    void FigureComponent::onUpdate(float dt)
    {
        if (m_figure == nullptr || !m_figure->IsInitializeSuccess())
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
        if(relPath.size() == 0) relPath = fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');

        if (strcmp(m_path.c_str(), relPath.c_str()) != 0)
        {
            m_path = relPath;

            if (m_figure != nullptr)
            {
                onResourceRemoved(m_figure);
                m_figure->DecReference();
                m_figure = nullptr;
            }

            auto fsPath = fs::path(m_path);
            auto fPath = fsPath.extension().compare(".pyxf") == 0 ? m_path : fsPath.parent_path().append(fsPath.stem().string() + ".pyxf").string();
            if (fPath.size() == 0) fPath = fsPath.string();
            std::replace(fPath.begin(), fPath.end(), '\\', '/');

            m_figure = ResourceCreator::Instance().NewFigure(fPath.c_str());

            // Update transform from transform component
            auto transform = getOwner()->getTransform();
            m_figure->SetPosition(transform->getWorldPosition());
            m_figure->SetRotation(transform->getWorldRotation());
            m_figure->SetScale(transform->getWorldScale());

            // Wait build
            m_figure->WaitBuild();

            // Cache mesh alpha for visibility setting
            m_meshAlphaValues.clear();
            m_disableMeshes.clear();
            for (int i = 0; i < m_figure->NumMeshes(); ++i) {
                m_meshAlphaValues.push_back(m_figure->GetMeshAlpha(i));
            }

            onResourceAdded(m_figure);
            getOwner()->getTransform()->makeDirty();
        }
    }

    //! Set mesh alpha (use to disable rendering of mesh)
    const bool FigureComponent::isMeshEnable(int idx) const {
        return std::find(m_disableMeshes.begin(), m_disableMeshes.end(), idx) == m_disableMeshes.end();
    }

    void FigureComponent::setMeshEnable(int idx, bool enable) {
        auto itr = std::find(m_disableMeshes.begin(), m_disableMeshes.end(), idx);
        if (enable && itr != m_disableMeshes.end()) {
            m_disableMeshes.erase(itr);
            m_figure->SetMeshAlpha(idx, m_meshAlphaValues[idx]);
        }
        else {
            if (itr == m_disableMeshes.end()) {
                m_disableMeshes.insert(idx);
            }
            m_figure->SetMeshAlpha(idx, 0.001f);
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

    //! Enable double side
    void FigureComponent::setDoubleSideEnable(bool enable)
    {
        if (m_bIsDoubleSideEnable != enable)
        {
            m_bIsDoubleSideEnable = enable;
            if (m_figure)
            {
                // Setup point lights shader
                for (int i = 0; i < m_figure->NumMaterials(); ++i)
                {
                    auto shaderDesc = pyxieResourceCreator::Instance().NewShaderDescriptor();
                    shaderDesc->SetValue(m_figure->GetShaderName(i));
                    shaderDesc->SetDoubleside(m_bIsDoubleSideEnable);
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
    //! Enable Scissor Test
    void FigureComponent::setScissorTestEnable(bool enable)
    {
        m_bIsScissorTestEnable = enable;
        if (m_figure)
        {
            // Setup point lights shader
            for (int i = 0; i < m_figure->NumMaterials(); ++i)
            {
                auto shaderDesc = pyxieResourceCreator::Instance().NewShaderDescriptor();
                shaderDesc->SetValue(m_figure->GetShaderName(i));
                shaderDesc->SetScissorTest(m_bIsScissorTestEnable);
                m_figure->SetShaderName(i, shaderDesc->GetValue());
            }
        }
    }

    //! Enable
    void FigureComponent::onEnable()
    {
        if (!getOwner()->isActive() || !isEnabled()) return;
        Component::onEnable();
        onResourceAdded(m_figure);
    }

    //! Disable
    void FigureComponent::onDisable()
    {
        onResourceRemoved(m_figure);
        Component::onDisable();
    }

    //! Utils to add/remove resource
    void FigureComponent::onResourceAdded(Resource* res)
    {
        if (m_bResAdded || res == nullptr) return;
        if (getOwner() && getOwner()->getScene()) {
            getOwner()->getScene()->getResourceAddedEvent().invoke(res);
            m_bResAdded = true;
        }
    }

    void FigureComponent::onResourceRemoved(Resource* res)
    {
        if (!m_bResAdded || res == nullptr) return;
        if (getOwner() && getOwner()->getScene()) {
            getOwner()->getScene()->getResourceRemovedEvent().invoke(res);
            m_bResAdded = false;
        }
    }

    //! Serialize
    void FigureComponent::to_json(json &j) const
    {
        Component::to_json(j);
        j["path"] = getPath();
        j["fog"] = isFogEnabled();
        j["cull"] = isCullFaceEnable();
        j["zTest"] = isDepthTestEnable();
        j["zWrite"] = isDepthWriteEnable();
        j["aBlend"] = isAlphaBlendingEnable();
        j["aBlendOp"] = getAlphaBlendingOp();
        j["doubleSide"] = isDoubleSideEnable();
        j["scissor"] = isScissorTestEnable();
        j["disableMeshes"] = m_disableMeshes;
    }

    //! Deserialize
    void FigureComponent::from_json(const json &j)
    {
        setPath(j.value("path", std::string()));
        setFogEnabled(j.value("fog", false));
        setCullFaceEnable(j.value("cull", true));
        setDepthTestEnable(j.value("zTest", true));
        setDepthWriteEnable(j.value("zWrite", true));
        setAlphaBlendingEnable(j.value("aBlend", true));
        setAlphaBlendingOp(j.value("aBlendOp", 2));
        setDoubleSideEnable(j.value("doubleSide", false));
        setScissorTestEnable(j.value("scissor", false));
        auto disableMeshes = j.value("disableMeshes", std::set<int>());
        for (auto idx: disableMeshes) {
            setMeshEnable(idx, false);
        }
        Component::from_json(j);
    }

    //! Update property by key value
    void FigureComponent::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("path") == 0)
        {
            setPath(val);
        }
        else if (key.compare("fog") == 0)
        {
            setFogEnabled(val);
        }
        else if (key.compare("cull") == 0)
        {
            setCullFaceEnable(val);
        }
        else if (key.compare("zTest") == 0)
        {
            setDepthTestEnable(val);
        }
        else if (key.compare("zWrite") == 0)
        {
            setDepthWriteEnable(val);
        }
        else if (key.compare("aBlend") == 0)
        {
            setAlphaBlendingEnable(val);
        }
        else if (key.compare("aBlendOp") == 0)
        {
            setAlphaBlendingOp(val);
        }
        else if (key.compare("doubleSide") == 0)
        {
            setDoubleSideEnable(val);
        }
        else if (key.compare("scissor") == 0)
        {
            setScissorTestEnable(val);
        }
        else
        {
            Component::setProperty(key, val);
        }
    }
} // namespace ige::scene
