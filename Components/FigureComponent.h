#pragma once

#include "pyxieFigure.h"

#include "Component.h"

namespace ige::scene
{
    class FigureComponent : public Component
    {
    public:
        FigureComponent(std::shared_ptr<SceneObject> owner);
        virtual ~FigureComponent();

        virtual std::string getName() override { return "FigureComponent"; }

        virtual bool loadFigure(const std::string& path);

        void onUpdate(float dt);

        //! Serialize
        void to_json(json& j, const Component& obj);

        //! Deserialize 
        void from_json(const json& j, Component& obj);

    protected:
        std::string m_resourcePath = "";
        std::shared_ptr<pyxie::pyxieFigure> m_figure = nullptr;
    };
}
