#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/Component.h"

namespace ige::scene
{
    //! FigureComponent
    class FigureComponent : public Component
    {
    public:
        //! Constructor
        FigureComponent(std::shared_ptr<SceneObject> owner, const std::string& path, Figure* figure = nullptr);

        //! Destructor
        virtual ~FigureComponent();

        //! Get component name
        virtual std::string getName() const override { return "FigureComponent"; }

        //! Update
        void onUpdate(float dt);

        //! Render
        void onRender();

        //! Serialize
        void to_json(json& j, const Component& obj);

        //! Deserialize 
        void from_json(const json& j, Component& obj);

        //! Get associated figure
        Figure* getFigure() { return m_figure; }

    protected:
        //! Associated figure object
        Figure* m_figure = nullptr;
    };
}
