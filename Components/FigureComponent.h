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
        FigureComponent(std::shared_ptr<SceneObject> owner, const std::string& path = "");

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

        //! Path
        void setPath(const std::string& path);
        const std::string& getPath() { return m_path; }

        //! Events
        Event<Figure*>& getOnFigureCreatedEvent() { return m_onFigureCreatedEvent; }
        Event<Figure*>& getOnFigureDestroyedEvent() { return m_onFigureDestroyedEvent; }

    protected:
        //! Associated figure object
        Figure* m_figure = nullptr;

        //! Events
        Event<Figure*> m_onFigureCreatedEvent;
        Event<Figure*> m_onFigureDestroyedEvent;

        //! Path to figure file
        std::string m_path = "";
    };
}
