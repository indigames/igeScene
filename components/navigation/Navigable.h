#pragma once

#include "components/Component.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
    // Class Navigable: mark the object as navigable in the scene
    class Navigable : public Component
    {
    public:
        //! Constructor
        Navigable(SceneObject &owner);

        //! Destructor
        virtual ~Navigable();

        //! Get name
        std::string getName() const override { return "Navigable"; }

        //! Recursive
        bool isRecursive() const { return m_bIsRecursive; }
        void setRecursive(bool recursive) { m_bIsRecursive = recursive; }

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

    protected:
        //! Recursive
        bool m_bIsRecursive = true;
    };
} // namespace ige::scene
