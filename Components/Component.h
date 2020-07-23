#pragma once

#include <memory>
#include <vector>
#include <string>

#include "utils/Serialize.h"

namespace ige::scene
{
    class SceneObject;

    /**
	* Component is the base class of any component
	*/
    class Component
    {
    public:
        //! Constructor
        Component(const std::shared_ptr<SceneObject>& owner);
        
        //! Destructor
        virtual ~Component();

        //! Get owner object
        virtual std::shared_ptr<SceneObject> getOwner() const { return m_owner.lock(); }

        //! Check if owner was set
        virtual bool hasOwner() const { return getOwner() != nullptr; }

        //! Returns the name of the component
		virtual std::string getName() const = 0;
        
        //! Awake
        virtual void onAwake();

        //! Start
        virtual void onStart();

        //! Enable
        virtual void onEnable();

        //! Disable
        virtual void onDisable();

        //! Update functions
        virtual void onUpdate(float dt);
        virtual void onFixedUpdate(float dt);
        virtual void onLateUpdate(float dt);

        //! Render
        virtual void onRender();

        //! Destroyed
        virtual void onDestroy();

        //! Serialize
        virtual void to_json(json& j) const = 0;

        //! Deserialize 
        virtual void from_json(const json& j) = 0;
        
        //! Serialize
        friend void to_json(json& j, const Component& obj);

        //! Deserialize 
        friend void from_json(const json& j, Component& obj);

    protected:
        //! Reference to owner object
        std::weak_ptr<SceneObject> m_owner;
    };

    //! Serialize
    void to_json(json& j, const Component& obj);

    //! Deserialize 
    void from_json(const json& j, Component& obj);
}
