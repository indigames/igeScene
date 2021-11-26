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
        Component::to_json(j);
        j["text"] = getText();
        j["font"] = getFontPath();
        j["size"] = getFontSize();
        j["color"] = getColor();
        j["alignhorizontal"] = getTextAlignHorizontal();
        j["alignvertical"] = getTextAlignVertical();
    }

    //! Deserialize
    void UITextBitmap::from_json(const json &j)
    {
        m_fontType = 1;
        m_textAlignHorizontal = (j.value("alignhorizontal", 0));
        m_textAlignVertical = (j.value("alignvertical", 0));
        setText(j.at("text"));
        setFontPath(j.at("font"));
        setFontSize(j.at("size"));
        setColor(j.at("color"));
        Component::from_json(j);
    }

    //! Text
    void UITextBitmap::setText(const std::string& text)
    {
        m_textData = text;
        if (m_text == nullptr) {
            generateText(m_textData, m_fontPath, m_fontSize, m_color, m_fontType);
        }
        else {
            auto oldFigure = m_text->getFigure();
            m_text->setText(text, m_fontType);
            auto newFigure = m_text->getFigure();
            if (oldFigure != newFigure)
            {
                if (oldFigure)
                    onResourceRemoved(oldFigure);
                if (newFigure)
                    onResourceAdded(newFigure);
            }
        }
        getOwner()->getTransform()->makeDirty();
    }


    EditableFigure* UITextBitmap::getCurrentFigure() { return getFigure(); }
    SceneObject* UITextBitmap::getSceneObjectOwner() { return getOwner(); }
} // namespace ige::scene
