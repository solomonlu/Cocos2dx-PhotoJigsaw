#ifndef __FRAME_SETUPLAYER_H__
#define __FRAME_SETUPLAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

class SetupLayer: public cocos2d::LayerColor
{
public:
    virtual bool init();
	void quitButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void musicButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void effectButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void newbieGuideCheckBoxTouched(Ref *pSender, cocos2d::ui::CheckBoxEventType type);
	bool onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent);
private:
	cocos2d::EventListenerTouchOneByOne* _listener;
};

#endif // __FRAME_SETUPLAYER_H__
