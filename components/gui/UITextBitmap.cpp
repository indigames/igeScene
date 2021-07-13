#include "components/gui/UITextBitmap.h"
#include "components/gui/RectTransform.h"
#include "components/gui/UIMask.h"

#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "event/EventContext.h"

namespace ige::scene
{
    //! Constructor
    UITextBitmap::UITextBitmap(SceneObject &owner, const std::string &text, const std::string &fontPath, int fontSize, const Vec4 &color)
        : UIText(owner, text, fontPath, fontSize, color, 1)
    {
    }

    //! Destructor
    UITextBitmap::~UITextBitmap()
    {
    }

    //! Serialize
    void UITextBitmap::to_json(json &j) const
    {
        UIText::to_json(j);
    }

    //! Deserialize
    void UITextBitmap::from_json(const json &j)
    {
        UIText::from_json(j);
    }


    EditableFigure* UITextBitmap::getCurrentFigure() { return getFigure(); }
    SceneObject* UITextBitmap::getSceneObjectOwner() { return getOwner(); }


} // namespace ige::scene
