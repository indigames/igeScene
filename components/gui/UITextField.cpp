#include "components/gui/UITextField.h"
#include "components/gui/RectTransform.h"

#include "scene/SceneObject.h"

namespace ige::scene
{
    UITextField::UITextField(SceneObject &owner, const std::string &text)
        : UIText(owner, text)
    {
    }

    //! Destructor
    UITextField::~UITextField()
    {
    }

    //! Update
    void UITextField::onUpdate(float dt)
    {
        if (!getOwner()->isSelected())
        {
            if (getOwner()->getParent() == nullptr || !getOwner()->getParent()->isSelected())
            {
                if (m_bIsInputing)
                {
                    StopTextInput();
                    m_bIsInputing = false;
                }
            }
        }
        if (m_bIsInputing)
        {
            int size;
            UIText::setText(std::string(GetInputText(&size)));
        }
        UIText::onUpdate(dt);
    }

    //! Click
    void UITextField::onClick()
    {
        StartTextInput(getText().c_str());
        m_bIsInputing = true;
    }

    //! Serialize
    void UITextField::to_json(json &j) const
    {
        UIText::to_json(j);
    }

    //! Deserialize
    void UITextField::from_json(const json &j)
    {
        UIText::from_json(j);
    }
} // namespace ige::scene