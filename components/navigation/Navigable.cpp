#include "components/navigation/Navigable.h"

namespace ige::scene
{
    //! Constructor
    Navigable::Navigable(SceneObject &owner)
        : Component(owner)
    {
    }

    Navigable::~Navigable()
    {
    }

    //! Serialize
    void Navigable::to_json(json &j) const
    {
        Component::to_json(j);
        j["recursive"] = isRecursive();
    }

    //! Deserialize
    void Navigable::from_json(const json &j)
    {
        setRecursive(j.value("recursive", true));
        Component::from_json(j);
    }
} // namespace ige::scene
