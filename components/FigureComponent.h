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
        virtual std::string getName() const override { return "FigureComponent"; }

        //! Update
        void onUpdate(float dt) override;

        //! Render
        void onRender() override;

        //! Get associated figure
        Figure *getFigure() { return m_figure; }

        //! Path
        void setPath(const std::string &path);
        const std::string &getPath() { return m_path; }

        //! Enable fog
        const bool isFogEnabled() { return m_bIsFogEnabled; }
        void setFogEnabled(bool enable = true);

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
    };
} // namespace ige::scene
