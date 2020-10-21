#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/Component.h"

namespace ige::scene
{
    //! FigureComponent
    class EnvironmentComponent : public Component
    {
    public:
        //! Constructor
        EnvironmentComponent(SceneObject &owner);

        //! Destructor
        virtual ~EnvironmentComponent();

        //! Get component name
        virtual std::string getName() const override { return "EnvironmentComponent"; }

        //! Fog near distance
        float getDistanceFogNear() const;
        void setDistanceFogNear(float f_near);

        //! Fog far distance
        float getDistanceFogFar() const;
        void setDistanceFogFar(float f_far);

        //! Fog alpha distance
        float getDistanceFogAlpha() const;
        void setDistanceFogAlpha(float alpha);

        //! Fog color distance
        Vec3 getDistanceFogColor() const;
        void setDistanceFogColor(const Vec3 &col);

        //! Shadow color
        Vec3 getShadowColor() const;
        void setShadowColor(const Vec3 &col);

        //! Shadow density
        float getShadowDensity() const;
        void setShadowDensity(float density);

        //! Shadow wideness
        float getShadowWideness() const;
        void setShadowWideness(float wideness);

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;
    };
} // namespace ige::scene
