#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/Component.h"

namespace ige::scene
{
    //! UIImage
    class UIImage : public Component
    {
    public:
        //! Constructor
        UIImage(const std::shared_ptr<SceneObject>& owner, const std::string& texture = "", const Vec2& size = {128.f, 128.f});

        //! Destructor
        virtual ~UIImage();

        //! Get component name
        virtual std::string getName() const override { return "UIImage"; }

        //! Update
        void onUpdate(float dt);

        //! Render
        void onRender();

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

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
        EditableFigure* m_figure;

        //! Events
        Event<EditableFigure*> m_onFigureCreatedEvent;
        Event<EditableFigure*> m_onFigureDestroyedEvent;

        //! Path
        std::string m_path;
        Vec2 m_size;
    };
}
