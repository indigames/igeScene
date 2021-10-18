#include "components/TextBitmapComponent.h"
#include "components/TransformComponent.h"
#include "components/gui/RectTransform.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

#include "utils/GraphicsHelper.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene
{
    //! Constructor
    TextBitmapComponent::TextBitmapComponent(SceneObject &owner, const std::string& text, const std::string& fontPath, int fontSize, const Vec4& color,  bool isBillboard, bool isGUI)
        : TextComponent(owner, text, fontPath, fontSize, color, isBillboard, isGUI, 1)
    {
        
    }

    //! Destructor
    TextBitmapComponent::~TextBitmapComponent()
    {
        if (m_text != nullptr && m_text->getFigure() && getOwner()->getScene()) {
            onResourceRemoved(m_text->getFigure());
        }
        m_text = nullptr;

        if (getOwner()->getTransform())
            getOwner()->getTransform()->makeDirty();
    }

    //! Serialize
    void TextBitmapComponent::to_json(json &j) const
    {
        Component::to_json(j);
        j["text"] = getText();
        j["font"] = getFontPath();
        j["size"] = getFontSize();
        j["color"] = getColor();
        j["billboard"] = isBillboard();
    }

    //! Deserialize
    void TextBitmapComponent::from_json(const json &j)
    {
        m_fontType = 1;
        m_textData = j.value("text", "");
        m_fontPath = j.value("font", "fontBitmaps/dejavu-sans_0.pybm");
        m_fontSize = j.value("size", 12);
        m_color = j.value("color", Vec4(1.f, 1.f, 1.f, 1.f));
        generateText(m_textData, m_fontPath, m_fontSize, m_color, m_fontType);
        setBillboard(j.value("billboard", false));
        Component::from_json(j);
    }

    //! Update property by key value
    void TextBitmapComponent::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("text") == 0)
        {
            setText(val);
        }
        else if (key.compare("font") == 0)
        {
            setFontPath(val);
        }
        else if (key.compare("size") == 0)
        {
            setFontSize(val);
        }
        else if (key.compare("color") == 0)
        {
            setColor(val);
        }
        else if (key.compare("billboard") == 0)
        {
            setBillboard(val);
        }
        else
        {
            Component::setProperty(key, val);
        }
    }
} // namespace ige::scene
