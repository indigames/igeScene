#ifndef __IGE_COMPONENT_H__
#define __IGE_COMPONENT_H__

#include "components/Component.h"
#include "components/CameraComponent.h"
#include "components/EnvironmentComponent.h"
#include "components/FigureComponent.h"
#include "components/ScriptComponent.h"
#include "components/SpriteComponent.h"
#include "components/TransformComponent.h"
#include "components/TextComponent.h"

#include "components/audio/AudioListener.h"
#include "components/audio/AudioSource.h"
#include "components/audio/AudioManager.h"

#include "components/light/AmbientLight.h"
#include "components/light/DirectionalLight.h"
#include "components/light/PointLight.h"
#include "components/light/SpotLight.h"

#include "components/physic/Rigidbody.h"
#include "components/physic/BoxCollider.h"
#include "components/physic/CapsuleCollider.h"
#include "components/physic/SphereCollider.h"
#include "components/physic/Softbody.h"

#include "components/particle/Particle.h"

#include "components/navigation/NavAgent.h"
#include "components/navigation/NavMesh.h"
#include "components/navigation/DynamicNavMesh.h"
#include "components/navigation/NavAgent.h"
#include "components/navigation/Navigable.h"
#include "components/navigation/OffMeshLink.h"
#include "components/navigation/NavObstacle.h"

#include "components/gui/RectTransform.h"
#include "components/gui/Canvas.h"
#include "components/gui/UIImage.h"
#include "components/gui/UIText.h"
#endif