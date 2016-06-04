#include "NewbieGuideLayer.h"
#include "AdvertHelper.h"

USING_NS_CC;
using namespace ui;

bool NewbieGuideLayer::init()
{
	if ( !LayerColor::initWithColor(Color4B(0, 0, 0, 150)))
    {
        return false;
    }

	// 从json中导入ui
	Layout* mainPanel = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		FileUtils::getInstance()->fullPathForFilename("ui/NewbieGuideLayout.json").c_str()));
	_newbieGuidePageView = static_cast<PageView*>(mainPanel->getChildByName("NewbieGuidePagetView"));

	// 从json中读出页面并加入PageView
	Layout* layout1 = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		FileUtils::getInstance()->fullPathForFilename("ui/NewbieGuide1Layout.json").c_str()));
	auto layout1NextPageButton = static_cast<PageView*>(layout1->getChildByName("NextPageButton"));
	layout1NextPageButton->addTouchEventListener(this, toucheventselector(NewbieGuideLayer::nextPageButtonTouched));
	_newbieGuidePageView->addPage(layout1);

	Layout* layout2 = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		FileUtils::getInstance()->fullPathForFilename("ui/NewbieGuide2Layout.json").c_str()));
	auto layout2NextPageButton = static_cast<PageView*>(layout2->getChildByName("NextPageButton"));
	layout2NextPageButton->addTouchEventListener(this, toucheventselector(NewbieGuideLayer::nextPageButtonTouched));
	_newbieGuidePageView->addPage(layout2);

	Layout* layout3 = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		FileUtils::getInstance()->fullPathForFilename("ui/NewbieGuide3Layout.json").c_str()));
	auto layout3NextPageButton = static_cast<PageView*>(layout3->getChildByName("NextPageButton"));
	layout3NextPageButton->addTouchEventListener(this, toucheventselector(NewbieGuideLayer::nextPageButtonTouched));
	_newbieGuidePageView->addPage(layout3);

	Layout* layout4 = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		FileUtils::getInstance()->fullPathForFilename("ui/NewbieGuide4Layout.json").c_str()));
	auto layout4CloseButton = static_cast<PageView*>(layout4->getChildByName("CloseButton"));
	layout4CloseButton->addTouchEventListener(this, toucheventselector(NewbieGuideLayer::closeButtonTouched));
	_newbieGuidePageView->addPage(layout4);
	
	this->addChild(mainPanel);

	// 模态效果
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	_listener = EventListenerTouchOneByOne::create();
	_listener->onTouchBegan = CC_CALLBACK_2(NewbieGuideLayer::onTouchBegan,this);
	_listener->setSwallowTouches(true);
	dispatcher->addEventListenerWithSceneGraphPriority(_listener,this);

	return true;
}

bool NewbieGuideLayer::onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent)
{
	return true;
}

void NewbieGuideLayer::nextPageButtonTouched(Ref *pSender, TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		_newbieGuidePageView->scrollToPage(_newbieGuidePageView->getCurPageIndex() + 1);
	}
}

void NewbieGuideLayer::closeButtonTouched(Ref *pSender, TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		auto dispatcher = Director::getInstance()->getEventDispatcher();
		dispatcher->removeEventListener(_listener);

		this->removeFromParentAndCleanup(false);

		// 显示广告
		AdvertHelper::showAdvert();
	}
}


