#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/Component.h"
#include "components/LightComponent.h"

namespace ige::scene
{
    //! AmbientLight
    class AmbientLight : public LightComponent
    {
    public:
        //! Constructor
        AmbientLight(SceneObject &owner);

        //! Destructor
        virtual ~AmbientLight();

        //! Get component name
        virtual std::string getName() const override { return "AmbientLight"; }

        //! Sky Color
        Vec3 getSkyColor() const;
        void setSkyColor(const Vec3 &col);

        //! Ground Color
        Vec3 getGroundColor() const;
        void setGroundColor(const Vec3 &col);

        //! Direction
        Vec3 getDirection() const;
        void setDirection(const Vec3 &direction);

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;
    };
} // namespace ige::scene
