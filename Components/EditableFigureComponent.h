#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/Component.h"

namespace ige::scene
{
    //! EditableFigureComponent
    class EditableFigureComponent : public Component
    {
    public:
        //! Constructor
        EditableFigureComponent(std::shared_ptr<SceneObject> owner, const std::string& path = "", EditableFigure* efig = nullptr);

        //! Destructor
        virtual ~EditableFigureComponent();

        //! Get component name
        virtual std::string getName() const override { return "EditableFigureComponent"; }
        
        //! Update
        void onUpdate(float dt);

        //! Render
        void onRender();

        //! Serialize
        void to_json(json& j, const Component& obj);

        //! Deserialize 
        void from_json(const json& j, Component& obj);

        //! Get associated figure
        EditableFigure* getEditableFigure() { return m_figure; }

        //! Path
        void setPath(const std::string& path);
        const std::string& getPath() { return m_path; }

        //! Events
        Event<EditableFigure*>& getOnFigureCreatedEvent() { return m_onFigureCreatedEvent; }
        Event<EditableFigure*>& getOnFigureDestroyedEvent() { return m_onFigureDestroyedEvent; }

    protected:
        //! Associated EditableFigure object
        EditableFigure* m_figure = nullptr;

        //! Events
        Event<EditableFigure*> m_onFigureCreatedEvent;
        Event<EditableFigure*> m_onFigureDestroyedEvent;

        //! Path
        std::string m_path;
    };
}
