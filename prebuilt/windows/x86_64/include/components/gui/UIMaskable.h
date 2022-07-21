#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/Component.h"


namespace ige::scene
{
    class UIEventContext;
    class EventContext;

    //! UIImage
    class UIMaskable
    {
    public:
        //! Constructor
        UIMaskable();

        //! Destructor
        virtual ~UIMaskable();

        void setStencilMask(int value = -1);
    protected:
        virtual void onSetParent(EventContext* context);

        virtual EditableFigure* getCurrentFigure() = 0;
        virtual SceneObject* getSceneObjectOwner() = 0;
    };
}
