#include "IllustrationSence.h"
#include "GameStartSence.h"
#include "NoIllustrationLayer.h"
#include "AreYouSureLayer.h"
#include "PhotoManager.h"

USING_NS_CC;
using namespace ui;

Scene* IllustrationSence::scene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    IllustrationSence *layer = IllustrationSence::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}


bool IllustrationSence::init()
{
	if ( !Layer::init() )
    {
        return false;
    }

	// 从json中导入ui
	Layout* mainPanel = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		FileUtils::getInstance()->fullPathForFilename("ui/IllustrationLayout.json").c_str()));

	// 按钮绑定回调
	auto backButton = static_cast<Button*>(mainPanel->getChildByName("BackButton"));
	backButton->addTouchEventListener(this, toucheventselector(IllustrationSence::backButtonTouched));

	auto deleteButton = static_cast<Button*>(mainPanel->getChildByName("DeleteButton"));
	deleteButton->addTouchEventListener(this, toucheventselector(IllustrationSence::deleteButtonTouched));

	_photoFragmentPageView = static_cast<PageView*>(mainPanel->getChildByName("PhotoFragmentPagetView"));
	_photoFragmentPageView->addEventListenerPageView(this, pagevieweventselector(IllustrationSence::selectedFragmentEvent));

	_currentPhotoIndexText = static_cast<TextBMFont*>(mainPanel->getChildByName("CurrentPhotoIndexLabel"));
	_totalPhotoIndexText = static_cast<TextBMFont*>(mainPanel->getChildByName("TotalPhotoIndexLabel"));

	this->addChild(mainPanel);

	// 处理该界面的按键操作
	auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyReleased = CC_CALLBACK_2(IllustrationSence::onKeyReleased, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	// 获取所有已经获得的碎片
	auto photoFragments = PhotoManager::getInstance()->getAllPhotoFragments();
	if (photoFragments.empty())
	{
		_currentPhotoIndexText->setText("0");
		_totalPhotoIndexText->setText("0");

		NoIllustrationLayer* layer = new NoIllustrationLayer();
		layer->init();
		layer->autorelease();
		this->addChild(layer);
	}
	else
	{
		char str[5] = {0};
		sprintf(str,"%d",photoFragments.size());
		_currentPhotoIndexText->setText("1");
		_totalPhotoIndexText->setText(str);

		Size winSize = Director::getInstance()->getWinSize();
		const int piecesInWidth = 3;
		const int piecesInHeight = 4;
		const int totalFragment = piecesInWidth*piecesInHeight;

		for(auto& photoFragmentProperty : photoFragments)
		{
			auto layout = Layout::create();

			Texture2D *photoTexture = Director::getInstance()->getTextureCache()->addImage(
				FileUtils::getInstance()->fullPathForFilename(photoFragmentProperty.photoFileName));

			int photoWidth = photoTexture->getPixelsWide();
			int photoHeight = photoTexture->getPixelsHigh();
			int fragmentWidth = photoWidth/piecesInWidth;
			int fragmentHeight = photoHeight/piecesInHeight;
			float scale = winSize.width/photoWidth;

			// 如果碎片全收集齐了
			if (photoFragmentProperty.FragmentMask == ((1 << totalFragment) - 1))
			{
				auto photo = Sprite::createWithTexture(photoTexture);
				photo->setAnchorPoint(Point(0,0));
			
				layout->addChild(photo);
				layout->setScale(scale);
			}
			// 如果碎片没收集齐，那就分片显示
			else
			{
				for (int i = 0 ; i < totalFragment ; ++i)
				{
					int indexX = i%piecesInWidth;
					int indexY = i/piecesInWidth;

					Sprite* photoFragment = nullptr;
					if (photoFragmentProperty.FragmentMask & (1<<i))
					{
						photoFragment = Sprite::createWithTexture(photoTexture,Rect(indexX*fragmentWidth,indexY*fragmentHeight,fragmentWidth,fragmentHeight));
					}
					else
					{
						photoFragment = Sprite::create(FileUtils::getInstance()->fullPathForFilename("fragment_back.png"));
					}

					photoFragment->setAnchorPoint(Point(0,0));
					Point location = Point(indexX*fragmentWidth,photoHeight-(indexY+1)*fragmentHeight);
					photoFragment->setPosition(location);
					layout->addChild(photoFragment);
					layout->setScale(scale);
				}
			}

			_photoFragmentPageView->addPage(layout);
		}
	}

	return true;
}

void IllustrationSence::backButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		auto newSence = TransitionFade::create(1.f,GameStartSence::scene());
		if (newSence)
		{
			Director::getInstance()->replaceScene(newSence);
		}
	}
}

void IllustrationSence::deleteButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		auto photoFragments = PhotoManager::getInstance()->getAllPhotoFragments();
		if (photoFragments.empty())
		{
			return;
		}

		AreYouSureLayer* layer = new AreYouSureLayer();
		layer->initWithFunction(CC_CALLBACK_0(IllustrationSence::deleteFragment,this));
		layer->autorelease();
		this->addChild(layer);
	}
}

void IllustrationSence::deleteFragment()
{
	auto photoFragments = PhotoManager::getInstance()->getAllPhotoFragments();
	if (photoFragments.empty())
	{
		return;
	}

	auto oldPhotoFragmentsCount = photoFragments.size();
	auto photoFileName = photoFragments[_photoFragmentPageView->getCurPageIndex()].photoFileName;

	// 文件中删掉
	PhotoManager::getInstance()->delFragment(photoFileName);

	// 当前page页也删掉
	_photoFragmentPageView->removePageAtIndex(_photoFragmentPageView->getCurPageIndex());

	auto newPhotoFragmentsCount = oldPhotoFragmentsCount - 1;
	ssize_t currentPageIndex;
	if (newPhotoFragmentsCount == 0)
	{
		currentPageIndex = 0;
	}
	else
	{
		currentPageIndex = _photoFragmentPageView->getCurPageIndex() + 1;
	}
	// 更新一下索引
	char str[5] = {0};
	sprintf(str,"%d",currentPageIndex);
	_currentPhotoIndexText->setText(str);
	sprintf(str,"%d",newPhotoFragmentsCount);
	_totalPhotoIndexText->setText(str);
}

void IllustrationSence::selectedFragmentEvent(Ref *pSender, cocos2d::ui::PageViewEventType type)
{
	char str[5] = {0};
	sprintf(str,"%d",_photoFragmentPageView->getCurPageIndex() + 1);
	_currentPhotoIndexText->setText(str);
}

void IllustrationSence::onKeyReleased(EventKeyboard::KeyCode key, Event* event)
{
	if (EventKeyboard::KeyCode::KEY_RETURN == key
		|| EventKeyboard::KeyCode::KEY_ESCAPE == key
		|| EventKeyboard::KeyCode::KEY_BACKSPACE == key)
    {
		auto newSence = TransitionFade::create(1.f,GameStartSence::scene());
		if (newSence)
		{
			Director::getInstance()->replaceScene(newSence);
		}
    }
}

