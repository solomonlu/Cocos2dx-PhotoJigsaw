#include "AreYouSureLayer.h"

USING_NS_CC;
using namespace ui;

bool AreYouSureLayer::initWithFunction(const std::function<void()> &func)
{
	if ( !LayerColor::initWithColor(Color4B(0, 0, 0, 150)))
    {
        return false;
    }

	_function = func;

	// 从json中导入ui
	Layout* mainPanel = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		FileUtils::getInstance()->fullPathForFilename("ui/AreYouSureLayout.json").c_str()));

	// 按钮绑定回调
	auto yesButton = static_cast<Button*>(mainPanel->getChildByName("YesButton"));
	yesButton->addTouchEventListener(this, toucheventselector(AreYouSureLayer::yesButtonTouched));

	auto noButton = static_cast<Button*>(mainPanel->getChildByName("NoButton"));
	noButton->addTouchEventListener(this, toucheventselector(AreYouSureLayer::noButtonTouched));

	this->addChild(mainPanel);

	// 模态效果
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	_listener = EventListenerTouchOneByOne::create();
	_listener->onTouchBegan = CC_CALLBACK_2(AreYouSureLayer::onTouchBegan,this);
	_listener->setSwallowTouches(true);
	dispatcher->addEventListenerWithSceneGraphPriority(_listener,this);

	return true;
}

bool AreYouSureLayer::onTouchBegan(cocos2d::Touch *pTouch, cocos2d::Event *pEvent)
{
	return true;
}

void AreYouSureLayer::yesButtonTouched(Ref *pSender, TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		_function();

		auto dispatcher = Director::getInstance()->getEventDispatcher();
		dispatcher->removeEventListener(_listener);

		this->removeFromParentAndCleanup(false);
	}
}

void AreYouSureLayer::noButtonTouched(Ref *pSender, TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		auto dispatcher = Director::getInstance()->getEventDispatcher();
		dispatcher->removeEventListener(_listener);

		this->removeFromParentAndCleanup(false);
	}
}
