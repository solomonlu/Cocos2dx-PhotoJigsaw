#ifndef __APPMACROS_H__
#define __APPMACROS_H__

#include "cocos2d.h"

/* For demonstrating using one design resolution to match different resources,
   or one resource to match different design resolutions.

   [Situation 1] Using one design resolution to match different resources.
     Please look into Appdelegate::applicationDidFinishLaunching.
     We check current device frame size to decide which resource need to be selected.
     So if you want to test this situation which said in title '[Situation 1]',
     you should change ios simulator to different device(e.g. iphone, iphone-retina3.5, iphone-retina4.0, ipad, ipad-retina),
     or change the window size in "proj.XXX/main.cpp" by "CCEGLView::setFrameSize" if you are using win32 or linux plaform
     and modify "proj.mac/AppController.mm" by changing the window rectangle.

   [Situation 2] Using one resource to match different design resolutions.
     The coordinates in your codes is based on your current design resolution rather than resource size.
     Therefore, your design resolution could be very large and your resource size could be small.
     To test this, just define the marco 'TARGET_DESIGN_RESOLUTION_SIZE' to 'DESIGN_RESOLUTION_4096X3072'
     and open iphone simulator or create a window of 960x640 size.

   [Note] Normally, developer just need to define one design resolution(e.g. 960x640) with one or more resources.
 */

#define DESIGN_RESOLUTION_640X960    0
#define DESIGN_RESOLUTION_1536X2048  1
#define DESIGN_RESOLUTION_3072X4096  2

/* If you want to switch design resolution, change next line */
#define TARGET_DESIGN_RESOLUTION_SIZE  DESIGN_RESOLUTION_640X960

typedef struct tagResource
{
    cocos2d::Size size;
    char directory[100];
}Resource;

static Resource smallResource  =  { cocos2d::Size(640, 960),   "iphone" };
static Resource mediumResource =  { cocos2d::Size(1536, 2048),  "ipad"   };
static Resource largeResource  =  { cocos2d::Size(3072, 4096), "ipadhd" };

#if (TARGET_DESIGN_RESOLUTION_SIZE == DESIGN_RESOLUTION_640X960)
static cocos2d::Size designResolutionSize = cocos2d::Size(640, 960);
#elif (TARGET_DESIGN_RESOLUTION_SIZE == DESIGN_RESOLUTION_1536X2048)
static cocos2d::Size designResolutionSize = cocos2d::Size(1536, 2048);
#elif (TARGET_DESIGN_RESOLUTION_SIZE == DESIGN_RESOLUTION_3072X4096)
static cocos2d::Size designResolutionSize = cocos2d::Size(3072, 4096);
#else
#error unknown target design resolution!
#endif

// The font size 24 is designed for small resolution, so we should change it to fit for current design resolution
#define TITLE_FONT_SIZE  (cocos2d::Director::getInstance()->getOpenGLView()->getDesignResolutionSize().width / smallResource.size.width * 24)

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
static const char* MUSIC_FILE = "music/background.mp3";
static const char* MOVE_START_EFFECT_FILE = "music/move_start.wav";
static const char* MOVE_END_EFFECT_FILE = "music/move_end.wav";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
static const char* MUSIC_FILE = "music/background.ogg";
static const char* MOVE_START_EFFECT_FILE = "music/move_start.wav";
static const char* MOVE_END_EFFECT_FILE = "music/move_end.wav";
#endif

#endif /* __APPMACROS_H__ */
