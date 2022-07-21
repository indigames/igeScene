#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/Component.h"

namespace ige::scene
{
    //! AmbientLight
    class AmbientLight : public Component
    {
    public:
        //! Constructor
        AmbientLight(SceneObject& owner);

        //! Destructor
        virtual ~AmbientLight();

        //! Get component name
        virtual std::string getName() const override { return "AmbientLight"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::AmbientLight; }

        //! Sky Color
        Vec3 getSkyColor() const;
        void setSkyColor(const Vec3 &col);

        //! Ground Color
        Vec3 getGroundColor() const;
        void setGroundColor(const Vec3 &col);

        //! Direction
        Vec3 getDirection() const;
        void setDirection(const Vec3 &direction);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;
    };
} // namespace ige::scene
