#pragma once

#include <memory>
#include <vector>
#include <string>

#include <utils/PyxieHeaders.h>
#include "components/Component.h"
#include "components/CameraComponent.h"
#include "event/Event.h"

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

        //! Clear
        void clear();

        //! Get name
        const std::string& getName() const { return m_name; }

        //! Get name
        void setName(const std::string& name) { m_name = name; }

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

        //! Create GUI object
        virtual std::shared_ptr<SceneObject> createGUIObject(std::string name = "", std::shared_ptr<SceneObject> parent = nullptr, const Vec3& pos = Vec3(), const Vec2& size = {64, 64});

        //! Remove scene object
        virtual bool removeObject(const std::shared_ptr<SceneObject>& obj);

        //! Remove scene object by its name
        virtual bool removeObjectById(uint64_t id);

        //! Find object in hierarchy by it's id
        virtual std::shared_ptr<SceneObject> findObjectById(uint64_t id);

        //! Find object in hierarchy by name
        virtual std::shared_ptr<SceneObject> findObjectByName(std::string name);

        //! Serialize
        virtual void to_json(json& j) const;

        //! Deserialize
        virtual void from_json(const json& j);

        //! Get roots of scene nodes tree
        std::vector<std::shared_ptr<SceneObject>>& getObjects() { return m_objects; };

        //! Set active camera
        void setActiveCamera(CameraComponent* camera);

        //! Get active camera
        CameraComponent* getActiveCamera() { return m_activeCamera; }

        //! Return the cached path
        const std::string& getPath() const { return m_path; }

        //! Cache the path
        void setPath(const std::string& path) { m_path = path; }

        //! Get showcase
        Showcase* getShowcase() { return m_showcase; }

        //! Internal event
        Event<Resource*>& getResourceAddedEvent() { return m_resourceAddedEvent; }
        Event<Resource*>& getResourceRemovedEvent() { return m_resourceRemovedEvent; }

        //! Resource added/removed event
        void onResourceAdded(Resource* resource);
        void onResourceRemoved(Resource* resource);

    protected:
        //! Scene root node
        std::vector<std::shared_ptr<SceneObject>> m_objects;

        //! Showcase which contains all rendering resources
        Showcase* m_showcase = nullptr;

        //! Internal events
        Event<Resource*> m_resourceAddedEvent;
        Event<Resource*> m_resourceRemovedEvent;

        //! Cache active camera
        CameraComponent* m_activeCamera = nullptr;

        //! Object ID counter
        uint64_t m_nextObjectID = 0;

        //! Scene name
        std::string m_name;

        //! Cached path
        std::string m_path;
    };
}
