#include "components/particle/ParticleManager.h"
#include "components/particle/Particle.h"

#include "utils/PyxieHeaders.h"

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

#include <Effekseer.h>
#include "EffekseerRendererGL/EffekseerRendererGL.h"
#include "EffekseerRendererGL/EffekseerRenderer/GraphicsDevice.h"

namespace ige::scene
{
    ParticleTextureLoader::ParticleTextureLoader()
        : Effekseer::TextureLoader()
    {
    }

    Effekseer::TextureRef ParticleTextureLoader::Load(const char16_t* path, Effekseer::TextureType textureType)
    {
        auto data = Effekseer::TextureLoader::Load(path, textureType);
        if(data == nullptr)
        {
            char utf8Path[512];
            Effekseer::ConvertUtf16ToUtf8(utf8Path, 512, path);

            auto fsPath = fs::path(utf8Path);
            fsPath = fsPath.replace_extension(".pyxi");

            auto texture = (Texture*)ResourceCreator::Instance().NewTexture((const char*)fsPath.c_str());
            if(texture != nullptr)
            {
                texture->WaitInitialize();
                data = Effekseer::MakeRefPtr <Effekseer::Texture>();
                auto ret = ::Effekseer::MakeRefPtr <::EffekseerRendererGL::Backend::Texture>(nullptr);
                ret->Init(texture->GetTextureHandle(), texture->GetNumMips() > 0, [texture]() {
                    texture->DecReference();
                });
                data->SetBackend(ret);
            }
        }
        return data;
    }

    ParticleManager::ParticleManager(SceneObject& owner, int maxParticlesNumber, bool enableCulling, int threadsNumber)
        : Component(owner), m_maxParticleNumber(maxParticlesNumber), m_bIsCullingEnabled(enableCulling), m_numThreads(threadsNumber)
    {
    #if defined(_MOBILE)
        m_renderer = EffekseerRendererGL::Renderer::Create(maxParticlesNumber, EffekseerRendererGL::OpenGLDeviceType::OpenGLES3);
    #else
        m_renderer = EffekseerRendererGL::Renderer::Create(maxParticlesNumber, EffekseerRendererGL::OpenGLDeviceType::OpenGL3);
    #endif

        // Create renderer instance        
        m_renderer->SetTextureUVStyle(EffekseerRenderer::UVStyle::VerticalFlipped);

        // Create manager instance
        initializeManager();

        // Register event listeners
        Particle::getCreatedEvent().addListener(std::bind(static_cast<void(ParticleManager::*)(Particle*)>(&ParticleManager::onCreated), this, std::placeholders::_1));
        Particle::getDestroyedEvent().addListener(std::bind(static_cast<void(ParticleManager::*)(Particle*)>(&ParticleManager::onDestroyed), this, std::placeholders::_1));
    }

    ParticleManager::~ParticleManager()
    {
        // Unregister listener
        Particle::getCreatedEvent().removeAllListeners();
        Particle::getDestroyedEvent().removeAllListeners();

        // Clear particles
        m_particles.clear();

        // Destroy
        destroyManager();

        // Destroy renderer
        m_renderer = nullptr;
    }

    //! Initialize
    void ParticleManager::initializeManager()
    {
        destroyManager();

        m_manager = Effekseer::Manager::Create(m_maxParticleNumber);

        // Set renderers
        m_manager->SetSpriteRenderer(m_renderer->CreateSpriteRenderer());
        m_manager->SetRibbonRenderer(m_renderer->CreateRibbonRenderer());
        m_manager->SetRingRenderer(m_renderer->CreateRingRenderer());
        m_manager->SetTrackRenderer(m_renderer->CreateTrackRenderer());
        m_manager->SetModelRenderer(m_renderer->CreateModelRenderer());

        // Set loaders
        auto textureLoader = Effekseer::MakeRefPtr<ParticleTextureLoader>();
        m_manager->SetTextureLoader(textureLoader);
        m_manager->SetModelLoader(m_renderer->CreateModelLoader());
        m_manager->SetMaterialLoader(m_renderer->CreateMaterialLoader());

        // Set projection matrix to identity
        m_renderer->SetProjectionMatrix(Effekseer::Matrix44().Indentity());

        // Set culling properties
        if(m_bIsCullingEnabled && m_cullingLayerNumber > 0)
        {
            m_manager->CreateCullingWorld(m_cullingWorldSize.X(), m_cullingWorldSize.Y(), m_cullingWorldSize.Y(), m_cullingLayerNumber);
        }

        // Set worker threads
        if(m_numThreads > 1)
        {
            m_manager->LaunchWorkerThreads(m_numThreads);
        }

        // Replay particles
        for (auto& particle : m_particles)
        {
            if (particle->isEnabled())
            {
                particle->play();
            }
        }
    }

    //! Destroy
    void ParticleManager::destroyManager()
    {
        // Destroy manager
        if (m_manager.Get())
        {
            // Stop particles
            for (auto& particle : m_particles)
            {
                if (m_manager->Exists(particle->getHandle()))
                {
                    particle->stop();
                }
            }
            m_manager = nullptr;
        }
    }

    //! Update
    void ParticleManager::onUpdate(float dt)
    {
        // Replay looped particles
        for (auto& particle : m_particles)
        {
            if (particle->getHandle() != -1 && !m_manager->Exists(particle->getHandle()))
            {
                particle->getFrameEndedEvent().invoke();

                if (particle->isLooped())
                    particle->play();
                else
                    particle->stop();
            }
        }

        // Update manager
        m_manager->Update(Time::Instance().GetElapsedTime() * 60);
    }

    //! Render
    void ParticleManager::onRender()
    {
        // Setup render matrix
        m_renderer->ResetRenderState();
        setProjectionMatrix(RenderContext::InstancePtr()->GetRenderProjectionMatrix());
        setCameraMatrix(RenderContext::InstancePtr()->GetRenderViewMatrix());

        auto param = Effekseer::Manager::DrawParameter();
        param.CameraCullingMask = (int32_t)(1 << (int)Particle::Layer::_3D);

        m_renderer->BeginRendering();
        {
            if (m_bIsCullingEnabled) {
                m_manager->CalcCulling(m_renderer->GetCameraProjectionMatrix(), true);
            }
            m_manager->Draw(param);
        }
        m_renderer->EndRendering();
    }

    //! Render
    void ParticleManager::onRenderUI()
    {
        // Setup render matrix
        m_renderer->ResetRenderState();
        setUIProjectionMatrix(RenderContext::InstancePtr()->GetRenderProjectionMatrix());
        setUICameraMatrix(RenderContext::InstancePtr()->GetRenderViewMatrix());

        auto param = Effekseer::Manager::DrawParameter();
        param.CameraCullingMask = (int32_t)(1 << (int)Particle::Layer::_2D);

        m_renderer->BeginRendering();
        {
            m_manager->Draw(param);
        }
        m_renderer->EndRendering();
    }

    //! Set projection matrix
    void ParticleManager::setProjectionMatrix(const Mat4& matrix)
    {
        if(m_projectionMatrix != matrix) {
            m_projectionMatrix = matrix;
            if (m_renderer.Get()) {
                Effekseer::Matrix44 mat;
                memcpy(mat.Values, m_projectionMatrix.P(), sizeof(float) * 16);
                m_projectionMatrixEff = mat;
            }
        }
        m_renderer->SetProjectionMatrix(m_projectionMatrixEff);
    }

    //! Set projection matrix
    void ParticleManager::setUIProjectionMatrix(const Mat4& matrix)
    {
        if(m_UIProjectionMatrix != matrix) {
            m_UIProjectionMatrix = matrix;
            if (m_renderer.Get()) {
                Effekseer::Matrix44 mat;
                memcpy(mat.Values, m_UIProjectionMatrix.P(), sizeof(float) * 16);
                m_UIProjectionMatrixEff = mat;
            }
        }
        m_renderer->SetProjectionMatrix(m_UIProjectionMatrixEff);
    }

    //! Set camera matrix
    void ParticleManager::setCameraMatrix(const Mat4& matrix)
    {
        if (m_cameraMatrix != matrix) {
            m_cameraMatrix = matrix;
            if (m_renderer.Get()) {
                Effekseer::Matrix44 mat;
                memcpy(mat.Values, m_cameraMatrix.P(), sizeof(float) * 16);
                m_cameraMatrixEff = mat;
            }
        }
        m_renderer->SetCameraMatrix(m_cameraMatrixEff);
    }

    //! Set camera matrix
    void ParticleManager::setUICameraMatrix(const Mat4& matrix)
    {
        if (m_UICameraMatrix != matrix) {
            m_UICameraMatrix = matrix;
            if (m_renderer.Get()) {
                Effekseer::Matrix44 mat;
                memcpy(mat.Values, m_UICameraMatrix.P(), sizeof(float) * 16);
                mat.Values[3][2] = -1.f;
                m_UICameraMatrixEff = mat;
            }
        }
        m_renderer->SetCameraMatrix(m_UICameraMatrixEff);
    }


    //! Culling
    void ParticleManager::setCullingEnabled(bool culled)
    {
        if(m_bIsCullingEnabled != culled)
        {
            m_bIsCullingEnabled = culled;
            initializeManager();
        }
    }

    //! Culling world size
    void ParticleManager::setCullingWorldSize(const Vec3& worldSize)
    {
        if(m_cullingWorldSize != worldSize)
        {
            m_cullingWorldSize = worldSize;
            initializeManager();
        }
    }

    //! Culling layer number
    void ParticleManager::setCullingLayerNumber(int number)
    {
        if(m_cullingLayerNumber != number)
        {
            m_cullingLayerNumber = number;
            initializeManager();
        }
    }

    //! Max number of particles
    void ParticleManager::setMaxParticleNumber(int max)
    {
        if(m_maxParticleNumber != max)
        {
            m_maxParticleNumber = max;
            initializeManager();
        }
    }

    //! Threads number
    void ParticleManager::setNumberOfThreads(int number)
    {
        if(m_numThreads != number)
        {
            m_numThreads = number;
            initializeManager();
        }
    }

    //! Particle created/destroyed events
    void ParticleManager::onCreated(Particle *particle)
    {
        m_particles.push_back(particle);
    }

    void ParticleManager::onDestroyed(Particle *particle)
    {
        auto found = std::find_if(m_particles.begin(), m_particles.end(), [&particle](auto element) {
            return particle == element;
        });

        if (found != m_particles.end())
        {
            m_manager->StopEffect((*found)->getHandle());
            m_particles.erase(found);
        }
    }

    //! Serialize
    void ParticleManager::to_json(json &j) const
    {
        Component::to_json(j);
        j["culled"] = isCullingEnabled();
        j["cullSize"] = getCullingWorldSize();
        j["cullLayer"] = getCullingLayerNumber();
        j["maxParNum"] = getMaxParticleNumber();
        j["thrNum"] = getNumberOfThreads();
    }

    //! Deserialize
    void ParticleManager::from_json(const json &j)
    {
        setCullingEnabled(j.value("culled", true));
        setCullingWorldSize(j.value("cullSize", Vec3(1000.f, 1000.f, 1000.f)));
        setCullingLayerNumber(j.value("cullLayer", 4));
        setMaxParticleNumber(j.value("maxParNum", 2000));
        setNumberOfThreads(j.value("thrNum", 2));
        Component::from_json(j);
    }
} // namespace ige::scene