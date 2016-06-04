#include "PhotoSelectSence.h"
#include "MainSence.h"
#include "GameStartSence.h"
#include "PhotoManager.h"

USING_NS_CC;
using namespace ui;

/*
 * 带选中框的ImageView
 */
class ImageViewWithSelectBox : public ImageView
{
public:
	ImageViewWithSelectBox(){}
	static ImageViewWithSelectBox* create(const std::string &imageFileName
		,TextureResType texType = UI_TEX_TYPE_LOCAL)
	{
		ImageViewWithSelectBox *widget = new ImageViewWithSelectBox;
		if (widget && widget->init(FileUtils::getInstance()->fullPathForFilename("select_box.png"), texType)) 
		{
			auto _photo = Sprite::create(imageFileName);
			_photo->setAnchorPoint(Point(0,0));
			_photo->setPosition(Point(20,20));
			Sprite* _selectBox = (Sprite*)(widget->getVirtualRenderer());
			_selectBox->setVisible(false);
			widget->addChild(_photo);
			widget->autorelease();
			return widget;
		}
		CC_SAFE_DELETE(widget);
		return nullptr;
	}

	void startBlink()
	{
		Sprite* _selectBox = (Sprite*)(this->getVirtualRenderer());
		_selectBox->setVisible(true);
		auto blinkAction = RepeatForever::create(Blink::create(1.0f,2));
		_selectBox->runAction(blinkAction);
	}

	void stopBlink()
	{
		Sprite* _selectBox = (Sprite*)(this->getVirtualRenderer());
		_selectBox->setVisible(false);
		_selectBox->stopAllActions();
	}
};

///////////////////////////////////////////

Scene* PhotoSelectSence::scene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    PhotoSelectSence *layer = PhotoSelectSence::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

PhotoSelectSence::PhotoSelectSence()
	:_photoListView(nullptr)
	,_selectBox(nullptr)
	,_selectMode(SelectMode::IN_CATEGORY_SELECT_MODE)
	,_selectPhotoStarted(false)
	,_selectPhotoStartIndex(-1)
	,_selectedPhotoIndex(0)
	,_selectedCategoryIndex(0)
{
}

bool PhotoSelectSence::init()
{
	if ( !Layer::init() )
    {
        return false;
    }

	// 从json中导入ui
	Layout* mainPanel = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		FileUtils::getInstance()->fullPathForFilename("ui/PhotoSelectLayout.json").c_str()));

	_photoSelectLayout = static_cast<Layout*>(mainPanel->getChildByName("PhotoSelectPanel"));

	// 按钮绑定回调
	auto backButton = static_cast<Button*>(mainPanel->getChildByName("BackButton"));
	backButton->addTouchEventListener(this, toucheventselector(PhotoSelectSence::backButtonTouched));

	auto startButton = static_cast<Button*>(_photoSelectLayout->getChildByName("StartButton"));
	startButton->addTouchEventListener(this, toucheventselector(PhotoSelectSence::startButtonTouched));

	// 分类按钮赋值
	auto categoryListView = static_cast<ScrollView*>(_photoSelectLayout->getChildByName("CategoryScrollView"));
	auto categoryGirlsButton = static_cast<Button*>(categoryListView->getChildByName("CategoryGirlsButton"));
	categoryGirlsButton->setTag(0);
	categoryGirlsButton->addTouchEventListener(this, toucheventselector(PhotoSelectSence::categoryButtonTouched));
	auto categoryBoysButton = static_cast<Button*>(categoryListView->getChildByName("CategoryBoysButton"));
	categoryBoysButton->setTag(1);
	categoryBoysButton->addTouchEventListener(this, toucheventselector(PhotoSelectSence::categoryButtonTouched));
	auto categoryAnimalsButton = static_cast<Button*>(categoryListView->getChildByName("CategoryAnimalsButton"));
	categoryAnimalsButton->setTag(2);
	categoryAnimalsButton->addTouchEventListener(this, toucheventselector(PhotoSelectSence::categoryButtonTouched));
	auto categoryLandscapesButton = static_cast<Button*>(categoryListView->getChildByName("CategoryLandscapesButton"));
	categoryLandscapesButton->setTag(3);
	categoryLandscapesButton->addTouchEventListener(this, toucheventselector(PhotoSelectSence::categoryButtonTouched));

	_photoListView = static_cast<ListView*>(_photoSelectLayout->getChildByName("PhotoListView"));
	_photoListView->addEventListenerListView(this, listvieweventselector(PhotoSelectSence::selectedPhotoEvent));

	this->addChild(mainPanel);

	// 处理该界面的按键操作
	auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyReleased = CC_CALLBACK_2(PhotoSelectSence::onKeyReleased, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	return true;
}

void PhotoSelectSence::startButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		auto photos = PhotoManager::getInstance()->getAllPhotos();
		auto selectPhoto = photos[_selectedCategoryIndex][_selectedPhotoIndex];
		TransitionScene* newSence = TransitionSlideInR::create(1.f,MainSence::scene(selectPhoto));

		if (newSence)
		{
			Director::getInstance()->replaceScene(newSence);
		}
	}
}

void PhotoSelectSence::backButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		backToUpperUI();
	}
}

void PhotoSelectSence::categoryButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		if (_selectMode == SelectMode::IN_PHOTO_SELECT_MODE)
		{
			return;
		}
		auto* button = static_cast<Button*>(pSender);
		_selectedCategoryIndex = button->getTag();

		_selectedPhotoIndex = 0;
		_photoListView->removeAllItems();
		_photoListView->jumpToLeft();

		// 显示照片ListView
		auto photos = PhotoManager::getInstance()->getAllPhotos();
		for(auto& photoName : photos[_selectedCategoryIndex])
		{
			auto photo = ImageViewWithSelectBox::create(FileUtils::getInstance()->fullPathForFilename(photoName));
			photo->setTouchEnabled(true);
			_photoListView->pushBackCustomItem(photo);
		}

		// 一开始就是第一个开始闪
		((ImageViewWithSelectBox*)_photoListView->getItem(0))->startBlink();

		// 整个页面左移
		auto moveLeftAction = MoveTo::create(1.f,Point(-640,0));
		_photoSelectLayout->runAction(moveLeftAction);
		_selectMode = SelectMode::IN_PHOTO_SELECT_MODE;
	}
}

void PhotoSelectSence::selectedPhotoEvent(Ref *pSender, ListViewEventType type)
{
    switch (type)
    {
        case LISTVIEW_ONSELECTEDITEM_START:
        {
            ListView* listView = static_cast<ListView*>(pSender);
            CC_UNUSED_PARAM(listView);
            CCLOG("select child start index = %ld", listView->getCurSelectedIndex());
			_selectPhotoStarted = true;
			_selectPhotoStartIndex = listView->getCurSelectedIndex();
            break;
        }
        case LISTVIEW_ONSELECTEDITEM_END:
        {
            ListView* listView = static_cast<ListView*>(pSender);
            CC_UNUSED_PARAM(listView);
            CCLOG("select child end index = %ld", listView->getCurSelectedIndex());
			// 点击起始的Index和点击结束的Index一致才能为有效选择
			if (_selectPhotoStartIndex == listView->getCurSelectedIndex())
			{
				// 先原来的照片停止闪
				((ImageViewWithSelectBox*)listView->getItem(_selectedPhotoIndex))->stopBlink();
				// 再选中的照片开始闪
				((ImageViewWithSelectBox*)listView->getItem(_selectPhotoStartIndex))->startBlink();
				// 更新索引
				_selectedPhotoIndex = _selectPhotoStartIndex;
				_selectPhotoStartIndex = -1;
			}
            break;
        }
        default:
            break;
    }
}

void PhotoSelectSence::onKeyReleased(EventKeyboard::KeyCode key, Event* event)
{
	if (EventKeyboard::KeyCode::KEY_RETURN == key
		|| EventKeyboard::KeyCode::KEY_ESCAPE == key
		|| EventKeyboard::KeyCode::KEY_BACKSPACE == key)
    {
		backToUpperUI();
    }
}

void PhotoSelectSence::backToUpperUI()
{
	if (_selectMode == SelectMode::IN_PHOTO_SELECT_MODE)
	{
		// 整个页面右移
		auto moveRightAction = MoveTo::create(1.f,Point(0,0));
		_photoSelectLayout->runAction(moveRightAction);

		_selectMode = SelectMode::IN_CATEGORY_SELECT_MODE;
	}
	else if (_selectMode == SelectMode::IN_CATEGORY_SELECT_MODE)
	{
		auto newSence = TransitionSlideInL::create(1.f,GameStartSence::scene());
		if (newSence)
		{
			Director::getInstance()->replaceScene(newSence);
		}
	}
	else
	{
	}
}
