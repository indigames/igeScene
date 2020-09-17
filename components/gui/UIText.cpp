#include "components/gui/UIText.h"
#include "components/gui/RectTransform.h"

#include "scene/SceneObject.h"

namespace ige::scene
{
    //! Constructor
    UIText::UIText(const std::shared_ptr<SceneObject>& owner, const std::string& text, const std::string& fontPath, int fontSize, const Vec4& color)
        : Component(owner)
    {
        m_text = std::make_shared<Text>(text, fontPath, fontSize, color);
        if (m_text->getFigure())
            getOwner()->getRoot()->getResourceAddedEvent().invoke(m_text->getFigure());
    }

    //! Destructor
    UIText::~UIText()
    {
        if (m_text->getFigure())
            if(hasOwner()) getOwner()->getRoot()->getResourceRemovedEvent().invoke(m_text->getFigure());
        m_text = nullptr;
    }

    //! Update
    void UIText::onUpdate(float dt)
    {
        if (getFigure() == nullptr) return;

        // Update transform from transform component
        auto transform = std::dynamic_pointer_cast<RectTransform>(getOwner()->getTransform());

        // Scale container to fit text size
        auto containerSize = transform->getSize();
        auto size = m_text->getSize();
        auto sizeChanged = false;        
        if (size.X() > containerSize.X()) 
        {
            containerSize.X(size.X() + 1.f);
            sizeChanged = true;
        }
        if (size.Y() > containerSize.Y())
        {
            containerSize.Y(size.Y() + 1.f);
            sizeChanged = true;
        }
        if (sizeChanged) transform->setSize(containerSize);

        getFigure()->SetPosition(transform->getWorldPosition());
        getFigure()->SetRotation(transform->getWorldRotation());
        getFigure()->SetScale(transform->getWorldScale());

        // Update
        getFigure()->Pose();
    }

    //! Render
    void UIText::onRender()
    {
        if (getFigure() == nullptr) return;
        getFigure()->Render();
    }

    //! Serialize
    void UIText::to_json(json& j) const
    {
        j = json {
            {"text", getText()},
            {"font", getFontPath()},
            {"size", getFontSize()},
            {"color", getColor()}
        };
    }

    //! Deserialize
    void UIText::from_json(const json& j)
    {
        setText(j.at("text"));
        setFontPath(j.at("font"));
        setFontSize(j.at("size"));
        setColor(j.at("color"));
    }

    //! Text
    void UIText::setText(const std::string& text)
    {
        auto oldFigure = m_text->getFigure();
        m_text->setText(text);
        auto newFigure = m_text->getFigure();
        if(oldFigure != newFigure)
        {
            if (oldFigure) getOwner()->getRoot()->getResourceRemovedEvent().invoke(oldFigure);
            if (newFigure) getOwner()->getRoot()->getResourceAddedEvent().invoke(newFigure);
        }
    }

    //! Font Path
    void UIText::setFontPath(const std::string& path)
    {
        auto oldFigure = m_text->getFigure();
        m_text->setFontPath(path);
        auto newFigure = m_text->getFigure();
        if(oldFigure != newFigure)
        {
            if (oldFigure) getOwner()->getRoot()->getResourceRemovedEvent().invoke(oldFigure);
            if (newFigure) getOwner()->getRoot()->getResourceAddedEvent().invoke(newFigure);
        }
    }

    //! Font Size
    void UIText::setFontSize(int size)
    {
        auto oldFigure = m_text->getFigure();
        m_text->setFontSize(size);
        auto newFigure = m_text->getFigure();
        if(oldFigure != newFigure)
        {
            if (oldFigure) getOwner()->getRoot()->getResourceRemovedEvent().invoke(oldFigure);
            if (newFigure) getOwner()->getRoot()->getResourceAddedEvent().invoke(newFigure);
        }
    }

    //! Color
    void UIText::setColor( const Vec4& color)
    {
        m_text->setColor(color);
    }
}
