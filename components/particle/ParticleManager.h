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
#include <Effekseer/Effekseer.TextureLoader.h>

#include "utils/PyxieHeaders.h"
using namespace pyxie;

namespace ige::scene
{
    class Particle;

    //! Class ParticleTextureLoader: load IGE texture to Effekseer
    class ParticleTextureLoader : public Effekseer::TextureLoader
    {
    public:
        //! Constructor
        ParticleTextureLoader();

        //! Load
        Effekseer::TextureRef Load(const char16_t* path, Effekseer::TextureType textureType) override;       
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

        //! Returns the type of the component
        virtual Type getType() const override { return Type::ParticleManager; }

        //! Update
        void onUpdate(float dt) override;

        //! Render.
        void onRender() override;

        //! Get manager
        Effekseer::ManagerRef getEffekseerManager() { return m_manager; }

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

        //! Check if multple edit allowed
        virtual bool canMultiEdit() override { return false; }

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
        int m_numThreads = 1;

        //! Particles
        std::vector<Particle *> m_particles;

        //! Manager instance
        Effekseer::ManagerRef m_manager = nullptr;

        //! Renderer instance
        EffekseerRenderer::RendererRef m_renderer = nullptr;

        //! Projection matrix
        Mat4 m_projectionMatrix = {};

        //! Camera matrix
        Mat4 m_cameraMatrix = {};
    };
} // namespace ige::scene
