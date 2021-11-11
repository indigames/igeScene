
#include "components/particle/Particle.h"
#include "components/particle/ParticleManager.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene
{
    //! Static members initialization
    Event<Particle *> Particle::m_onCreatedEvent;
    Event<Particle *> Particle::m_onDestroyedEvent;

    //! Constructor
    Particle::Particle(SceneObject &owner, const std::string &path)
        : Component(owner)
    {
        // Register manager
        if (!getOwner()->getScene()->isPrefab() && getOwner()->getRoot())
        {
            auto manager = getOwner()->getRoot()->getComponent<ParticleManager>();
            if (manager == nullptr)
                manager = getOwner()->getRoot()->addComponent<ParticleManager>();
            setManager(manager);
        }

        // Set path
        if (!path.empty())
        {
            setPath(path);
        }

        // Invoke created event
        m_onCreatedEvent.invoke(this);
    }

    //! Destructor
    Particle::~Particle()
    {
        m_effect.Reset();
        m_onDestroyedEvent.invoke(this);
        m_manager.reset();
    }

    //! Set enabled
    void Particle::setEnabled(bool enable)
    {
        Component::setEnabled(enable);

        if (isEnabled())
            play();
        else
            stop();
    }

    //! Set path
    void Particle::setPath(const std::string &path)
    {
        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');

        if (strcmp(m_path.c_str(), relPath.c_str()) != 0)
        {
            m_path = relPath;

            stop();

            if (getManager()) {
                EFK_CHAR path[512] = { 0 };
                Effekseer::ConvertUtf8ToUtf16(path, 512, m_path.c_str());

                // Create new effect
                m_effect.Reset();
                m_effect = Effekseer::Effect::Create(getManager()->getEffekseerManager(), (const EFK_CHAR*)path);

                if (isEnabled())
                    play();
            }
        }
    }

    //! Layer
    void Particle::setLayer(int layer)
    {
        m_layer = layer;
        if (m_handle != -1 && getManager())
            getManager()->getEffekseerManager()->SetLayer(m_handle, m_layer);
    }

    //! Group mask
    void Particle::setGroupMask(int mask)
    {
        m_groupMask = mask;
        if (m_handle != -1 && getManager())
            getManager()->getEffekseerManager()->SetGroupMask(m_handle, m_groupMask);
    }

    //! Speed
    void Particle::setSpeed(float speed)
    {
        m_speed = speed;
        if (m_handle != -1 && getManager())
            getManager()->getEffekseerManager()->SetSpeed(m_handle, m_speed);
    }

    //! Time scale
    void Particle::setTimeScale(float timeScale)
    {
        m_timeScale = timeScale;
        if (m_handle != -1 && getManager())
            getManager()->getEffekseerManager()->SetTimeScaleByHandle(m_handle, timeScale);
    }

    //! Auto Drawing
    void Particle::setAutoDrawing(bool autoDraw)
    {
        m_bIsAutoDrawing = autoDraw;
        if (m_handle != -1 && getManager())
            getManager()->getEffekseerManager()->SetAutoDrawing(m_handle, autoDraw);
    }

    //! Dynamic input parameter
    void Particle::setDynamicInputParameter(const Vec4 &param)
    {
        m_dynamicInputParameter = param;

        if (m_handle != -1 && getManager())
        {
            for (int i = 0; i < 4; ++i)
            {
                getManager()->getEffekseerManager()->SetDynamicInput(m_handle, i, m_dynamicInputParameter[i]);
            }
        }
    }

    //! Color
    void Particle::setColor(const Vec4 &color)
    {
        m_color = color;

        if (m_handle != -1 && getManager())
        {
            getManager()->getEffekseerManager()->SetAllColor(m_handle, { (uint8_t)(m_color[0] * 255), (uint8_t)(m_color[1] * 255), (uint8_t)(m_color[2] * 255), (uint8_t)(m_color[3] * 255) });
        }
    }

    //! Check loop
    bool Particle::isLooped() const
    {
        return m_bIsLooped;
    }

    //! Set loop
    void Particle::setLoop(bool loop)
    {
        if (m_bIsLooped != loop)
        {
            m_bIsLooped = loop;
        }
    }

    //! Target location
    void Particle::setTargetLocation(const Vec3 &location)
    {
        m_targetLocation = location;

        if (m_handle != -1 && getManager())
        {
            getManager()->getEffekseerManager()->SetTargetLocation(m_handle, m_targetLocation[0], m_targetLocation[1], m_targetLocation[2]);
        }
    }

    //! Play
    void Particle::play()
    {
        if (m_effect.Get() && getManager())
        {
            // Stop playing effect
            stop();

            // Create handle
            auto position = getOwner()->getTransform()->getPosition();
            m_handle = getManager()->getEffekseerManager()->Play(m_effect, { position[0], position[1], position[2] });
            m_effect->AddRef();

            // Apply configuration
            setLayer(m_layer);
            setGroupMask(m_groupMask);
            setSpeed(m_speed);
            setTimeScale(m_timeScale);
            setAutoDrawing(m_bIsAutoDrawing);
            setLoop(m_bIsLooped);
            setTargetLocation(m_targetLocation);
            setDynamicInputParameter(m_dynamicInputParameter);
            setColor(m_color);
        }
    }

    //! Pause
    void Particle::pause()
    {
        if (m_handle != -1 && getManager())
            getManager()->getEffekseerManager()->SetPaused(m_handle, true);
    }

    //! Resume
    void Particle::resume()
    {
        if (m_handle != -1 && getManager())
            getManager()->getEffekseerManager()->SetPaused(m_handle, false);
    }

    //! Stop
    void Particle::stop()
    {
        if (m_handle != -1 && getManager()) {
            getManager()->getEffekseerManager()->StopEffect(m_handle);
            m_handle = -1;
            if (m_effect.Get())
                m_effect->Release();
        }
    }

    //! isPaused
    bool Particle::isPaused()
    {
        return (m_handle != -1 && getManager()) && getManager()->getEffekseerManager()->GetPaused(m_handle);
    }

    //! isStopped
    bool Particle::isStopped()
    {
        return (m_handle == -1);
    }

    //! Update function
    void Particle::onUpdate(float dt)
    {
        // Apply transform
        if (m_handle != -1 && getManager())
        {
            auto transform = getOwner()->getTransform();
            auto position = transform->getPosition();

            if(m_lastPosition != position)
            {
                getManager()->getEffekseerManager()->SetLocation(m_handle, position[0], position[1], position[2]);
                m_lastPosition = position;
            }

            auto rotation = transform->getRotation();
            if (m_lastRotation != rotation)
            {
                Vec3 euler;
                vmath_quatToEuler(rotation.P(), euler.P());
                getManager()->getEffekseerManager()->SetRotation(m_handle, euler[0], euler[1], euler[2]);
                m_lastRotation = rotation;
            }

            auto scale = transform->getScale();
            if (m_lastScale != scale)
            {
                getManager()->getEffekseerManager()->SetScale(m_handle, scale[0], scale[1], scale[2]);
                m_lastScale = scale;
            }
        }
    }

    //! Serialize
    void Particle::to_json(json &j) const
    {
        Component::to_json(j);
        j["path"] = getPath();
        j["layer"] = getLayer();
        j["mask"] = getGroupMask();
        j["speed"] = getSpeed();
        j["timeScale"] = getTimeScale();
        j["autoDraw"] = isAutoDrawing();
        j["loop"] = isLooped();
        j["target"] = getTargetLocation();
        j["param"] = getDynamicInputParameter();
        j["color"] = getColor();
    }

    //! Deserialize
    void Particle::from_json(const json &j)
    {
        Component::from_json(j);
        setLayer(j.value("layer", 0));
        setGroupMask(j.value("mask", 0));
        setSpeed(j.value("speed", 1.f));
        setTimeScale(j.value("timeScale", 1.f));
        setAutoDrawing(j.value("autoDraw", true));
        setLoop(j.value("loop", false));
        setTargetLocation(j.value("target", Vec3(0.f, 0.f, 0.f)));
        setDynamicInputParameter(j.value("param", Vec4(0.f, 0.f, 0.f, 0.f)));
        setColor(j.value("color", Vec4(1.f, 1.f, 1.f, 1.f)));
        setPath(j.value("path", std::string()));
    }

    //! Update property by key value
    void Particle::setProperty(const std::string& key, const json& val)
    {
        if (key.compare("path") == 0)
        {
            setPath(val);
        }
        else if (key.compare("layer") == 0)
        {
            setLayer(val);
        }
        else if (key.compare("mask") == 0)
        {
            setGroupMask(val);
        }
        else if (key.compare("speed") == 0)
        {
            setSpeed(val);
        }
        else if (key.compare("timeScale") == 0)
        {
            setTimeScale(val);
        }
        else if (key.compare("autoDraw") == 0)
        {
            setAutoDrawing(val);
        }
        else if (key.compare("loop") == 0)
        {
            setLoop(val);
        }
        else if (key.compare("target") == 0)
        {
            setTargetLocation(val);
        }
        else if (key.compare("param") == 0)
        {
            setDynamicInputParameter(val);
        }
        else if (key.compare("color") == 0)
        {
            setColor(val);
        }
        else
        {
            Component::setProperty(key, val);
        }
    }
} // namespace ige::scene
