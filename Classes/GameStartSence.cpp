#include "GameStartSence.h"
#include "PhotoSelectSence.h"
#include "IllustrationSence.h"
#include "SetupLayer.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#endif

USING_NS_CC;
using namespace ui;

const int SETUP_LAYER_TAG = 0;

Scene* GameStartSence::scene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    GameStartSence *layer = GameStartSence::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool GameStartSence::init()
{
	if ( !Layer::init() )
    {
        return false;
    }

	// 从json中导入ui
	Layout* mainPanel = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		FileUtils::getInstance()->fullPathForFilename("ui/GameStartLayout.json").c_str()));

	// 按钮绑定回调
	auto startButton = static_cast<Button*>(mainPanel->getChildByName("StartButton"));
	startButton->addTouchEventListener(this, toucheventselector(GameStartSence::startButtonTouched));

	auto illustrationButton = static_cast<Button*>(mainPanel->getChildByName("IllustrationButton"));
	illustrationButton->addTouchEventListener(this, toucheventselector(GameStartSence::illustrationButtonTouched));

	auto setupButton = static_cast<Button*>(mainPanel->getChildByName("SetupButton"));
	setupButton->addTouchEventListener(this, toucheventselector(GameStartSence::setupButtonTouched));

	this->addChild(mainPanel);

	// 处理该界面的按键操作
	auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyReleased = CC_CALLBACK_2(GameStartSence::onKeyReleased, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);
	
	return true;
}

void GameStartSence::startButtonTouched(Ref *pSender, TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		auto newSence = TransitionSlideInR::create(1.f,PhotoSelectSence::scene());
		if (newSence)
		{
			Director::getInstance()->replaceScene(newSence);
		}
	}
}

void GameStartSence::illustrationButtonTouched(Ref *pSender, TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		auto newSence = TransitionFade::create(1.5f,IllustrationSence::scene());
		if (newSence)
		{
			Director::getInstance()->replaceScene(newSence);
		}
	}
}

void GameStartSence::setupButtonTouched(Ref *pSender, TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		auto layerExist = this->getChildByTag(SETUP_LAYER_TAG);
		if (!layerExist)
		{
			SetupLayer* layer = new SetupLayer();
			layer->init();
			layer->autorelease();
			this->addChild(layer,0,SETUP_LAYER_TAG);
		}
	}
}

void GameStartSence::onKeyReleased(EventKeyboard::KeyCode key, Event* event)
{
	if (EventKeyboard::KeyCode::KEY_RETURN == key
		|| EventKeyboard::KeyCode::KEY_ESCAPE == key
		|| EventKeyboard::KeyCode::KEY_BACKSPACE == key)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		unsigned long now = time(0);
		if (now - _backKeyPressedTime > 2)
		{
			_backKeyPressedTime = now;
			JniMethodInfo minfo;
			jobject jobj;    
			bool b = JniHelper::getStaticMethodInfo(minfo,     
				"org/cocos2dx/cpp/ToastHelper",  
				"showToast",
				"()V");
			jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		}
		else
		{
			Director::getInstance()->end();
		}
#else
		Director::getInstance()->end();
#endif
    }
}

