#ifndef __FRAME_NOILLUSTRATIONLAYER_H__
#define __FRAME_NOILLUSTRATIONLAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

class NoIllustrationLayer: public cocos2d::LayerColor
{
public:
    virtual bool init();
	void okButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	bool onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent);
private:
	cocos2d::EventListenerTouchOneByOne* _listener;
};

#endif // __FRAME_NOILLUSTRATIONLAYER_H__
