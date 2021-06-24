#include "components/CompoundComponent.h"
#include "scene/SceneObject.h"
#include "scene/Scene.h"

namespace ige::scene
{

    //! Constructor
    CompoundComponent::CompoundComponent(SceneObject &owner)
        : Component(owner) 
    {

    }

    //! Destructor
    CompoundComponent::~CompoundComponent()
    {

    }

    //! Returns the name of the component
    std::string CompoundComponent::getName() const
    {
        return "CompoundComponent";
    }

    //! Add/remove/clear components
    void CompoundComponent::add(const std::shared_ptr<Component>& comp)
    {
        if(comp != nullptr)
        {
            auto itr = std::find(m_components.begin(), m_components.end(), comp);
            if (itr == m_components.end())
            {
                m_components.push_back(comp);
            }
        }        
    }

    void CompoundComponent::remove(const std::shared_ptr<Component>& comp)
    {
        if (comp != nullptr)
        {
            auto itr = std::find(m_components.begin(), m_components.end(), comp);
            if (itr != m_components.end())
            {
                m_components.erase(itr);
            }
        }
    }

    void CompoundComponent::clear()
    {
        m_components.clear();
    }


    //! Enable/Disable
    bool CompoundComponent::isEnabled() const
    {
        if(m_components.empty())
            return false;
        return m_components[0]->isEnabled();
    }

    void CompoundComponent::setEnabled(bool enable)
    {
        for(auto& comp: m_components)
        {
            if(comp) comp->setEnabled(enable);
        }
    }

} // namespace ige::scene