#pragma once

#include <memory>
#include <vector>
#include <string>
#include <optional>

#include "event/Event.h"
#include "components/Component.h"

#include <Effekseer.h>
#define __EFFEKSEER_RENDERER_INTERNAL_LOADER__
#include <EffekseerRendererGL/EffekseerRendererGL.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.TextureLoader.h>

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
        Effekseer::TextureData *Load(const EFK_CHAR *path, Effekseer::TextureType textureType) override;
    };

    /**
     * Class ParticleManager: Manage particle effects. Should be added to the root node.
     */
    class ParticleManager : public Component
    {
    public:
        ParticleManager(SceneObject &owner, int maxParticlesNumber = 2000, bool enableCulling = true, int threadsNumber = 2);
        virtual ~ParticleManager();

        //! Get name
        std::string getName() const override { return "ParticleManager"; }

        //! Update
        void onUpdate(float dt);

        //! Render.
        void onRender();

        //! Get manager
        Effekseer::Manager *getEffekseerManager() { return m_manager; }

        //! Set projection matrix
        void setProjectionMatrix(const Mat4 &matrix);

        //! Set camera matrix
        void setCameraMatrix(const Mat4 &matrix);

        //! Culling
        bool isCullingEnabled() const {return m_bIsCullingEnabled; }
        void setCullingEnabled(bool culled);

        //! Culling world size
        const Vec3& getCullingWorldSize() const { return m_cullingWorldSize; }
        void setCullingWorldSize(const Vec3& worldSize);

        //! Culling layer number
        int getCullingLayerNumber() const { return m_cullingLayerNumber; }
        void setCullingLayerNumber(int max);

        //! Max number of particles
        int getMaxParticleNumber() const { return m_maxParticleNumber; }
        void setMaxParticleNumber(int max);

        //! Threads number
        int getNumberOfThreads() const { return m_numThreads; }
        void setNumberOfThreads(int num);

    protected:
        //! Initialize manager instance
        void initializeManager();

        //! Destroy manager instance
        void destroyManager();

        //! Particle created/destroyed events
        void onCreated(Particle *particle);
        void onDestroyed(Particle *particle);

        //! Serialize
        virtual void to_json(json &j) const override;

        //! Deserialize
        virtual void from_json(const json &j) override;

    protected:
        //! Enable culling
        bool m_bIsCullingEnabled = true;

        //! Culling world size
        Vec3 m_cullingWorldSize = {1000.f, 1000.f, 1000.f};

        //! Culling layer number. Larger number produces more efficient culling, but it also uses a lot more of memory.
        int m_cullingLayerNumber = 4;

        //! Max particle number
        int m_maxParticleNumber = 2000;

        //! Number of threads
        int m_numThreads = 2;

        //! Particles
        std::vector<Particle *> m_particles;

        //! Manager instance
        Effekseer::Manager *m_manager = nullptr;

        //! Renderer instance
        EffekseerRenderer::Renderer *m_renderer = nullptr;

        //! Particle texture loader
        ParticleTextureLoader *m_textureLoader = nullptr;

        //! Projection matrix
        Mat4 m_projectionMatrix = {};

        //! Camera matrix
        Mat4 m_cameraMatrix = {};
    };
} // namespace ige::scene
