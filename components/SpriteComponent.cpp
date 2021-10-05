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
    SpriteComponent::SpriteComponent(SceneObject &owner, const std::string &path, const Vec2 &size, bool isBillboard, bool isGUI)
        : Component(owner), m_bIsGUI(isGUI), m_path(path), m_texture(nullptr)
    {
        if(path.empty()) 
            m_sprite = std::make_shared<Sprite>(size);
        else {
            m_texture = ResourceCreator::Instance().NewTexture(m_path.c_str());
            m_sprite = std::make_shared<Sprite>(m_texture, size);
        }

        if (m_sprite->getFigure() && getOwner()->getScene())
        {
            if(m_bIsGUI)
                getOwner()->getScene()->getUIResourceAddedEvent().invoke(m_sprite->getFigure());
            else 
                getOwner()->getScene()->getResourceAddedEvent().invoke(m_sprite->getFigure());
        }

        m_sprite->setIsScaleBorder(true);

        setBillboard(isBillboard);
    }

    //! Destructor
    SpriteComponent::~SpriteComponent()
    {
        if (m_sprite && m_sprite->getFigure())
            if (getOwner()->getScene()) {
                if (m_bIsGUI)
                    getOwner()->getScene()->getUIResourceRemovedEvent().invoke(m_sprite->getFigure());
                else 
                    getOwner()->getScene()->getResourceRemovedEvent().invoke(m_sprite->getFigure());
            }

        if (m_texture) {
            m_texture->DecReference();
            m_texture = nullptr;
        }

        m_sprite = nullptr;
        if(getOwner()->getTransform())
            getOwner()->getTransform()->makeDirty();
    }

    //! Enable
    void SpriteComponent::onEnable()
    {
        Component::onEnable();
        if (m_sprite && m_sprite->getFigure()) {
            if (getOwner()->getScene()) {
                if (m_bIsGUI)
                    getOwner()->getScene()->getUIResourceAddedEvent().invoke(m_sprite->getFigure());
                else
                    getOwner()->getScene()->getResourceAddedEvent().invoke(m_sprite->getFigure());
            }
        }
    }

    //! Disable
    void SpriteComponent::onDisable()
    {
        if (m_sprite && m_sprite->getFigure()) {
            if (getOwner()->getScene()) {
                if (m_bIsGUI)
                    getOwner()->getScene()->getUIResourceRemovedEvent().invoke(m_sprite->getFigure());
                else
                    getOwner()->getScene()->getResourceRemovedEvent().invoke(m_sprite->getFigure());
            }
        }
        Component::onDisable();
    }

    //! Update
    void SpriteComponent::onUpdate(float dt)
    {
        if (getFigure() == nullptr)
            return;

        // Update transform from transform component
        auto transform = getOwner()->getTransform();
        getFigure()->SetPosition(transform->getWorldPosition());
        getFigure()->SetRotation(transform->getWorldRotation());
        getFigure()->SetScale(transform->getWorldScale());

        // Update
        getFigure()->Pose();
    }

    //! Update
    void SpriteComponent::onRender()
    {
        /*if (getFigure() == nullptr)
            return;
        getFigure()->Render();*/
    }

    //! Set path
    void SpriteComponent::setPath(const std::string &path)
    {
        if (m_texture != nullptr) {
            m_texture->DecReference();
            m_texture = nullptr;
        }

        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
        if (relPath.size() == 0) relPath = fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');

        if (strcmp(m_path.c_str(), relPath.c_str()) != 0) {
            m_path = relPath;

            auto fsPath = fs::path(m_path);
            auto fPath = fsPath.extension().compare(".pyxi") == 0 ? m_path : fsPath.parent_path().append(fsPath.stem().string() + ".pyxi").string();
            if (fPath.size() == 0) fPath = fsPath.string();
            std::replace(fPath.begin(), fPath.end(), '\\', '/');
            
            m_texture = ResourceCreator::Instance().NewTexture(fPath.c_str());

            auto oldFigure = m_sprite->getFigure();
            m_sprite->setTexture(m_texture);
            auto newFigure = m_sprite->getFigure();

            if (oldFigure == nullptr && newFigure) {
                onCreateFigure(newFigure);
            }
            if (oldFigure != nullptr && newFigure == nullptr) {
                onRemoveFigure(oldFigure);
            }

            if (newFigure)
            {
                auto shaderDesc = pyxieResourceCreator::Instance().NewShaderDescriptor();
                shaderDesc->SetValue(newFigure->GetShaderName(0));
                shaderDesc->SetBillboard(m_bIsBillboard);
                newFigure->SetShaderName(0, shaderDesc->GetValue());
            }

            getOwner()->getTransform()->makeDirty();
        }
    }

    void SpriteComponent::onCreateFigure(EditableFigure* fig) {
        if (m_bIsGUI)
            getOwner()->getScene()->getUIResourceAddedEvent().invoke(fig);
        else
            getOwner()->getScene()->getResourceAddedEvent().invoke(fig);
    }

    void SpriteComponent::onRemoveFigure(EditableFigure* fig) {
        if (m_bIsGUI)
            getOwner()->getScene()->getUIResourceRemovedEvent().invoke(fig);
        else
            getOwner()->getScene()->getResourceRemovedEvent().invoke(fig);
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
        if (m_bIsBillboard != isBillboard)
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
        j["aBlend"] = isAlphaBlendingEnable();
        j["aBlendOp"] = getAlphaBlendingOp();
    }

    //! Deserialize
    void SpriteComponent::from_json(const json &j)
    {
        setSize(j.value("size", Vec2(0, 0)));
        setBillboard(j.value("billboard", false));
        setTiling(j.value("tiling", Vec2(0, 0)));
        setOffset(j.value("offset", Vec2(0, 0)));
        setWrapMode(j.value("wrapmode", 0));
        setColor(j.value("color", Vec4(0, 0, 0, 0)));
        setSpriteType(j.value("spritetype", 0));
        setBorder(j.value("border", Vec4(0, 0, 0, 0)));
        setPath(j.value("path", ""));
        setAlphaBlendingEnable(j.value("aBlend", false));
        setAlphaBlendingOp(j.value("aBlendOp", 0));
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
