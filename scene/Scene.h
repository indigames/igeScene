#pragma once

#include <memory>
#include <vector>
#include <string>

#include <utils/PyxieHeaders.h>
#include "components/Component.h"

namespace ige::scene
{
    class SceneObject;

    /**
     * Class Scene: Manage scene objects hierarchy 
     */
    class Scene
    {
    public:
        //! Constructor
        Scene(const std::string& name);

        //! Destructor
        virtual ~Scene();     

        //! Initialize
        bool initialize();

        //! Get name
        const std::string& getName() const { return m_name; }

        //! Update
        virtual void update(float dt);

        //! Update with fixed dt
        virtual void fixedUpdate(float dt);

        //! Late update
        virtual void lateUpdate(float dt);

        //! Render
        virtual void render();

        //! Create scene object
        virtual std::shared_ptr<SceneObject> createObject(std::string name = "", std::shared_ptr<SceneObject> parent = nullptr);

        //! Remove scene object
        virtual bool removeObject(std::shared_ptr<SceneObject> obj);
        
        //! Find object in hierarchy by it's id
        virtual std::shared_ptr<SceneObject> findObjectById(uint64_t id)  const;

        //! Find object in hierarchy by name
        virtual std::shared_ptr<SceneObject> findObjectByName(std::string name) const;

        //! Load scene from IO
        bool loadScene(const std::string& scenePath);

        //! Save scene
        bool saveScene(const std::string& scenePath);

        //! Component added event
        void onComponentAdded(std::shared_ptr<Component> component);

        //! Component added event
        void onComponentRemoved(std::shared_ptr<Component> component);

    protected:
        //! Scene root node
        std::shared_ptr<SceneObject> m_root;

        //! Scene showcase
        Showcase* m_showcase;

        //! Object ID counter
        uint64_t m_nextObjectID = 1;

        //! Scene name
        std::string m_name;
    };
}
