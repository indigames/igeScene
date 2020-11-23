#pragma once

#include <memory>
#include <vector>
#include <string>
#include <optional>

#include <Effekseer.h>

#define __EFFEKSEER_RENDERER_INTERNAL_LOADER__
#include <EffekseerRendererGL/EffekseerRendererGL.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.TextureLoader.h>

#include "utils/Singleton.h"
#include "event/Event.h"

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
    class Particle;

    //! Class ParticleTextureLoader: load IGE texture to Effekseer
    class ParticleTextureLoader : public EffekseerRendererGL::TextureLoader
    {
    public:
        //! Constructor
        ParticleTextureLoader();

        //! Load
        Effekseer::TextureData* Load(const EFK_CHAR* path, Effekseer::TextureType textureType) override;
    };

    /**
     * Class ParticleManager: Manage particle effects
     */
    class ParticleManager : public Singleton<ParticleManager>
    {
    public:
        ParticleManager(int maxParticlesNumber = 2000, bool enableCulling = true, int threadsNumber = 2);
        virtual ~ParticleManager();

        //! Update
        void onUpdate(float dt);

        //! Render
        void onRender();

        //! Get manager
        Effekseer::Manager* getManager() { return m_manager; }

        //! Set projection matrix
        void setProjectionMatrix(const Mat4& matrix);

        //! Set camera matrix
        void setCameraMatrix(const Mat4& matrix);

    protected:
        //! Particle created/destroyed events
        void onCreated(Particle *particle);
        void onDestroyed(Particle *particle);

    protected:
        //! Enable culling
        bool m_bIsCullingEnabled = true;

        //! Particles
        std::vector<Particle*> m_particles;

        //! Manager instance
        Effekseer::Manager* m_manager = nullptr;

        //! Renderer instance
        EffekseerRenderer::Renderer* m_renderer = nullptr;

        //! Particle texture loader
        ParticleTextureLoader* m_textureLoader = nullptr;

        //! Projection matrix
        Mat4 m_projectionMatrix = {};

        //! Camera matrix
        Mat4 m_cameraMatrix = {};
    };
} // namespace ige::scene
