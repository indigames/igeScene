
#include "components/particle/Particle.h"
#include "systems/particle/ParticleManager.h"
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
        if (!path.empty())
        {
            setPath(path);
        }
        m_onCreatedEvent.invoke(this);
    }

    //! Destructor
    Particle::~Particle()
    {
        m_onDestroyedEvent.invoke(this);
        if (m_effect)
        {
            stop();
            m_effect->Release();
            m_effect = nullptr;
        }
    }

    //! Set enabled
    void Particle::setEnabled(bool enable)
    {
        m_bIsEnabled = enable;

        if (m_bIsEnabled)
        {
            play();
        }
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

            if (m_effect)
            {
                stop();
                m_effect->Release();
                m_effect = nullptr;
            }

            EFK_CHAR path[512] = { 0 };
            Effekseer::ConvertUtf8ToUtf16(path, 512, m_path.c_str());

            // Create new effect
            m_effect = Effekseer::Effect::Create(ParticleManager::getInstance()->getManager(), (const EFK_CHAR*)path);

            if(isEnabled())
                play();
        }
    }

    //! Layer
    void Particle::setLayer(int layer)
    {
        m_layer = layer;
        if (m_handle != -1)
            ParticleManager::getInstance()->getManager()->SetLayer(m_handle, m_layer);
    }

    //! Group mask
    void Particle::setGroupMask(int mask)
    {
        m_groupMask = mask;
        if (m_handle != -1)
            ParticleManager::getInstance()->getManager()->SetGroupMask(m_handle, m_groupMask);
    }

    //! Speed
    void Particle::setSpeed(float speed)
    {
        m_speed = speed;
        if (m_handle != -1)
            ParticleManager::getInstance()->getManager()->SetSpeed(m_handle, m_speed);
    }

    //! Time scale
    void Particle::setTimeScale(float timeScale)
    {
        if (m_handle != -1)
            ParticleManager::getInstance()->getManager()->SetTimeScaleByHandle(m_handle, timeScale);
    }

    //! Auto Drawing
    void Particle::setAutoDrawing(bool autoDraw)
    {
        if (m_handle != -1)
            ParticleManager::getInstance()->getManager()->SetAutoDrawing(m_handle, autoDraw);
    }

    //! Dynamic input parameter
    void Particle::setDynamicInputParameter(const Vec4 &param)
    {
        m_dynamicInputParameter = param;

        if (m_handle != -1)
        {
            for (int i = 0; i < 4; ++i)
            {
                ParticleManager::getInstance()->getManager()->SetDynamicInput(m_handle, i, m_dynamicInputParameter[i]);
            }
        }
    }

    //! Color
    void Particle::setColor(const Vec4 &color)
    {
        m_color = color;

        if (m_handle != -1)
        {
            ParticleManager::getInstance()->getManager()->SetAllColor(m_handle, { (uint8_t)(m_color[0] * 255), (uint8_t)(m_color[1] * 255), (uint8_t)(m_color[2] * 255), (uint8_t)(m_color[3] * 255) });
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

        if (m_handle)
        {
            ParticleManager::getInstance()->getManager()->SetTargetLocation(m_handle, m_targetLocation[0], m_targetLocation[1], m_targetLocation[2]);
        }
    }

    //! Play
    void Particle::play()
    {
        if (m_effect)
        {
            // Stop playing effect
            stop();

            // Create handle
            auto position = getOwner()->getTransform()->getWorldPosition();
            m_handle = ParticleManager::getInstance()->getManager()->Play(m_effect, { position[0], position[1], position[2] });
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
        if (m_handle != -1)
            ParticleManager::getInstance()->getManager()->SetPaused(m_handle, true);
    }

    //! Resume
    void Particle::resume()
    {
        if (m_handle != -1)
            ParticleManager::getInstance()->getManager()->SetPaused(m_handle, false);
    }

    //! Stop
    void Particle::stop()
    {
        if (m_handle != -1)
            ParticleManager::getInstance()->getManager()->StopEffect(m_handle);
        m_handle = -1;
    }

    //! Update function
    void Particle::onUpdate(float dt)
    {
        // Apply transform
        if (m_handle)
        {
            auto transform = getOwner()->getTransform();
            auto position = transform->getWorldPosition();

            if(m_lastPosition != position)
            {
                ParticleManager::getInstance()->getManager()->SetLocation(m_handle, position[0], position[1], position[2]);
                m_lastPosition = position;
            }

            auto rotation = transform->getWorldRotation();
            if (m_lastRotation != rotation)
            {
                Vec3 euler;
                vmath_quatToEuler(rotation.P(), euler.P());
                ParticleManager::getInstance()->getManager()->SetRotation(m_handle, euler[0], euler[1], euler[2]);
                m_lastRotation = rotation;
            }
            
            auto scale = transform->getWorldScale();
            if (m_lastScale != scale)
            {
                ParticleManager::getInstance()->getManager()->SetScale(m_handle, scale[0], scale[1], scale[2]);
                m_lastScale = scale;
            }
        }
    }

    //! Serialize
    void Particle::to_json(json &j) const
    {
        j = json{
            {"enable", isEnabled()},
            {"path", getPath()},
            {"layer", getLayer()},
            {"speed", getSpeed()},
            {"timeScale", getTimeScale()},
            {"autoDraw", isAutoDrawing()},
            {"loop", isLooped()},
            {"target", getTargetLocation()},
            {"param", getDynamicInputParameter()},
            {"color", getColor()},
        };
    }

    //! Deserialize
    void Particle::from_json(const json &j)
    {
        setPath(j.value("path", std::string()));
        setLayer(j.value("layer", 0));
        setSpeed(j.value("speed", 1.f));
        setTimeScale(j.value("timeScale", 1.f));
        setAutoDrawing(j.value("autoDraw", true));
        setLoop(j.value("loop", false));
        setTargetLocation(j.value("target", Vec3(0.f, 0.f, 0.f)));
        setDynamicInputParameter(j.value("param", Vec4(0.f, 0.f, 0.f, 0.f)));
        setColor(j.value("color", Vec4(1.f, 1.f, 1.f, 1.f)));
        setEnabled(j.value("enable", true));
    }
} // namespace ige::scene
