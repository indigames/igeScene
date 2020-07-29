#include "components/gui/UIImage.h"
#include "components/gui/RectTransform.h"
#include "scene/SceneObject.h"

#include "utils/GraphicsHelper.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace ige::scene
{
    //! Constructor
    UIImage::UIImage(const std::shared_ptr<SceneObject>& owner, const std::string& path, const Vec2& size)
        : Component(owner), m_figure(nullptr), m_size(size), m_path(path)
    {
        if (!m_path.empty())
        {
            m_figure = GraphicsHelper::getInstance()->createSprite(m_size, m_path.c_str());
            m_figure->WaitBuild();
            getOnFigureCreatedEvent().invoke(m_figure);
        }
    }

    //! Destructor
    UIImage::~UIImage() 
    {
        if(m_figure)
            m_figure->DecReference();
        m_figure = nullptr;
    }

    //! Update
    void UIImage::onUpdate(float dt)
    {
        if (m_figure == nullptr) return;

        // Update transform from transform component
        auto transCmp = std::dynamic_pointer_cast<RectTransform>(getOwner()->getTransform());
        if(m_size != transCmp->getSize())
        {
            setSize(transCmp->getSize());
            return;
        }
        
        m_figure->SetPosition(transCmp->getWorldPosition());
        m_figure->SetRotation(transCmp->getWorldRotation());
        m_figure->SetScale(transCmp->getWorldScale());

        // Update
        m_figure->Pose();
    }

    //! Update
    void UIImage::onRender()
    {
        if (m_figure == nullptr) return;
        m_figure->Render();
    }

    //! Set path
    void UIImage::setPath(const std::string& path)
    {
        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();

        if (strcmp(m_path.c_str(), relPath.c_str()) != 0)
        {
            m_path = relPath;

            if (m_figure != nullptr)
            {
                getOnFigureDestroyedEvent().invoke(m_figure);
                m_figure->DecReference();
                m_figure = nullptr;
            }

            if (m_path != "")
            {
                m_figure = GraphicsHelper::getInstance()->createSprite(m_size, m_path.c_str());
                m_figure->WaitBuild();
                getOnFigureCreatedEvent().invoke(m_figure);
            }
        }
    }

    //! Set size
    void UIImage::setSize(const Vec2& size)
    {
        if (m_size != size)
        {
            m_size = size;

            if (m_figure != nullptr)
            {
                getOnFigureDestroyedEvent().invoke(m_figure);
                m_figure->DecReference();
                m_figure = nullptr;
            }
            
            if (m_path != "")
            {
                m_figure = GraphicsHelper::getInstance()->createSprite(m_size, m_path.c_str());
                m_figure->WaitBuild();
                getOnFigureCreatedEvent().invoke(m_figure);
            }
        }
    }

    //! Serialize
    void UIImage::to_json(json& j) const
    {
        j = json {
            {"path", m_path},
            {"size", m_size}
        };
    }

    //! Deserialize
    void UIImage::from_json(const json& j)
    {
        j.at("size").get_to(m_size);
        setPath(j.at("path"));
    }
}
