#ifndef __FRAME_ILLUSTRATIONSCENE_H__
#define __FRAME_ILLUSTRATIONSCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

class IllustrationSence : public cocos2d::Layer
{
public:
    static cocos2d::Scene* scene();
	CREATE_FUNC(IllustrationSence);
	
	virtual bool init() override;

	void backButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void deleteButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void selectedFragmentEvent(Ref *pSender, cocos2d::ui::PageViewEventType type);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event* event);
private:
	void deleteFragment();

	cocos2d::ui::TextBMFont* _currentPhotoIndexText;
	cocos2d::ui::TextBMFont* _totalPhotoIndexText;
	cocos2d::ui::PageView* _photoFragmentPageView;
};

#endif // __FRAME_ILLUSTRATIONSCENE_H__