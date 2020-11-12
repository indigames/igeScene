#include "systems/particle/ParticleManager.h"
#include "components/particle/Particle.h"

#include "utils/PyxieHeaders.h"

namespace ige::scene
{
    ParticleTextureLoader::ParticleTextureLoader()
        : EffekseerRendererGL::TextureLoader()
    {
    }

    Effekseer::TextureData* ParticleTextureLoader::Load(const EFK_CHAR* path, Effekseer::TextureType textureType)
    {
        auto data = EffekseerRendererGL::TextureLoader::Load(path, textureType);
        if(data == nullptr)
        {
            auto texture = (Texture*)ResourceManager::Instance().GetResource((const char*)path, TEXTURETYPE);
            if(texture != nullptr)
            {
                texture->WaitBuild();
                data = new Effekseer::TextureData();
                data->TextureFormat = Effekseer::TextureFormatType::ABGR8;
                data->Width = texture->GetTextureWidth();
                data->Height = texture->GetTextureHeight();
                data->UserID = texture->GetTextureHandle();
                data->HasMipmap = (texture->GetNumMips() > 0);
            }
        }
        return data;
    }

    ParticleManager::ParticleManager(int maxParticlesNumber, bool enableCulling, int threadsNumber)
        : m_bIsCullingEnabled(enableCulling)
    {
    #if defined(_MOBILE)
        m_renderer = EffekseerRendererGL::Renderer::Create(maxParticlesNumber, EffekseerRendererGL::OpenGLDeviceType::OpenGLES3);
    #else
        m_renderer = EffekseerRendererGL::Renderer::Create(maxParticlesNumber, EffekseerRendererGL::OpenGLDeviceType::OpenGL3);
    #endif

        // Create renderer instance
        m_renderer->SetTextureUVStyle(EffekseerRenderer::UVStyle::VerticalFlipped);

        // Create manager instance
        m_manager = Effekseer::Manager::Create(maxParticlesNumber);

        // Set renderers
        m_manager->SetRibbonRenderer(m_renderer->CreateRibbonRenderer());
        m_manager->SetRingRenderer(m_renderer->CreateRingRenderer());
        m_manager->SetTrackRenderer(m_renderer->CreateTrackRenderer());
        m_manager->SetModelRenderer(m_renderer->CreateModelRenderer());

        // Set loaders
        m_textureLoader = new ParticleTextureLoader();
        m_manager->SetTextureLoader(m_textureLoader);
        m_manager->SetModelLoader(m_renderer->CreateModelLoader());
        m_manager->SetMaterialLoader(m_renderer->CreateMaterialLoader());

        // Set culling properties
        if(enableCulling)
        {
            m_manager->CreateCullingWorld(200.0f, 200.0f, 200.0f, 4);
        }

        // Set worker threads
        if(threadsNumber > 1)
        {
            m_manager->LaunchWorkerThreads(threadsNumber);
        }

        // Listen to AudioSource events
        Particle::getCreatedEvent().addListener(std::bind(static_cast<void(ParticleManager::*)(Particle&)>(&ParticleManager::onCreated), this, std::placeholders::_1));
        Particle::getDestroyedEvent().addListener(std::bind(static_cast<void(ParticleManager::*)(Particle&)>(&ParticleManager::onDestroyed), this, std::placeholders::_1));
    }

    ParticleManager::~ParticleManager()
    {
        // Clear particles
        m_particles.clear();

        // Destroy manager
        m_manager->Destroy();
        m_manager = nullptr;

        // Destroy renderer
        m_renderer->Destroy();
        m_renderer = nullptr;

        // Delete texture loader
        m_textureLoader = nullptr;
    }

    //! Update
    void ParticleManager::onUpdate(float dt)
    {
        // Replay looped particles
        for (const auto& particleRef : m_particles)
        {
            auto particle = particleRef.get();
            if (particle.getHandle() != -1 && particle.isLooped() && !m_manager->Exists(particle.getHandle()))
            {
                particle.play();
            }
        }

        // Update manager
        m_manager->Update(dt);
    }

    //! Render
    void ParticleManager::onRender()
    {
        m_renderer->BeginRendering();
        {
            if (m_bIsCullingEnabled)
            {
                m_manager->CalcCulling(m_renderer->GetCameraProjectionMatrix(), true);
            }
            m_manager->Draw();
        }        
        m_renderer->EndRendering();
    }

    //! Set projection matrix
    void ParticleManager::setProjectionMatrix(const Mat4& matrix)
    {
        if(m_projectionMatrix != matrix)
        {
            m_projectionMatrix = matrix;
            if (m_renderer)
            {
                Effekseer::Matrix44 projectionMatrix;
                memcpy(projectionMatrix.Values, m_projectionMatrix.P(), sizeof(float) * 16);
                m_renderer->SetProjectionMatrix(projectionMatrix);
            }
        }
    }

    //! Set camera matrix
    void ParticleManager::setCameraMatrix(const Mat4& matrix)
    {
        if (m_cameraMatrix != matrix)
        {
            m_cameraMatrix = matrix;
            if (m_renderer)
            {
                Effekseer::Matrix44 projectionMatrix;
                memcpy(projectionMatrix.Values, m_cameraMatrix.P(), sizeof(float) * 16);
                m_renderer->SetCameraMatrix(projectionMatrix);
            }
        }
    }

    //! Particle created/destroyed events
    void ParticleManager::onCreated(Particle &particle)
    {
        m_particles.push_back(std::ref(particle));
    }

    void ParticleManager::onDestroyed(Particle &particle)
    {
        auto found = std::find_if(m_particles.begin(), m_particles.end(), [&particle](auto element) {
            return std::addressof(particle) == std::addressof(element.get());
        });

        if (found != m_particles.end())
            m_particles.erase(found);
    }
} // namespace ige::scene