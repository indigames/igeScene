#ifndef __MACROS_SCENE_H__
#define __MACROS_SCENE_H__

#define NS_IGE_SCENE_BEGIN				namespace ige::scene {
#define NS_IGE_SCENE_END                }
#define USING_NS_IGE_CREATOR            using namespace ige::creator;
#define USING_NS_IGE_SCENE				using namespace ige::scene;
#define NS_IGE_SCENE					ige::scene
#define USING_NS_PYXIE					using namespace pyxie;	

#define IGE_PLATFORM_UNKNOWN            0
#define IGE_PLATFORM_EDITOR				1

#define IGE_PLATFORM					IGE_PLATFORM_UNKNOWN

#if defined(_WIN32) && defined(_WINDOWS)
#if defined(_FINAL)
#else
#undef  IGE_PLATFORM
#define IGE_PLATFORM					IGE_PLATFORM_EDITOR
#endif //_FINAL
#endif


#define RAD_TO_DEGREE                   57.2957795f
#define DEGREE_TO_RAD                   0.01745329f
#define DEG360_TO_RAD                   6.2831844f


#define CALLBACK_0(__selector__,__target__, ...) std::bind(&__selector__,__target__, ##__VA_ARGS__)
#define CALLBACK_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1)
#define CALLBACK_2(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define CALLBACK_3(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)

#endif // __MACROS_SCENE_H__