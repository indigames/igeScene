#pragma once

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "event/Event.h"
#include "components/gui/UIText.h"

namespace ige::scene
{
    //! UITextField
    class UITextField : public UIText
    {
    public:
        //! Constructor
        UITextField(SceneObject &owner, const std::string &text = "");

        //! Destructor
        virtual ~UITextField();

        //! Get component name
        virtual std::string getName() const override { return "UITextField"; }

        //! Returns the type of the component
        virtual Type getType() const override { return Type::UITextField; }

        //! Update
        virtual void onUpdate(float dt) override;

        //! Click
        virtual void onClick() override;

        //! Update property by key value
        virtual void setProperty(const std::string& key, const json& val) override;

    protected:
        //! Serialize
        virtual void to_json(json& j) const override;

        //! Deserialize
        virtual void from_json(const json& j) override;

        bool m_bIsInputing = false;
    };
} // namespace ige::scene
