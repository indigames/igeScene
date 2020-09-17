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
        FigureComponent(const std::shared_ptr<SceneObject>& owner, const std::string& path = "");

        //! Destructor
        virtual ~FigureComponent();

        //! Get component name
        virtual std::string getName() const override { return "FigureComponent"; }

        //! Update
        void onUpdate(float dt);

        //! Render
        void onRender();

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        //! Get associated figure
        Figure* getFigure() { return m_figure; }

        //! Path
        void setPath(const std::string& path);
        const std::string& getPath() { return m_path; }

    protected:
        //! Associated figure object
        Figure* m_figure;

        //! Path to figure file
        std::string m_path;
    };
}
