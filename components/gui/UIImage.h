#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/SpriteComponent.h"
#include "components/gui/GUITypes.h"

namespace ige::scene
{

    //! UIImage
    class UIImage : public SpriteComponent
    {
    public:
        //! Constructor
        UIImage(SceneObject &owner, const std::string &texture = "", const Vec2 &size = {128.f, 128.f});

        //! Destructor
        virtual ~UIImage();

        //! Get component name
        virtual std::string getName() const override { return "UIImage"; }

        //! Update
        virtual void onUpdate(float dt) override;

        //! Render
        virtual void onRender() override;

        //! FillMethod
        void setFillMethod(int value);
        const FillMethod& getFillMethod() const { return m_sprite->getFillMethod(); }

        //! FillOrigin
        void setFillOrigin(int value);
        const FillOrigin& getFillOrigin() const { return m_sprite->getFillOrigin(); }
        
        //! FillAmount
        void setFillAmount(float value);
        const float getFillAmount()  const { return m_sprite->getFillAmount(); }
        
        //! Clockwise
        void setClockwise(bool value);
        const bool getClockwise() const { return m_sprite->getClockwise(); }
        
    protected:
        


        //! Serialize
        virtual void to_json(json &j) const override;

        //! Deserialize
        virtual void from_json(const json &j) override;
    };
}
