#pragma once

#include <set>
#include <algorithm>

#include "utils/PyxieHeaders.h"
using namespace pyxie;

#include "components/Component.h"

#include "event/Event.h"

namespace ige::scene
{
    //! Canvas: the playground of UI elements
    class Canvas : public Component
    {
    public:
        //! Constructor/destructor
        Canvas(SceneObject &owner);
        virtual ~Canvas();

        //! Get component name
        virtual std::string getName() const override { return "Canvas"; }

        //! Serialize
        void to_json(json &j) const override;

        //! Deserialize
        void from_json(const json &j) override;

        //! Canvas to viewport matrix
        void setCanvasToViewportMatrix(const Mat4 &matrix);
        const Mat4 &getCanvasToViewportMatrix() const;

        //! Viewport to canvas matrix
        const Mat4 &getViewportToCanvasMatrix() const;

        //! Design canvas size
        const Vec2 &getDesignCanvasSize() const { return m_canvasSize; }

        // Set design canvas size
        void setDesignCanvasSize(const Vec2 &canvasSize);

        //! Target canvas size
        Vec2 getTargetCanvasSize() const { return m_targetCanvasSize; }

        // Set target canvas size, based on device resolution
        void setTargetCanvasSize(const Vec2 &canvasSize);

    public:
        //! Default canvas size
        static Vec2 s_defaultCanvasSize;

    protected:
        //! Canvas to viewport matrix
        Mat4 m_canvasToViewportMatrix;

        //! Viewport to canvas matrix
        Mat4 m_viewportToCanvasMatrix;

        //! Canvas size
        Vec2 m_canvasSize;

        //! Target canvas size
        Vec2 m_targetCanvasSize;

        //! Device scale
        Vec2 m_deviceScale;
    };
} // namespace ige::scene
