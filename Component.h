#pragma once

#include <memory>
#include <vector>
#include <string>

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
        Component(std::shared_ptr<SceneObject> owner);
        
        //! Destructor
        virtual ~Component();

        //! Returns the name of the component
		virtual std::string getName() = 0;
        
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

        //! Destroyed
        virtual void onDestroy();

    protected:
        //! Reference to owner object
        std::weak_ptr<SceneObject> m_owner;
    };
}
