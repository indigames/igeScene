#pragma once

#include <memory>
#include <vector>
#include <string>

namespace ige::scene
{
    class SceneObject;

    /**
     * Class SceneManager: Manage scene object hierarchy 
     */
    class SceneManager
    {
    public:
        //! Constructor
        SceneManager();

        //! Destructor
        virtual ~SceneManager();     

        //! Update
        virtual void update(float dt);

        //! Update with fixed dt
        virtual void fixedUpdate(float dt);

        //! Late update
        virtual void lateUpdate(float dt);

        //! Create scene object
        virtual std::shared_ptr<SceneObject> createObject(std::string name = "", std::shared_ptr<SceneObject> parent = nullptr);

        //! Remove scene object
        virtual bool removeObject(std::shared_ptr<SceneObject> obj);
        
        //! Find object in hierarchy by it's id
        virtual std::shared_ptr<SceneObject> findObjectById(uint64_t id)  const;

        //! Find object in hierarchy by name
        virtual std::shared_ptr<SceneObject> findObjectByName(std::string name) const;

        //! Create empty scene
        bool createEmptyScene();

        //! Load scene from IO
        bool loadScene(const std::string& scenePath);

        //! Save scene
        bool saveScene(const std::string& scenePath);

    protected:
        //! Scene root node
        std::shared_ptr<SceneObject> m_root;

        //! Object ID counter
        uint64_t m_nextObjectID = 1;
    };
}
