#pragma once
#include <memory>

#include "scene/SceneObject.h"

namespace ige::scene
{
    class Scene;

    /**
    * TargetObject represents a single/group of selected object(s)
    */
    class TargetObject : public SceneObject
    {
    public:
        //! Constructor
        TargetObject(Scene* scene);

        //! Destructor
        virtual ~TargetObject();

        //! Get ID
        virtual uint64_t getId() const override;

        //! Get UUID
        virtual std::string getUUID() const override;

        //! Get Name
        virtual const std::string &getName() const override;

        // Get parent
        virtual SceneObject *getParent() const override;

        //! Add a component
        virtual void addComponent(const std::shared_ptr<Component> &component) override;

        //! Remove a component
        virtual bool removeComponent(const std::shared_ptr<Component> &component) override;

        //! Get components list
        virtual std::vector<std::shared_ptr<Component>> &getComponents() override;

        //! Check component by type
        template <typename T>
        inline bool hasComponent();

        //! Get component by type
        template <typename T>
        inline std::shared_ptr<T> getComponent();

        //! Add component by type
        template <typename T, typename... Args>
        std::shared_ptr<T> addComponent(Args &&... args);

        //! Enable or disable the actor
        void setActive(bool isActive) override;

        //! Check active
        bool isActive() const override;

        //! Serialize
        virtual void to_json(json &j) override {}

        //! Deserialize
        virtual void from_json(const json &j) override {}

        //! Add/remove object
        void add(SceneObject* object);
        void remove(SceneObject* object);

    protected:
        //! Utils to collect shared components
        void collectSharedComponents();

    protected:
        //! List of objects
        std::vector<SceneObject*> m_objects;

        //! List of components
        std::vector<std::shared_ptr<Component>> m_components;
    };

    //! Check component by type
    template <typename T>
    inline bool TargetObject::hasComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T should derive from Component");

        for (auto it = m_components.begin(); it != m_components.end(); ++it)
        {
            auto result = std::dynamic_pointer_cast<T>(*it);
            if (result) return true;
        }
        return false;
    }

    //! Get component by type
    template <typename T>
    inline std::shared_ptr<T> TargetObject::getComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T should derive from Component");

        for (auto it = m_components.begin(); it != m_components.end(); ++it)
        {
            auto result = std::dynamic_pointer_cast<T>(*it);
            if (result)
                return result;
        }
        return nullptr;
    }

    //! Add component by type
    template <typename T, typename... Args>
    inline std::shared_ptr<T> TargetObject::addComponent(Args &&... args)
    {
        static_assert(std::is_base_of<Component, T>::value, "T should derive from Component");
        auto instance = std::make_shared<T>(*this, args...);
        addComponent(instance);
        return instance;
    }
} // namespace ige::scene
