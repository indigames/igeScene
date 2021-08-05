#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/Component.h"

namespace ige::scene
{
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

        //! Render
        void onRender() override;

        //! Get associated figure
        Figure *getFigure() { return m_figure; }

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

        //! Enable alpha blending
        const bool isAlphaBlendingEnable() const { return m_bIsAlphaBlendingEnable; }
        void setAlphaBlendingEnable(bool enable = true);

        //! Alpha blending operation
        const int getAlphaBlendingOp() const { return m_alphaBlendingOp; }
        void setAlphaBlendingOp(int op);

        //! Enable Scissor Test
        const bool isScissorTestEnable() const { return m_bIsScissorTestEnable; }
        void setScissorTestEnable(bool enable = true);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Associated figure object
        Figure *m_figure;

        //! Path to figure file
        std::string m_path;

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

        //! Cache alpha blending state
        bool m_bIsAlphaBlendingEnable = true;

        //! Cache alpha blending operation (COL = 0, ADD = 1, SUB = 2, MUL = 3)
        int m_alphaBlendingOp = 2;

        //! Cache Scissor Test state
        bool m_bIsScissorTestEnable = false;
    };
} // namespace ige::scene
