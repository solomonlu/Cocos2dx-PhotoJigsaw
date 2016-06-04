#ifndef __FRAME_GAMESTARTSCENE_H__
#define __FRAME_GAMESTARTSCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

class GameStartSence : public cocos2d::Layer
{
public:
    static cocos2d::Scene* scene();
	CREATE_FUNC(GameStartSence);

	virtual bool init() override;
	void startButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void illustrationButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void setupButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event* event);
private:
	unsigned long _backKeyPressedTime;
};

#endif // __FRAME_GAMESTARTSCENE_H__