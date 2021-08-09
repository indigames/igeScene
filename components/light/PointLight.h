#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/Component.h"

namespace ige::scene
{
    //! PointLight
    class PointLight : public Component
    {
    public:
        //! Constructor
        PointLight(SceneObject &owner);

        //! Destructor
        virtual ~PointLight();

        //! Get component name
        virtual std::string getName() const override { return "PointLight"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::PointLight; }

        //! Range
        float getRange() const;
        void setRange(float range);

        //! Intensity
        float getIntensity() const;
        void setIntensity(float intensity);

        //! Color
        Vec3 getColor() const;
        void setColor(const Vec3 &col);

        //! Position
        Vec3 getPosition() const;        

        //! Index
        int getIndex() const { return m_index; }

        //! Update
        void onUpdate(float dt) override;

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json &j) const override;

        //! Deserialize
        virtual void from_json(const json &j) override;

        //! Index of light in pyxieEnvironment
        int m_index = -1;
    };
} // namespace ige::scene
