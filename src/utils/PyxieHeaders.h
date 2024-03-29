#pragma once

#include <pyxie.h>
#include <pyxieCamera.h>
#include <pyxieShowcase.h>
#include <pyxieTexture.h>
#include <pyxieEnvironmentSet.h>
#include <pyxieRenderContext.h>
#include <pyxieRenderTarget.h>
#include <pyxieFigure.h>
#include <pyxieEditableFigure.h>
#include <pyxieResourceCreator.h>
#include <pyxieResourceManager.h>
#include <input/pyxieInputHandler.h>
#include <input/pyxieTouch.h>
#include <input/pyxieKeyboard.h>
#include <pyxieFios.h>
#include <pyxieSystemInfo.h>
#include <pyxieApplication.h>
#include <pyxieMathutil.h>

using Camera = pyxie::pyxieCamera;
using Showcase = pyxie::pyxieShowcase;
using Texture = pyxie::pyxieTexture;
using Environment = pyxie::pyxieEnvironmentSet;
using RenderContext = pyxie::pyxieRenderContext;
using Figure = pyxie::pyxieFigure;
using EditableFigure = pyxie::pyxieEditableFigure;
using BaseFigure = pyxie::pyxieFigureBase;
using RenderTarget = pyxie::pyxieRenderTarget;
using Resource = pyxie::pyxieResource;
using ResourceCreator = pyxie::pyxieResourceCreator;
using ResourceManager = pyxie::pyxieResourceManager;
using InputHandler = pyxie::InputHandler;
using Touch = pyxie::TouchDevice;
using Application = pyxie::pyxieApplication;
using SystemInfo = pyxie::pyxieSystemInfo;
using FileIO = pyxie::pyxieFios;
using Time = pyxie::pyxieTime;
using ShaderDescriptor = pyxie::pyxieShaderDescriptor;
using Drawable = pyxie::pyxieDrawable;
using AABBox = pyxie::pyxieAABBox;
using Animator = pyxie::pyxieAnimator;

using namespace pyxie;
