#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/physic/PhysicBase.h"

namespace ige::scene
{
    //! PhysicBox
    class PhysicBox : public PhysicBase
    {
    public:
        //! Constructor
        PhysicBox(const std::shared_ptr<SceneObject> &owner, const Vec3& size = {1.f, 1.f, 1.f});

        //! Destructor
        virtual ~PhysicBox();

        //! Get name
        std::string getName() const override { return "PhysicBox"; }

        //! Get size
        Vec3& getSize();

        //! Set size
        void setSize(const Vec3& size);

    protected:
        //! Create collision shape
        void createCollisionShape(const Vec3& size);

        //! Recreate collision shape once size changed
        void recreateCollisionShape(const Vec3& size);

        //! Set local scale of the box
        virtual void setLocalScale(const Vec3& scale) override;

    protected:
        Vec3 m_size;
    };
}