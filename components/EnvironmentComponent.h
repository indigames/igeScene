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
        virtual std::string getName() const override { return "Environment"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::Environment; }

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

        //! Shadow bias
        float getShadowBias() const;
        void setShadowBias(float wideness);

        //! Shadow texture size
        const Vec2& getShadowTextureSize() const;
        void setShadowTextureSize(const Vec2& size);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;
    };
} // namespace ige::scene
