#include <algorithm>

#include "components/EditableFigureComponent.h"
#include "components/TransformComponent.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene
{
    //! Constructor
    EditableFigureComponent::EditableFigureComponent(SceneObject& owner, const std::string &path)
        : Component(owner), m_figure(nullptr)
    {
        setPath(path);
    }

    //! Destructor
    EditableFigureComponent::~EditableFigureComponent()
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

    void EditableFigureComponent::onUpdate(float dt) {
        if (m_figure == nullptr || !m_figure->IsInitializeSuccess())
            return;

        // Update transform from transform component
        auto transform = getOwner()->getTransform();
        m_figure->SetPosition(transform->getPosition());
        m_figure->SetRotation(transform->getRotation());
        m_figure->SetScale(transform->getScale());

        // Update
        m_figure->Step(dt * getFrameUpdateRatio());

        // Update transform back to transform component
        transform->setPosition(m_figure->GetPosition());
        transform->setRotation(m_figure->GetRotation());
        transform->setScale(m_figure->GetScale());
        if (!getOwner()->isActive())
            pyxie_printf("f Update\n");
    }

    //! Render
    void EditableFigureComponent::onRender()
    {    
    }

    void EditableFigureComponent::setFigure(EditableFigure* figure)
    {
        if (figure == m_figure)
            return;

        m_path.clear();

        if (m_figure != nullptr) {
            onResourceRemoved(m_figure);
            m_figure->DecReference();
            m_figure = nullptr;
        }

        if (figure == nullptr)
            return;

        // Use copy instance
        m_figure = figure;
        m_figure->IncReference();

        m_path = std::string(m_figure->ResourceName());

        // Update transform from transform component
        auto transform = getOwner()->getTransform();
        m_figure->SetPosition(transform->getPosition());
        m_figure->SetRotation(transform->getRotation());
        m_figure->SetScale(transform->getScale());

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
    
    void EditableFigureComponent::setPath(const std::string &path)
    {
        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path)).string() : fsPath.string();
        if(relPath.size() == 0) relPath = fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');

        m_path = relPath;

        if (m_figure != nullptr)
        {
            onResourceRemoved(m_figure);
            m_figure->DecReference();
            m_figure = nullptr;
        }

        fsPath = fs::path(m_path);
        auto fPath = fsPath.extension().compare(".pyxf") == 0 ? m_path : fsPath.parent_path().append(fsPath.stem().string() + ".pyxf").string();
        if (fPath.size() == 0) fPath = fsPath.string();
        std::replace(fPath.begin(), fPath.end(), '\\', '/');

        // Use copy instance
        m_figure = ResourceCreator::Instance().NewEditableFigure(fPath.c_str());

        // Wait build
        while(!m_figure->IsBuidSuccess()) m_figure->WaitBuild();

        // Update transform from transform component
        auto transform = getOwner()->getTransform();
        m_figure->SetPosition(transform->getPosition());
        m_figure->SetRotation(transform->getRotation());
        m_figure->SetScale(transform->getScale());

        // Cache mesh alpha for visibility setting
        m_meshAlphaValues.clear();
        m_disableMeshes.clear();
        for (int i = 0; i < m_figure->NumMeshes(); ++i) {
            m_meshAlphaValues.push_back(m_figure->GetMeshAlpha(i));
        }

        onResourceAdded(m_figure);
        getOwner()->getTransform()->makeDirty();
    }

    //! Set mesh alpha (use to disable rendering of mesh)
    const bool EditableFigureComponent::isMeshEnable(int idx) const {
        return std::find(m_disableMeshes.begin(), m_disableMeshes.end(), idx) == m_disableMeshes.end();
    }

    void EditableFigureComponent::setMeshEnable(int idx, bool enable) {
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
    void EditableFigureComponent::setFogEnabled(bool enable)
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

    //! Enable double side
    void EditableFigureComponent::setDoubleSideEnable(bool enable)
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

    //! Enable cull face
    void EditableFigureComponent::setCullFaceEnable(bool enable)
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

    //! Enable depth testing
    void EditableFigureComponent::setDepthTestEnable(bool enable)
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

    //! Enable depth testing
    void EditableFigureComponent::setDepthWriteEnable(bool enable)
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

    //! Enable Scissor Test
    void EditableFigureComponent::setScissorTestEnable(bool enable)
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
    void EditableFigureComponent::onEnable()
    {
        if (!getOwner()->isActive() || !isEnabled()) return;
        Component::onEnable();
        onResourceAdded(m_figure);
    }

    //! Disable
    void EditableFigureComponent::onDisable()
    {
        onResourceRemoved(m_figure);
        Component::onDisable();
    }

    //! Utils to add/remove resource
    void EditableFigureComponent::onResourceAdded(Resource* res)
    {
        if (m_bResAdded || res == nullptr) return;
        if (getOwner() && getOwner()->isActive(true) && getOwner()->getScene()) {
            getOwner()->getScene()->getResourceAddedEvent().invoke(res);
            m_bResAdded = true;
        }
    }

    void EditableFigureComponent::onResourceRemoved(Resource* res)
    {
        if (!m_bResAdded || res == nullptr) return;
        if (getOwner() && getOwner()->getScene()) {
            getOwner()->getScene()->getResourceRemovedEvent().invoke(res);
            m_bResAdded = false;
        }
    }

    //! Serialize
    void EditableFigureComponent::to_json(json &j) const
    {
        Component::to_json(j);
        j["path"] = getPath();
        j["fog"] = isFogEnabled();
        j["cull"] = isCullFaceEnable();
        j["zTest"] = isDepthTestEnable();
        j["zWrite"] = isDepthWriteEnable();
        j["doubleSide"] = isDoubleSideEnable();
        j["scissor"] = isScissorTestEnable();
        j["updateRatio"] = getFrameUpdateRatio();
        j["disableMeshes"] = m_disableMeshes;
        j["materials"] = m_materials;
    }

    //! Deserialize
    void EditableFigureComponent::from_json(const json &j)
    {
        setPath(j.value("path", std::string()));
        setFogEnabled(j.value("fog", false));
        setCullFaceEnable(j.value("cull", true));
        setDepthTestEnable(j.value("zTest", true));
        setDepthWriteEnable(j.value("zWrite", true));
        setDoubleSideEnable(j.value("doubleSide", false));
        setScissorTestEnable(j.value("scissor", false));
        setFrameUpdateRatio(j.value("updateRatio", 1.f));
        auto disableMeshes = j.value("disableMeshes", std::set<int>());
        for (auto idx: disableMeshes) {
            setMeshEnable(idx, false);
        }
        auto materials = j.value("materials", std::vector<FigureMaterial>());
        for (auto mat : materials) {
            setMaterialParams(mat);
        }
        Component::from_json(j);
    }

    //! Update property by key value
    void EditableFigureComponent::setProperty(const std::string& key, const json& val)
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
        else if (key.compare("doubleSide") == 0)
        {
            setDoubleSideEnable(val);
        }
        else if (key.compare("scissor") == 0)
        {
            setScissorTestEnable(val);
        }
        else if (key.compare("updateRatio") == 0)
        {
            setFrameUpdateRatio(val);
        }
        else
        {
            Component::setProperty(key, val);
        }
    }

    void EditableFigureComponent::setMaterialParams(uint64_t index, uint64_t hash, const Vec4& params) {
        auto itr = std::find_if(m_materials.begin(), m_materials.end(), [index, hash](const auto& elem) {
            return elem.idx == index && elem.hash == hash;
        });
        if(itr != m_materials.end()) {
            (*itr).params = params;
        }
        else {
            m_materials.push_back(FigureMaterial(index, hash, params));
        }
        if (m_figure) {
            m_figure->SetMaterialParam(index, hash, (void*)params.P());
        }
    }

    void EditableFigureComponent::setMaterialParams(uint64_t index, uint64_t hash, const std::string& texPath) {
        auto fsPath = fs::path(texPath);
        fsPath = fsPath.extension().compare(".pyxi") == 0 ? fsPath : fsPath.parent_path().append(fsPath.stem().string() + ".pyxi");

        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(texPath)).string() : fsPath.string();
        if (relPath.size() == 0) relPath = fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');

        auto itr = std::find_if(m_materials.begin(), m_materials.end(), [index, hash](const auto& elem) {
            return elem.idx == index && elem.hash == hash;
        });
        if (itr != m_materials.end()) {
            (*itr).texPath = relPath;
        }
        else {
            m_materials.push_back(FigureMaterial(index, hash, relPath));
        }
        if (m_figure) {
            Sampler sampler;
            sampler.tex = pyxieResourceCreator::Instance().NewTexture(relPath.c_str());
            sampler.samplerSlotNo = 0;
            sampler.samplerState.wrap_s = SamplerState::WRAP;
            sampler.samplerState.wrap_t = SamplerState::WRAP;
            sampler.samplerState.minfilter = SamplerState::LINEAR_MIPMAP_LINEAR;
            sampler.samplerState.magfilter = SamplerState::LINEAR;
            sampler.samplerState.mipfilter = SamplerState::LINEAR_MIPMAP_LINEAR;
            sampler.samplerState.borderColor = 0;
            m_figure->SetMaterialParam(index, hash, &sampler);
        }
    }

    void EditableFigureComponent::setMaterialParams(uint64_t index, const std::string& name, const Vec4& params) {
        setMaterialParams(index, GenerateNameHash(name.c_str()), params);
    }

    void EditableFigureComponent::setMaterialParams(uint64_t index, const std::string& name, const std::string& texPath) {
        setMaterialParams(index, GenerateNameHash(name.c_str()), texPath);
    }

    void EditableFigureComponent::setMaterialParams(FigureMaterial mat) {
        if(!mat.texPath.empty())
            setMaterialParams(mat.idx, mat.hash, mat.texPath);
        else
            setMaterialParams(mat.idx, mat.hash, mat.params);
    }
} // namespace ige::scene
