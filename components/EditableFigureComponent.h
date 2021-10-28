#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/FigureComponent.h"

namespace ige::scene
{
    //! EditableFigureComponent
    class EditableFigureComponent : public Component
    {
    public:
        //! Constructor
        EditableFigureComponent(SceneObject &owner, const std::string &path = "");

        //! Destructor
        virtual ~EditableFigureComponent();

        //! Get component name
        virtual std::string getName() const override { return "EditableFigure"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::EditableFigure; }

        //! Update
        void onUpdate(float dt) override;

        //! Render
        void onRender() override;

        //! Enable
        virtual void onEnable() override;

        //! Disable
        virtual void onDisable() override;

        //! Get associated figure
        EditableFigure *getFigure() { return m_figure; }
        void setFigure(EditableFigure* figure);

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
        EditableFigure *m_figure;

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
