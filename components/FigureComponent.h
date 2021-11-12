#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/Component.h"

namespace ige::scene
{
    class FigureMaterial {
    public:
        FigureMaterial():
            idx((uint64_t)-1), hash((uint64_t)-1) {}

        FigureMaterial(uint64_t idx, uint64_t hash, const Vec4& params)
            : idx(idx), hash(hash), params(params) {}

        FigureMaterial(uint64_t idx, uint64_t hash, const std::string& path)
            : idx(idx), hash(hash), texPath(path) {}

    public:
        uint64_t idx;
        uint64_t hash;
        Vec4 params;
        std::string texPath;

    protected:

        //! Serialize
        friend void to_json(json& j, const FigureMaterial& obj)
        {
            j["idx"] = obj.idx;
            j["hash"] = obj.hash;
            if (!obj.texPath.empty()) j["tex"] = obj.texPath;
            else j["params"] = obj.params;
        }

        //! Deserialize
        friend void from_json(const json& j, FigureMaterial& obj)
        {
            obj.idx = j.value("idx", (uint64_t)-1);
            obj.hash = j.value("hash", (uint64_t)-1);
            obj.params = j.value("params", Vec4());
            obj.texPath = j.value("tex", std::string());
        }
    };

    //! FigureComponent
    class FigureComponent : public Component
    {
    public:
        //! Constructor
        FigureComponent(SceneObject &owner, const std::string &path = "");

        //! Destructor
        virtual ~FigureComponent();

        //! Get component name
        virtual std::string getName() const override { return "Figure"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::Figure; }

        //! Update
        void onUpdate(float dt) override;
        void onAlwaysUpdate(float dt) override;

        //! Render
        void onRender() override;

        //! Enable
        virtual void onEnable() override;

        //! Disable
        virtual void onDisable() override;

        //! Get associated figure
        Figure *getFigure() { return m_figure; }
        void setFigure(Figure* figure);

        //! Path
        void setPath(const std::string &path);
        const std::string &getPath() const { return m_path; }

        //! Enable fog
        const bool isFogEnabled() const { return m_bIsFogEnabled; }
        void setFogEnabled(bool enable = true);

        //! Enable double side
        const bool isDoubleSideEnable() const { return m_bIsDoubleSideEnable; }
        void setDoubleSideEnable(bool enable = true);

        //! Enable cull face
        const bool isCullFaceEnable() const { return m_bIsCullFaceEnable; }
        void setCullFaceEnable(bool enable = true);

        //! Enable depth test
        const bool isDepthTestEnable() const { return m_bIsDepthTestEnable; }
        void setDepthTestEnable(bool enable = true);

        //! Enable depth write
        const bool isDepthWriteEnable() const { return m_bIsDepthWriteEnable; }
        void setDepthWriteEnable(bool enable = true);

        //! Enable Scissor Test
        const bool isScissorTestEnable() const { return m_bIsScissorTestEnable; }
        void setScissorTestEnable(bool enable = true);

        //! Set mesh alpha (use to disable rendering of mesh)
        const bool isMeshEnable(int idx) const;
        void setMeshEnable(int idx, bool enable = true);

        //! Frame update ratio (speedup/slower effects)
        float getFrameUpdateRatio() const { return m_frameUpdateRatio; };
        void setFrameUpdateRatio(float ratio) { m_frameUpdateRatio = ratio; }

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

        //! Set material params
        virtual void setMaterialParams(uint64_t index, uint64_t hash, const Vec4& params);
        virtual void setMaterialParams(uint64_t index, const std::string& name, const Vec4& params);
        virtual void setMaterialParams(uint64_t index, uint64_t hash, const std::string& texPath);
        virtual void setMaterialParams(uint64_t index, const std::string& name, const std::string& texPath);
        virtual void setMaterialParams(FigureMaterial mat);

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Utils to add/remove resource
        void onResourceAdded(Resource* figure);
        void onResourceRemoved(Resource* figure);
        bool m_bResAdded = false;

        //! Associated figure object
        Figure *m_figure;

        //! Path to figure file
        std::string m_path;

        //! Frame update ratio (speedup/slower effects)
        float m_frameUpdateRatio = 1.f;

        //! Cache fog state
        bool m_bIsFogEnabled = false;

        //! Cache culling state
        bool m_bIsCullFaceEnable = true;

        //! Cache double side state
        bool m_bIsDoubleSideEnable = true;

        //! Cache z-testing state
        bool m_bIsDepthTestEnable = true;

        //! Cache z-write state
        bool m_bIsDepthWriteEnable = true;

        //! Cache Scissor Test state
        bool m_bIsScissorTestEnable = false;

        //! Cache mesh alpha state for visibility
        std::vector<float> m_meshAlphaValues;
        std::set<int> m_disableMeshes;

        //! Cache material settings
        std::vector<FigureMaterial> m_materials;
    };
} // namespace ige::scene
