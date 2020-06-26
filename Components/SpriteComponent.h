#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/Component.h"

namespace ige::scene
{
    //! SpriteComponent
    class SpriteComponent : public Component
    {
    public:
        //! Constructor
        SpriteComponent(std::shared_ptr<SceneObject> owner, const Vec2& size = {32.f, 32.f}, const std::string& texture = "");

        //! Destructor
        virtual ~SpriteComponent();

        //! Get component name
        virtual std::string getName() const override { return "SpriteComponent"; }
        
        //! Update
        void onUpdate(float dt);

        //! Render
        void onRender();

        //! Serialize
        void to_json(json& j, const Component& obj);

        //! Deserialize 
        void from_json(const json& j, Component& obj);

        //! Get associated figure
        EditableFigure* getFigure() { return m_figure; }

        //! Path
        void setPath(const std::string& path);
        const std::string& getPath() { return m_path; }

        //! Size
        void setSize(const Vec2& size);
        const Vec2& getSize() { return m_size; }

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
        Vec2 m_size;
    };
}
