#include "SloganSence.h"
#include "GameStartSence.h"
#include "PhotoManager.h"

USING_NS_CC;
using namespace ui;

Scene* SloganSence::scene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    SloganSence *layer = SloganSence::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool SloganSence::init()
{
	if ( !Layer::init() )
    {
        return false;
    }

	// loading 界面
	auto slogan = Sprite::create(FileUtils::getInstance()->fullPathForFilename("slogan.png"));
	slogan->setAnchorPoint(Point(0,0));
	this->addChild(slogan);

	auto loading = Sprite::create(FileUtils::getInstance()->fullPathForFilename("loading.png"));
	auto size = Director::getInstance()->getWinSize();
	loading->setPosition(Point(size.width - 150, 30 ));
	loading->runAction(RepeatForever::create(Blink::create(3.0f,2)));
    this->addChild(loading);

	// 设置异步加载数据
	_numberOfSprites = 0;
	_numberOfLoadedSprites = 0;

	auto photos = PhotoManager::getInstance()->getAllPhotos();
	for(auto& photoList : photos)
	{
		for(auto& photo : photoList)
		{
			++_numberOfSprites;
			Director::getInstance()->getTextureCache()->addImageAsync(FileUtils::getInstance()->fullPathForFilename(photo)
				, CC_CALLBACK_1(SloganSence::loadingCallBack, this));
		}
	}
	
	return true;
}

void SloganSence::loadingCallBack(cocos2d::Texture2D *texture)
{
    ++_numberOfLoadedSprites;

    if (_numberOfLoadedSprites == _numberOfSprites)
    {
        auto newSence = TransitionFade::create(1.f,GameStartSence::scene());
		if (newSence)
		{
			Director::getInstance()->replaceScene(newSence);
		}
    }
}

