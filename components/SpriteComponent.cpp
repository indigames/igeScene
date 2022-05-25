#include "components/SpriteComponent.h"
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
    SpriteComponent::SpriteComponent(SceneObject &owner, const std::string &path, const Vec2 &size, bool isBillboard)
        : Component(owner), m_path(path)
    {
        if (path.empty()) {
            m_sprite = std::make_shared<Sprite>(size);
        }
        else {
            auto texture = ResourceCreator::Instance().NewTexture(m_path.c_str());
            m_sprite = std::make_shared<Sprite>(texture, size);
        }

        m_sprite->setIsScaleBorder(true);
        setBillboard(isBillboard);
        onResourceAdded(m_sprite->getFigure());
    }

    //! Destructor
    SpriteComponent::~SpriteComponent()
    {
        if (m_sprite && m_sprite->getFigure())
            onResourceRemoved(m_sprite->getFigure());

        m_sprite = nullptr;

        if(getOwner()->getTransform())
            getOwner()->getTransform()->makeDirty();
    }

    //! Enable
    void SpriteComponent::onEnable()
    {
        if (!getOwner()->isActive() || !isEnabled()) return;
        Component::onEnable();
        if (m_sprite) onResourceAdded(m_sprite->getFigure());
    }

    //! Disable
    void SpriteComponent::onDisable()
    {
        if(m_sprite) onResourceRemoved(m_sprite->getFigure());
        Component::onDisable();
    }

    //! Update
    void SpriteComponent::onUpdate(float dt)
    {
        if (getFigure() == nullptr)
            return;

        // Update transform from transform component
        auto transform = getOwner()->getTransform();
        getFigure()->SetPosition(transform->getPosition());
        getFigure()->SetRotation(transform->getRotation());
        getFigure()->SetScale(transform->getScale());

        // Update
        getFigure()->Pose();
    }

    //! Render
    void SpriteComponent::onRender()
    {
    }

    //! Set path
    void SpriteComponent::setTexture(Texture *texture)
    {
        if (texture == getTexture())
            return;

        m_path.clear();
        auto oldFigure = m_sprite->getFigure();

        m_sprite->setTexture(texture);
        auto newFigure = m_sprite->getFigure();

        if (texture != nullptr)
            m_path = texture->ResourceName();

        if (oldFigure == nullptr && newFigure) {
            onResourceAdded(newFigure);
        }

        if (oldFigure && newFigure == nullptr) {
            onResourceRemoved(oldFigure);
        }

        if (newFigure) {
            bool isBillboard = m_bIsBillboard;
            m_bIsBillboard = false;
            setBillboard(isBillboard);
        }

        if (getOwner()->getRectTransform()) {
            getOwner()->getRectTransform()->setSize(m_sprite->getSize());
        }
        else {
            getOwner()->getTransform()->makeDirty();
        }
    }

    //! Set path
    void SpriteComponent::setPath(const std::string &path)
    {
        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
        if (relPath.size() == 0) relPath = fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');

        m_path = relPath;

        fsPath = fs::path(m_path);
        auto fPath = fsPath.extension().compare(".pyxi") == 0 ? m_path : fsPath.parent_path().append(fsPath.stem().string() + ".pyxi").string();
        if (fPath.size() == 0) fPath = fsPath.string();
        std::replace(fPath.begin(), fPath.end(), '\\', '/');
            
        auto texture = ResourceCreator::Instance().NewTexture(fPath.c_str());

        auto oldFigure = m_sprite->getFigure();
        m_sprite->setTexture(texture);
        auto newFigure = m_sprite->getFigure();

        if (oldFigure == nullptr && newFigure) {
            onResourceAdded(newFigure);
        }

        if (oldFigure != nullptr && newFigure == nullptr) {
            onResourceRemoved(oldFigure);
        }

        if (newFigure) {
            bool isBillboard = m_bIsBillboard;
            m_bIsBillboard = false;
            setBillboard(isBillboard);
        }
    }

    void SpriteComponent::onResourceAdded(Resource* res) {
        if (m_bResAdded || res == nullptr) return;
        if (getOwner() && getOwner()->isActive(true) && getOwner()->getScene()) {
            if (getOwner()->isGUIObject())
                getOwner()->getScene()->getUIResourceAddedEvent().invoke(res);
            else
                getOwner()->getScene()->getResourceAddedEvent().invoke(res);
            m_bResAdded = true;
        }       
    }

    void SpriteComponent::onResourceRemoved(Resource* res) {
        if (!m_bResAdded || res == nullptr) return;
        if (getOwner() && getOwner()->getScene()) {
            if (getOwner()->isGUIObject())
                getOwner()->getScene()->getUIResourceRemovedEvent().invoke(res);
            else
                getOwner()->getScene()->getResourceRemovedEvent().invoke(res);
            m_bResAdded = false;
        }
    }

    //! Set size
    void SpriteComponent::setSize(const Vec2 &size)
    {
        m_sprite->setSize(size);
        getOwner()->getTransform()->makeDirty();
    }

    //! Set billboard
    void SpriteComponent::setBillboard(bool isBillboard)
    {
        m_bIsBillboard = isBillboard;
        auto figure = m_sprite->getFigure();
        if (figure)
        {
            auto shaderDesc = pyxieResourceCreator::Instance().NewShaderDescriptor();
            shaderDesc->SetValue(figure->GetShaderName(0));
            shaderDesc->SetBillboard(m_bIsBillboard);
            figure->SetShaderName(0, shaderDesc->GetValue());
        }
    }

    //! Set Tiling
    void SpriteComponent::setTiling(const Vec2& value)
    {
        m_sprite->setTiling(value);
    }

    //! Set Offset
    void SpriteComponent::setOffset(const Vec2& value)
    {
        m_sprite->setOffset(value);
    }

    //! Set Wrap mode
    void SpriteComponent::setWrapMode(int value)
    {
        m_sprite->setWrapMode((SamplerState::WrapMode)value);
    }

    //! Set Alpha
    void SpriteComponent::setAlpha(float value)
    {
        auto color = m_sprite->getColor();
        color[3] = value;
        m_sprite->setColor(color);
    }

    const float SpriteComponent::getAlpha() const 
    {
        return m_sprite->getColor()[3];
    }

    //! Set Color
    void SpriteComponent::setColor(const Vec4& value)
    {
        m_sprite->setColor(value);
    }

    void SpriteComponent::setColor(float r, float g, float b, float a) {
        m_sprite->setColor(r, g, b, a);
    }

    //! SpriteType
    void SpriteComponent::setSpriteType(int value) {
        m_sprite->setSpriteType((SpriteType)value);
    }

    void SpriteComponent::setBorderLeft(float value) {
        m_sprite->setBorderLeft(value);
    }

    void SpriteComponent::setBorderRight(float value) {
        m_sprite->setBorderRight(value);
    }

    void SpriteComponent::setBorderTop(float value) {
        m_sprite->setBorderTop(value);
    }

    void SpriteComponent::setBorderBottom(float value) {
        m_sprite->setBorderBottom(value);
    }

    void SpriteComponent::setBorder(const Vec4& value) {
        m_sprite->setBorder(value[0], value[1], value[2], value[3]);
    }

    //! Set Fill Method
    void SpriteComponent::setFillMethod(int value)
    {
        m_sprite->setFillMethod((FillMethod)value);
    }

    //! Set Fill Origin
    void SpriteComponent::setFillOrigin(int value)
    {
        m_sprite->setFillOrigin((FillOrigin)value);
    }

    //! Set Fill Amount
    void SpriteComponent::setFillAmount(float value)
    {
        m_sprite->setFillAmount(value);
    }

    //! Set Clockwise
    void SpriteComponent::setClockwise(bool value)
    {
        m_sprite->setClockwise(value);
    }

    //! Serialize
    void SpriteComponent::to_json(json &j) const
    {
        Component::to_json(j);
        j["path"] = getPath();
        j["size"] = getSize();
        j["billboard"] = isBillboard();
        j["tiling"] = getTiling();
        j["offset"] = getOffset();
        j["wrapmode"] = (int)getWrapMode();
        j["color"] = getColor();
        j["spritetype"] = (int)getSpriteType();
        j["border"] = getBorder();
        j["fillmethod"] = getFillMethod();
        j["fillorigin"] = getFillOrigin();
        j["fillamount"] = getFillAmount();
        j["clockwise"] = getClockwise();
    }

    //! Deserialize
    void SpriteComponent::from_json(const json &j)
    {
        setPath(j.value("path", ""));
        setSize(j.value("size", Vec2(64, 64)));
        setBillboard(j.value("billboard", false));
        setTiling(j.value("tiling", Vec2(1, 1)));
        setOffset(j.value("offset", Vec2(0, 0)));
        setWrapMode(j.value("wrapmode", (int)SamplerState::CLAMP));
        setColor(j.value("color", Vec4(1.f, 1.f, 1.f, 1.f)));
        setSpriteType(j.value("spritetype", (int)SpriteType::Simple));
        setBorder(j.value("border", Vec4(0, 0, 0, 0)));
        setFillMethod(j.value("fillmethod", (int)FillMethod::None));
        setFillOrigin(j.value("fillorigin", (int)FillOrigin::Bottom));
        setFillAmount(j.value("fillamount", 1.0f));
        setClockwise(j.value("clockwise", false));
        Component::from_json(j);
    }

    //! Update property by key value
    void SpriteComponent::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("size") == 0)
        {
            setSize(val);
        }
        else if (key.compare("billboard") == 0)
        {
            setBillboard(val);
        }
        else if (key.compare("tiling") == 0)
        {
            setTiling(val);
        }
        else if (key.compare("offset") == 0)
        {
            setOffset(val);
        }
        else if (key.compare("wrapmode") == 0)
        {
            setWrapMode(val);
        }
        else if (key.compare("color") == 0)
        {
            setColor(val);
        }
        else if (key.compare("spritetype") == 0)
        {
            setSpriteType(val);
        }
        else if (key.compare("border") == 0)
        {
            setBorder(val);
        }
        else if (key.compare("fillmethod") == 0)
        {
            setFillMethod(val);
        }
        else if (key.compare("fillorigin") == 0)
        {
            setFillOrigin(val);
        }
        else if (key.compare("fillamount") == 0)
        {
            setFillAmount(val);
        }
        else if (key.compare("clockwise") == 0)
        {
            setClockwise(val);
        }
        else if (key.compare("path") == 0)
        {
            setPath(val);
        }
        else
        {
            Component::setProperty(key, val);
        }
    }
} // namespace ige::scene
