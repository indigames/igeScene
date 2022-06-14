#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/Component.h"

namespace ige::scene
{
    //! DirectionalLight
    class DirectionalLight : public Component
    {
    public:
        //! Constructor
        DirectionalLight(SceneObject& owner);

        //! Destructor
        virtual ~DirectionalLight();

        //! Get component name
        virtual std::string getName() const override { return "DirectionalLight"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::DirectionalLight; }

        //! Intensity
        float getIntensity() const;
        void setIntensity(float intensity);

        //! Color
        Vec3 getColor() const;
        void setColor(const Vec3 &color);

        //! Direction
        Vec3 getDirection() const;

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
