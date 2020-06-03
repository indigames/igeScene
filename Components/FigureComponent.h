#pragma once

#include "pyxieFigure.h"
using namespace pyxie;
using Figure = pyxie::pyxieFigure;

#include "Component.h"

namespace ige::scene
{
    //! FigureComponent
    class FigureComponent : public Component
    {
    public:
        //! Constructor
        FigureComponent(std::shared_ptr<SceneObject> owner);

        //! Destructor
        virtual ~FigureComponent();

        //! Get component name
        virtual std::string getName() const override { return "FigureComponent"; }

        //! Get associated figure
        virtual std::shared_ptr<Figure> getFigure() const { return m_figure; }

        //! Load figure from storage
        virtual bool loadFigure(const std::string& path);

        //! Load figure from other figure
        virtual bool loadFromFigure(const Figure& figure);
        
        //! Bind animation
        virtual void bindAnim(Figure::AnimatorSlot slot, const std::string& name);

        //! Set animation time
        virtual void setAnimTime(float time);

        //! Set animation blending weight
        virtual void setAnimBlendWeight(uint32_t part, float weight);
        
        //! Update
        void onUpdate(float dt);

        //! Serialize
        void to_json(json& j, const Component& obj);

        //! Deserialize 
        void from_json(const json& j, Component& obj);

    protected:
        //! Associated figure object
        std::shared_ptr<Figure> m_figure = nullptr;
    };
}
