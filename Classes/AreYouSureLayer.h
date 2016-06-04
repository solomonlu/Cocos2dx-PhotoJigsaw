#ifndef __FRAME_AREYOUSURELAYER_H__
#define __FRAME_AREYOUSURELAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

class AreYouSureLayer: public cocos2d::LayerColor
{
public:
    virtual bool initWithFunction(const std::function<void()> &func);
	void yesButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void noButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	bool onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent);
private:
	cocos2d::EventListenerTouchOneByOne* _listener;
	std::function<void()> _function;
};

#endif // __FRAME_AREYOUSURELAYER_H__
