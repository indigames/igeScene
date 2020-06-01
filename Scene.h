#pragma once

#include <memory>
#include <vector>
#include <string>

namespace ige::scene
{
    class SceneObject;

    class Scene
    {
    public:
        Scene(std::string name = "");
        virtual ~Scene();

        virtual void update(float dt);

        virtual void fixedUpdate(float dt);

        virtual void lateUpdate(float dt);

        std::vector<std::shared_ptr<SceneObject>> getObjects() const;

        virtual std::shared_ptr<SceneObject> findObjectById(int id)  const;

        virtual std::shared_ptr<SceneObject> findObjectByName(std::string name) const;

    protected:
        std::string m_name;
        std::vector<std::shared_ptr<SceneObject>> m_objects;
    };
}
