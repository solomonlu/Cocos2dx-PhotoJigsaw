#ifndef __FRAME_PHOTOSELECTSCENE_H__
#define __FRAME_PHOTOSELECTSCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

class PhotoSelectSence : public cocos2d::Layer
{
public:
    static cocos2d::Scene* scene();
	CREATE_FUNC(PhotoSelectSence);
	
	virtual bool init() override;

	void startButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void backButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void categoryButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void selectedPhotoEvent(Ref *pSender, cocos2d::ui::ListViewEventType type);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event* event);
private:
	PhotoSelectSence();
	void backToUpperUI();

	cocos2d::ui::Layout* _photoSelectLayout;
	cocos2d::ui::ListView* _photoListView;
	cocos2d::ui::ImageView* _selectBox;

	enum class SelectMode
    {
		IN_CATEGORY_SELECT_MODE = 0,	// 分类选择界面
		IN_PHOTO_SELECT_MODE,			// 照片选择界面
    };
	SelectMode _selectMode;
	bool _selectPhotoStarted;
	int _selectPhotoStartIndex;
	int _selectedPhotoIndex;
	int _selectedCategoryIndex;
};

#endif // __FRAME_PHOTOSELECTSCENE_H__