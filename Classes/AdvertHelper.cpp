#include "AdvertHelper.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#endif

USING_NS_CC;

void AdvertHelper::showAdvert()
{
#if 0
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo minfo;    
    bool b = JniHelper::getStaticMethodInfo(minfo,     
        "org/cocos2dx/cpp/AdvertHelper",  
        "showAdvert",  
        "()V");  
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);    
#endif
}

void AdvertHelper::hideAdvert()
{
#if 0
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo minfo;    
    bool b = JniHelper::getStaticMethodInfo(minfo,     
        "org/cocos2dx/cpp/AdvertHelper",  
        "hideAdvert",  
        "()V");  
    minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);    
#endif
}
