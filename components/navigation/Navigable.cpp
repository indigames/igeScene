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
} // namespace ige::scene
