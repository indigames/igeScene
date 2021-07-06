#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/physic/PhysicObject.h"

namespace ige::scene
{
    //! PhysicBox
    class PhysicBox : public PhysicObject
    {
    public:
        //! Constructor
        PhysicBox(SceneObject &owner, const Vec3 &size = {1.f, 1.f, 1.f});

        //! Destructor
        virtual ~PhysicBox();

        //! Get name
        std::string getName() const override { return "PhysicBox"; }

        //! Get size
        const Vec3 &getSize() const;

        //! Set size
        void setSize(const Vec3 &size);

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json &j) const override;

        //! Deserialize
        virtual void from_json(const json &j) override;

        //! Create collision shape
        void createCollisionShape(const Vec3 &size);

        //! Recreate collision shape once size changed
        void recreateCollisionShape(const Vec3 &size);

    protected:
        //! Box size
        Vec3 m_size;
    };
} // namespace ige::scene