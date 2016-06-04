#ifndef __FRAME_NEWBIEGUIDELAYER_H__
#define __FRAME_NEWBIEGUIDELAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

class NewbieGuideLayer: public cocos2d::LayerColor
{
public:
    virtual bool init();
	void nextPageButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void closeButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	bool onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent);
private:
	cocos2d::EventListenerTouchOneByOne* _listener;

	cocos2d::ui::PageView* _newbieGuidePageView;
};

#endif // __FRAME_NEWBIEGUIDELAYER_H__
