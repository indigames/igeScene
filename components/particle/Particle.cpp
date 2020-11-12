
#include "components/particle/Particle.h"
#include "systems/particle/ParticleManager.h"
#include "scene/SceneObject.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::scene
{
    //! Static members initialization
    Event<Particle &> Particle::m_onCreatedEvent;
    Event<Particle &> Particle::m_onDestroyedEvent;

    //! Constructor
    Particle::Particle(SceneObject &owner, const std::string &path)
        : Component(owner)
    {
        if (!path.empty())
        {
            setPath(path);
        }
        m_onCreatedEvent.invoke(*this);
    }

    //! Destructor
    Particle::~Particle()
    {
        m_onDestroyedEvent.invoke(*this);
        if(m_effect)
        {
            m_effect->UnloadResources();
            m_effect->Release();
        }
        m_effect = nullptr;
    }

    //! Set enabled
    void Particle::setEnabled(bool enable)
    {
        if (m_bIsEnabled != enable)
        {
            m_bIsEnabled = enable;

            if (!m_bIsEnabled)
            {
                stop();
            }
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
            m_effect = Effekseer::Effect::Create(ParticleManager::getInstance()->getManager(), (const EFK_CHAR*)m_path.c_str());            
        }
    }

    //! Layer
    void Particle::setLayer(int layer)
    {
        m_layer = layer;
        if(m_handle != -1)
            ParticleManager::getInstance()->getManager()->SetLayer(m_handle, m_layer);
    }

    //! Group mask
    void Particle::setGroupMask(int mask)
    {
        m_groupMask = mask;
        if(m_handle != -1)
            ParticleManager::getInstance()->getManager()->SetGroupMask(m_handle, m_groupMask);
    }

    //! Speed
    void Particle::setSpeed(float speed)
    {
        m_speed = speed;
        if(m_handle != -1)
            ParticleManager::getInstance()->getManager()->SetSpeed(m_handle, m_speed);
    }

    //! Time scale
    void Particle::setTimeScale(float timeScale)
    {
        if(m_handle != -1)
            ParticleManager::getInstance()->getManager()->SetTimeScaleByHandle(m_handle, timeScale);
    }

    //! Auto Drawing
    void Particle::setAutoDrawing(bool autoDraw)
    {
        if(m_handle != -1)
            ParticleManager::getInstance()->getManager()->SetAutoDrawing(m_handle, autoDraw);
    }

    //! Dynamic input parameter
    void Particle::setDynamicInputParameter(const Vec4& param)
    {
        m_dynamicInputParameter = param;

        if(m_handle != -1)
        {
            for(int i = 0; i < 4; ++i)
            {
                ParticleManager::getInstance()->getManager()->SetDynamicInput(m_handle, i, m_dynamicInputParameter[i]);
            }
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
    void Particle::setTargetLocation(const Vec3& location)
    {
    }

    //! Play
    void Particle::play()
    {
        if (m_effect != nullptr)
        {
            auto position = getOwner()->getTransform()->getWorldPosition();
            m_handle = ParticleManager::getInstance()->getManager()->Play(m_effect, { position[0], position[1], position[2] });

            // Apply configuration
            setLayer(m_layer);
            setGroupMask(m_groupMask);
            setSpeed(m_speed);
            setTimeScale(m_timeScale);
            setAutoDrawing(m_bIsAutoDrawing);
            setLoop(m_bIsLooped);
            setTargetLocation(m_targetLocation);
            setDynamicInputParameter(m_dynamicInputParameter);
        }        
    }

    //! Pause
    void Particle::pause()
    {
        if(m_handle != -1)
            ParticleManager::getInstance()->getManager()->SetPaused(m_handle, true);
    }

    //! Resume
    void Particle::resume()
    {
        if(m_handle != -1)
            ParticleManager::getInstance()->getManager()->SetPaused(m_handle, false);
    }

    //! Stop
    void Particle::stop()
    {
        if(m_handle != -1)
            ParticleManager::getInstance()->getManager()->StopEffect(m_handle);
        m_handle = -1;
    }

    //! Serialize
    void Particle::to_json(json &j) const
    {
        j = json{
            {"isEnabled", isEnabled()},
            {"path", getPath()},
            {"layer", getLayer()},
            {"speed", getSpeed()},
            {"timeScale", getTimeScale()},
            {"autoDraw", isAutoDrawing()},
            {"loop", isLooped()},
            {"target", getTargetLocation()},
            {"param", getDynamicInputParameter()},
        };
    }

    //! Deserialize
    void Particle::from_json(const json &j)
    {
        setEnabled(j.value("isEnabled", false));
        setPath(j.value("path", std::string()));
        setLayer(j.value("layer", 0));
        setSpeed(j.value("speed", 1.f));
        setTimeScale(j.value("timeScale", 1.f));
        setAutoDrawing(j.value("autoDraw", true));
        setLoop(j.value("loop", false));
        setTargetLocation(j.value("target", Vec3(0.f, 0.f, 0.f)));
        setDynamicInputParameter(j.value("param", Vec4(0.f, 0.f, 0.f, 0.f)));
    }
} // namespace ige::scene
