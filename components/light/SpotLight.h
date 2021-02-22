#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/Component.h"
#include "components/LightComponent.h"

namespace ige::scene
{
    //! SpotLight
    class SpotLight : public LightComponent
    {
    public:
        //! Constructor
        SpotLight(SceneObject &owner);

        //! Destructor
        virtual ~SpotLight();

        //! Get component name
        virtual std::string getName() const override { return "SpotLight"; }

        //! Color
        Vec3 getColor() const;
        void setColor(const Vec3 &col);

        //! Position
        Vec3 getPosition() const;

        //! Direction
        Vec3 getDirection() const;

        //! Angle
        float getAngle() const;
        void setAngle(float angle);

        //! Range
        float getRange() const;
        void setRange(float range);

        //! Intensity
        float getIntensity() const;
        void setIntensity(float intensity);

        //! Index
        int getIndex() const { return m_index; }

        //! Update
        void onUpdate(float dt) override;

    protected:
        //! Serialize
        virtual void to_json(json &j) const override;

        //! Deserialize
        virtual void from_json(const json &j) override;

        //! Index of light in pyxieEnvironment
        int m_index = -1;
    };
} // namespace ige::scene
