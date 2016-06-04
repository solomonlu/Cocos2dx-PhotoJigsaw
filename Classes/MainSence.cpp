#include "MainSence.h"
#include "AppMacros.h"
#include "VictorySence.h"
#include "PhotoSelectSence.h"
#include "SimpleAudioEngine.h"
#include "AdvertHelper.h"
#include "NewbieGuideLayer.h"
#include "AreYouSureLayer.h"

USING_NS_CC;
using namespace ui;
using namespace CocosDenshion;

//const static int MW = 1936;		// 拼图的最大宽度
//const static int MH = 2592;		// 拼图的最大高度
//const static int n = 3;			// 拼图X坐标的碎片数量
//const static int m = 4;			// 拼图Y坐标的碎片数量
//const static float W = 640;		// 碎片宽度
//const static float H = 642;		// 碎片高度
//const static int s = 8;			// 碎片之间的缝隙宽度

const int NOMARL_JIGSAW_TAG = 0;
const int BLANK_JIGSAW_TAG = 1;

// 私有的工具函数
static float _getSectionValue(float min,float max,float value)
{
	if (value < min)
	{
		return min;
	}
	else if (value > max)
	{
		return max;
	}
	else
	{
		return value;
	}
}

Scene* MainSence::scene(const std::string& jigsawFileName)
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    MainSence *layer = MainSence::create();
	layer->initJigsaw(jigsawFileName);

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

MainSence::MainSence()
	:_jigsawLayout(nullptr)
	,_photoWidth(0)
	,_photoHeight(0)
	,_jigsawWidth(0)
	,_jigsawHeight(0)
	,_gapWidth(0)
	,_isValidSwap(false)
	,_isSwapLock(false)
	,_inSwapAnyMode(false)
	,_swap1Touched(false)
	,_moveStep(0)
	,_useItem(0)
{
	// TODO 目前是简单模式，以后会有困难模式
	_piecesInWidth = 3;
	_piecesInHeight = 4;

	_startTime = time(0);
}

bool MainSence::init()
{
	if ( !Layer::init() )
    {
        return false;
    }

	// 从json中导入ui
	Layout* mainPanel = static_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		FileUtils::getInstance()->fullPathForFilename("ui/JigsawLayout.json").c_str()));

	// 按钮绑定回调
	auto backButton = static_cast<Button*>(mainPanel->getChildByName("BackButton"));
	backButton->addTouchEventListener(this, toucheventselector(MainSence::backButtonTouched));

	// 拼图移动回调
	_containerLayout = static_cast<Layout*>(mainPanel->getChildByName("ContainerPanel"));

	_jigsawLayout = static_cast<Layout*>(_containerLayout->getChildByName("JigsawPanel"));
	_jigsawLayout->addTouchEventListener(this, toucheventselector(MainSence::jigsawLayerTouched));

	_originalLayout = static_cast<Layout*>(_containerLayout->getChildByName("OriginalPanel"));

	// 道具
	_itemButton = static_cast<Button*>(mainPanel->getChildByName("ItemButton"));
	_itemButton->addTouchEventListener(this, toucheventselector(MainSence::itemButtonTouched));

	// 原图
	_originalButton = static_cast<Button*>(mainPanel->getChildByName("OriginalButton"));
	_originalButton->addTouchEventListener(this, toucheventselector(MainSence::originalButtonTouched));

	// 测试
	auto testButton = static_cast<Button*>(mainPanel->getChildByName("TestButton"));
	testButton->addTouchEventListener(this, toucheventselector(MainSence::testButtonTouched));
	testButton->setVisible(false);
	testButton->setTouchEnabled(false);

	// 道具相关的Sprite
	_swap1Sprite = Sprite::create(FileUtils::getInstance()->fullPathForFilename("swap_1.png"));
	_swap1Sprite->setVisible(false);
	_swap2Sprite = Sprite::create(FileUtils::getInstance()->fullPathForFilename("swap_2.png"));
	_swap2Sprite->setVisible(false);

	_jigsawLayout->addChild(_swap1Sprite,1);
	_jigsawLayout->addChild(_swap2Sprite,1);

	this->addChild(mainPanel);

	// 处理该界面的按键操作
	auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyReleased = CC_CALLBACK_2(MainSence::onKeyReleased, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	bool newbieGuide = UserDefault::getInstance()->getBoolForKey("NewbieGuide",true);
	if (!newbieGuide)
	{
		// 显示广告
		AdvertHelper::showAdvert();
	}
	else
	{
		// 显示新手引导
		NewbieGuideLayer* layer = new NewbieGuideLayer();
		layer->init();
		layer->autorelease();
		this->addChild(layer);

		UserDefault::getInstance()->setBoolForKey("NewbieGuide", false);
	}

	return true;
}

void MainSence::onExit()
{
	// 隐藏广告
	AdvertHelper::hideAdvert();

	Layer::onExit();
}

void MainSence::jigsawLayerTouched(Ref *pSender, TouchEventType type)
{
	auto jigsawLayer = static_cast<Layout*>(pSender);
    switch (type)
    {
        case TOUCH_EVENT_BEGAN:
			{
				if (_inSwapAnyMode||_isSwapLock)
				{
					break;
				}

				// 移动的开始
				auto point = jigsawLayer->convertToNodeSpaceAR(jigsawLayer->getTouchStartPos());
				int indexX = static_cast<int>(point.x/(_jigsawWidth+_gapWidth));
				int indexY = static_cast<int>((_photoHeight-point.y)/(_jigsawHeight+_gapWidth));
				CCLOG("Touch start! x[%f] y[%f]----Index Point:x[%d],y[%d]",point.x,point.y,indexX,indexY);

				Point touchedJigsawIndex = Point((float)indexX,(float)indexY);

				// 看看四周，空白块是不是在周围？
				_isValidSwap = false;
				auto rightJigsaw = getSpriteByIndexPoint(Point(touchedJigsawIndex.x + 1,touchedJigsawIndex.y));
				auto leftJigsaw = getSpriteByIndexPoint(Point(touchedJigsawIndex.x - 1,touchedJigsawIndex.y));
				auto upJigsaw = getSpriteByIndexPoint(Point(touchedJigsawIndex.x,touchedJigsawIndex.y - 1));
				auto downJigsaw = getSpriteByIndexPoint(Point(touchedJigsawIndex.x,touchedJigsawIndex.y + 1));

				if (!_isValidSwap && rightJigsaw && rightJigsaw->getTag() == BLANK_JIGSAW_TAG)
				{
					_isValidSwap = true;
					_blankPosition = BlankPosition::ON_YOUR_RIGHT;
					_swapToJigsaw = Point(touchedJigsawIndex.x + 1,touchedJigsawIndex.y);
					CCLOG("Blank is on my right!");
				}
				if (!_isValidSwap && leftJigsaw && leftJigsaw->getTag() == BLANK_JIGSAW_TAG)
				{
					_isValidSwap = true;
					_blankPosition = BlankPosition::ON_YOUR_LEFT;
					_swapToJigsaw = Point(touchedJigsawIndex.x - 1,touchedJigsawIndex.y);
					CCLOG("Blank is on my left!");
				}
				if (!_isValidSwap && upJigsaw && upJigsaw->getTag() == BLANK_JIGSAW_TAG)
				{
					_isValidSwap = true;
					_blankPosition = BlankPosition::ON_YOUR_UP;
					_swapToJigsaw = Point(touchedJigsawIndex.x,touchedJigsawIndex.y - 1);
					CCLOG("Blank is on my up!");
				}
				if (!_isValidSwap && downJigsaw && downJigsaw->getTag() == BLANK_JIGSAW_TAG)
				{
					_isValidSwap = true;
					_blankPosition = BlankPosition::ON_YOUR_DOWN;
					_swapToJigsaw = Point(touchedJigsawIndex.x,touchedJigsawIndex.y + 1);
					CCLOG("Blank is on my down!");
				}

				// 如果周围有空白块，那么就做些记录，准备好移动吧
				if (_isValidSwap)
				{
					_fromSprite = getSpriteByIndexPoint(touchedJigsawIndex);
					_fromOriginalPosition = _fromSprite->getPosition();
					_swapFromJigsaw = touchedJigsawIndex;
					// 移动开始音效
					bool lastTimeEffect = UserDefault::getInstance()->getBoolForKey("SetupEffect",true);
					if (lastTimeEffect)
					{
						SimpleAudioEngine::getInstance()->playEffect(FileUtils::getInstance()->fullPathForFilename(MOVE_START_EFFECT_FILE).c_str());
					}
				}
			}
            break;
            
        case TOUCH_EVENT_MOVED:
			{
				if (_inSwapAnyMode||_isSwapLock)
				{
					break;
				}

				if (_isValidSwap)
				{
					auto startPoint = jigsawLayer->convertToNodeSpaceAR(jigsawLayer->getTouchStartPos());
					auto movePoint = jigsawLayer->convertToNodeSpaceAR(jigsawLayer->getTouchMovePos());
					auto offsetPoint = movePoint - startPoint;
					CCLOG("Touch move! x[%f] y[%f]",movePoint.x,movePoint.y);

					Point newPosition;
					// 移动时，根据空白块的位置和偏移量，计算出拼图的新位置
					if (_blankPosition == BlankPosition::ON_YOUR_RIGHT)
					{
						newPosition.x = _fromOriginalPosition.x + _getSectionValue(0,_jigsawWidth+_gapWidth,offsetPoint.x);
						newPosition.y = _fromOriginalPosition.y;
					}
					else if (_blankPosition == BlankPosition::ON_YOUR_LEFT)
					{
						newPosition.x = _fromOriginalPosition.x + _getSectionValue(-_jigsawWidth-_gapWidth,0,offsetPoint.x);
						newPosition.y = _fromOriginalPosition.y;
					}
					else if (_blankPosition == BlankPosition::ON_YOUR_UP)
					{
						newPosition.x = _fromOriginalPosition.x;
						newPosition.y = _fromOriginalPosition.y + _getSectionValue(0,_jigsawHeight+_gapWidth,offsetPoint.y);
					}
					else if (_blankPosition == BlankPosition::ON_YOUR_DOWN)
					{
						newPosition.x = _fromOriginalPosition.x;
						newPosition.y = _fromOriginalPosition.y + _getSectionValue(-_jigsawHeight-_gapWidth,0,offsetPoint.y);
					}
					else
					{
						// What?
					}

					_fromSprite->setPosition(newPosition);
				}
			}
            break;
            
        case TOUCH_EVENT_ENDED:
			{
				if (_isSwapLock)
				{
					break;
				}

				auto startPoint = jigsawLayer->convertToNodeSpaceAR(jigsawLayer->getTouchStartPos());
				auto endPoint = jigsawLayer->convertToNodeSpaceAR(jigsawLayer->getTouchEndPos());
				int indexX = static_cast<int>(endPoint.x/(_jigsawWidth+_gapWidth));
				int indexY = static_cast<int>((_photoHeight-endPoint.y)/(_jigsawHeight+_gapWidth));
				auto offsetPoint = endPoint - startPoint;
				CCLOG("Touch end! x[%f] y[%f]----Index Point:x[%d],y[%d]",endPoint.x,endPoint.y,indexX,indexY);

				Point touchedJigsawIndex = Point((float)indexX,(float)indexY);

				if (_inSwapAnyMode)
				{
					if (!_swap1Touched)
					{
						_swap1Touched = true;

						_swapFromJigsaw = touchedJigsawIndex;

						// 找到碎片中点
						auto fromPosition = getSpriteByIndexPoint(_swapFromJigsaw)->getPosition();
						auto midPoint = Point(fromPosition.x + _jigsawWidth/2 , fromPosition.y + _jigsawHeight/2);
						_swap1Sprite->setPosition(midPoint);
						_swap1Sprite->setVisible(true);
					}
					else
					{
						_swapToJigsaw = touchedJigsawIndex;

						// 找到碎片中点
						auto toPosition = getSpriteByIndexPoint(_swapToJigsaw)->getPosition();
						auto midPoint = Point(toPosition.x + _jigsawWidth/2 , toPosition.y + _jigsawHeight/2);
						_swap2Sprite->setPosition(midPoint);
						_swap2Sprite->setVisible(true);

						// 真正的交换
						auto fromOriginalPosition = getSpriteByIndexPoint(_swapFromJigsaw)->getPosition();
						swap(_swapFromJigsaw,fromOriginalPosition,_swapToJigsaw,false);

						_inSwapAnyMode = false;
						_itemButton->setBright(true);

						// 把2个标志也交换了
						auto fromToToAction = MoveTo::create(1.f,_swap2Sprite->getPosition());
						auto fromToToSeq = Sequence::create(fromToToAction
							,CallFunc::create(CC_CALLBACK_0(MainSence::setSwapAnyInvalid,this))
							,NULL);
						_swap1Sprite->runAction(fromToToSeq);

						auto toToFromAction = MoveTo::create(1.f,_swap1Sprite->getPosition());
						_swap2Sprite->runAction(toToFromAction);
					}

					break;
				}

				if (_isValidSwap)
				{
					// 移动结束时,判断碎片的偏移是否过了一半
					bool overHalf = false;
					float offset;
					if (_blankPosition == BlankPosition::ON_YOUR_RIGHT)
					{
						offset = offsetPoint.x;
						if (offset > (_jigsawWidth+_gapWidth)/2)
						{
							overHalf = true;
						}
						else
						{
							overHalf = false;
						}
					}
					else if (_blankPosition == BlankPosition::ON_YOUR_LEFT)
					{
						offset = -offsetPoint.x;
						if (offset > (_jigsawWidth+_gapWidth)/2)
						{
							overHalf = true;
						}
						else
						{
							overHalf = false;
						}
					}
					else if (_blankPosition == BlankPosition::ON_YOUR_UP)
					{
						offset = offsetPoint.y;
						if (offset > (_jigsawHeight+_gapWidth)/2)
						{
							overHalf = true;
						}
						else
						{
							overHalf = false;
						}
					}
					else if (_blankPosition == BlankPosition::ON_YOUR_DOWN)
					{
						offset = -offsetPoint.y;
						if (offset > (_jigsawHeight+_gapWidth)/2)
						{
							overHalf = true;
						}
						else
						{
							overHalf = false;
						}
					}
					else
					{
						// What?
					}

					
					// 如果偏移没过一半就恢复原位
					if (!overHalf)
					{
						_fromSprite->setPosition(_fromOriginalPosition);
					}
					// 如果偏移过了一半就真正的交换
					else
					{
						swap(_swapFromJigsaw,_fromOriginalPosition,_swapToJigsaw);
					}

					// 移动结束音效
					bool lastTimeEffect = UserDefault::getInstance()->getBoolForKey("SetupEffect",true);
					if (lastTimeEffect)
					{
						SimpleAudioEngine::getInstance()->playEffect(FileUtils::getInstance()->fullPathForFilename(MOVE_END_EFFECT_FILE).c_str());
					}

					_isValidSwap = false;
				}
			}
            break;
            
        case TOUCH_EVENT_CANCELED:
			{
				CCLOG("Touch canceled!");

				_isValidSwap = false;
				_fromSprite->setPosition(_fromOriginalPosition);
			}
            break;
            
        default:
            break;
    }
}

void MainSence::initJigsaw(const std::string& jigsawFileName)
{
	_jigsawFileName = jigsawFileName;
	Texture2D *jigsawTexture = Director::getInstance()->getTextureCache()->addImage(FileUtils::getInstance()->fullPathForFilename(jigsawFileName));
	// 初始化一些参数
	_photoWidth = jigsawTexture->getPixelsWide();
	_photoHeight = jigsawTexture->getPixelsHigh();
	CCASSERT(_photoHeight*_piecesInWidth == _piecesInHeight*_photoWidth , "Aspect ratio must be 4:3");
	_jigsawWidth = _photoWidth/_piecesInWidth;
	_jigsawHeight = _photoHeight/_piecesInHeight;
	// TODO 应该根据照片大小自由调节
	_gapWidth = 2;

	// 存放原图
	auto originalPhoto = Sprite::createWithTexture(jigsawTexture);
	originalPhoto->setAnchorPoint(Point(0,0));
	originalPhoto->setPosition(Point(0,0));
	Size winSize = Director::getInstance()->getWinSize();
	float scale = winSize.width/_photoWidth;
	_originalLayout->addChild(originalPhoto);
	_originalLayout->setScale(scale);

	// 照片总大小应该为照片大小加上缝隙大小
	Size realSize = Size(_photoWidth+(_piecesInWidth-1)*_gapWidth,_photoHeight+(_piecesInHeight-1)*_gapWidth);
	// 根据参数调整layout大小
	_jigsawLayout->setSize(realSize);
	_jigsawLayout->setScale(winSize.width/realSize.width);

	// 存放的是       碎片当前索引和位置Pos
	std::vector<std::pair<Point,Point>> currentPointVec;

	// 存放的是       Sprite和正确的索引
	std::vector<std::pair<Sprite*,Point>> realSpriteVec;

	for (int x = 0 ; x < _piecesInWidth ; ++x)
	{
		for (int y = 0 ; y < _piecesInHeight ; ++y)
		{
			auto originalPhotoFragment = Sprite::createWithTexture(jigsawTexture,Rect((_jigsawWidth+_gapWidth)*x,(_jigsawHeight+_gapWidth)*y,_jigsawWidth,_jigsawHeight));
			originalPhotoFragment->setAnchorPoint(Point(0,0));

			Point location = Point((_jigsawWidth+_gapWidth)*x,realSize.height-(_jigsawHeight+_gapWidth)*(y+1));
			Point index = Point(float(x),float(y));

			// 最后一块设置为透明色，并打上特别的标记
			if (x == _piecesInWidth - 1 && y == _piecesInHeight - 1)
			{
				originalPhotoFragment->setOpacity(0);
				originalPhotoFragment->setTag(BLANK_JIGSAW_TAG);

				// 最后一块由于永远在最后，直接放进容器就行
				originalPhotoFragment->setPosition(location);
				_jigsawMap.insert(std::make_pair(index,std::make_pair(originalPhotoFragment,index)));

				_jigsawLayout->addChild(originalPhotoFragment,-1);
			}
			else
			{
				originalPhotoFragment->setOpacity(100);
				originalPhotoFragment->setTag(NOMARL_JIGSAW_TAG);
				
				_jigsawLayout->addChild(originalPhotoFragment);

				// 放到临时的vector中，循环结束后在打乱一次
				currentPointVec.push_back(std::make_pair(index,location));
				realSpriteVec.push_back(std::make_pair(originalPhotoFragment,index));
			}
		}
	}

	// 打乱碎片
	srand((unsigned)time(0));
	size_t randSize = currentPointVec.size();
	for(size_t i = 0 ; i < realSpriteVec.size() ; ++i)
	{
		int randPos = rand()%randSize;
		auto newPosition = currentPointVec[randPos];
		auto JigsawSprite = realSpriteVec[i].first;
		JigsawSprite->setPosition(newPosition.second);
		_jigsawMap.insert(std::make_pair(newPosition.first,realSpriteVec[i]));

		CCLOG("Current Index Point:x[%f],y[%f]----Correct Index Point:x[%f],y[%f]"
			,newPosition.first.x,newPosition.first.y
			,realSpriteVec[i].second.x,realSpriteVec[i].second.y);

		// 把currentPointVec中的最后一个与被随机出的位置交换
		// 之后把currentPointVec的大小减一
		currentPointVec[randPos] = currentPointVec[--randSize];
	}

	// 设置初始的拼图透明度
	setAllJigsawOpacity();

	// 看看是不是一上来就胜利了（运气也太好了吧）
	checkVictory();
}

cocos2d::Sprite* MainSence::getSpriteByIndexPoint(cocos2d::Point index)
{
	auto iter = _jigsawMap.find(index);
	if (iter == _jigsawMap.end())
	{
		return nullptr;
	}
	else
	{
		return iter->second.first;
	}
}

void MainSence::swap(cocos2d::Point from,cocos2d::Point fromOriginalPosition,cocos2d::Point to,bool instant)
{
	CCLOG("Swap from:x[%f] y[%f] to:x[%f],y[%f]",from.x,from.y,to.x,to.y);

	auto fromIter = _jigsawMap.find(from);
	auto fromPosition = fromIter->first;
	auto fromSprite = fromIter->second;

	auto toIter = _jigsawMap.find(to);
	auto toPosition = toIter->first;
	auto toSprite = toIter->second;

	if (from == to)
	{
		CCLOG("Error:swap to self!");
		return;
	}

	swapBegin();

	// 将图片交换
	if (instant)
	{
		fromSprite.first->setPosition(toSprite.first->getPosition());
		toSprite.first->setPosition(fromOriginalPosition);

		swapEnd();
		swapEnd();
	}
	else
	{
		auto fromToToAction = MoveTo::create(1.f,toSprite.first->getPosition());
		auto fromToToSeq = Sequence::create(fromToToAction
			,CallFunc::create(CC_CALLBACK_0(MainSence::swapEnd,this))
			,NULL);
		fromSprite.first->runAction(fromToToSeq);

		auto toToFromAction = MoveTo::create(1.f,fromOriginalPosition);
		auto toToFromSeq = Sequence::create(toToFromAction
			,CallFunc::create(CC_CALLBACK_0(MainSence::swapEnd,this))
			,NULL);
		toSprite.first->runAction(toToFromSeq);

		++_useItem;
	}
}

void MainSence::swapBegin()
{
	_swapEndCount = 0;
	_isSwapLock = true;
}

void MainSence::swapEnd()
{
	if (++_swapEndCount != 2)
	{
		return;
	}

	_isSwapLock = false;

	auto fromIter = _jigsawMap.find(_swapFromJigsaw);
	auto fromPosition = fromIter->first;
	auto fromSprite = fromIter->second;

	auto toIter = _jigsawMap.find(_swapToJigsaw);
	auto toPosition = toIter->first;
	auto toSprite = toIter->second;

	// 更新map
	_jigsawMap.erase(fromIter);
	_jigsawMap.erase(toIter);
	_jigsawMap.insert(std::make_pair(fromPosition,toSprite));
	_jigsawMap.insert(std::make_pair(toPosition,fromSprite));

	// 记步
	++_moveStep;

	setAllJigsawOpacity();

	checkVictory();
}

void MainSence::checkVictory()
{
	bool allCorrect = true;
	for(auto& jigsaw : _jigsawMap)
	{
		// 只要有一个不在正确位置上，那就不对
		if (jigsaw.first != jigsaw.second.second)
		{
			allCorrect = false;
			break;
		}
	}

	if (allCorrect)
	{
		_winTime = time(0);
		CCLOG("You Win!Step[%d],Time[%lu]",_moveStep,_winTime-_startTime);

		auto newSence = TransitionSlideInR::create(1.f,VictorySence::scene(_jigsawFileName,_winTime-_startTime,_moveStep,_useItem));
		if (newSence)
		{
			Director::getInstance()->replaceScene(newSence);
		}
	}
}

void MainSence::setSwapAnyInvalid()
{
	_swap1Touched = false;
	_swap1Sprite->setVisible(false);
	_swap2Sprite->setVisible(false);

	if (_useItem == 0)
	{
		_itemButton->loadTextures(FileUtils::getInstance()->fullPathForFilename("ui/button/item_5_normal.png")
			,FileUtils::getInstance()->fullPathForFilename("ui/button/item_5_normal.png")
			,FileUtils::getInstance()->fullPathForFilename("ui/button/item_5_pressed.png")
			);
	}
	else if (_useItem == 1)
	{
		_itemButton->loadTextures(FileUtils::getInstance()->fullPathForFilename("item_4_normal.png")
			,FileUtils::getInstance()->fullPathForFilename("item_4_normal.png")
			,FileUtils::getInstance()->fullPathForFilename("item_4_pressed.png")
			);
	}
	else if (_useItem == 2)
	{
		_itemButton->loadTextures(FileUtils::getInstance()->fullPathForFilename("item_3_normal.png")
			,FileUtils::getInstance()->fullPathForFilename("item_3_normal.png")
			,FileUtils::getInstance()->fullPathForFilename("item_3_pressed.png")
			);
	}
	else if (_useItem == 3)
	{
		_itemButton->loadTextures(FileUtils::getInstance()->fullPathForFilename("item_2_normal.png")
			,FileUtils::getInstance()->fullPathForFilename("item_2_normal.png")
			,FileUtils::getInstance()->fullPathForFilename("item_2_pressed.png")
			);
	}
	else if (_useItem == 4)
	{
		_itemButton->loadTextures(FileUtils::getInstance()->fullPathForFilename("item_1_normal.png")
			,FileUtils::getInstance()->fullPathForFilename("item_1_normal.png")
			,FileUtils::getInstance()->fullPathForFilename("item_1_pressed.png")
			);
	}
	else if (_useItem == 5)
	{
		_itemButton->loadTextures(FileUtils::getInstance()->fullPathForFilename("item_0_normal.png")
			,FileUtils::getInstance()->fullPathForFilename("item_0_normal.png")
			,FileUtils::getInstance()->fullPathForFilename("item_0_normal.png")
			);
		_itemButton->setTouchEnabled(false);
	}
	else
	{

	}
}

void MainSence::setAllJigsawOpacity()
{
	for(auto& jigsaw : _jigsawMap)
	{
		if (jigsaw.first != jigsaw.second.second)
		{
			jigsaw.second.first->setOpacity(150);
		}
		else
		{
			jigsaw.second.first->setOpacity(255);
		}
	}
}

void MainSence::backToUpperUI()
{
	auto newSence = TransitionSlideInL::create(1.f,PhotoSelectSence::scene());
	if (newSence)
	{
		Director::getInstance()->replaceScene(newSence);
	}
}

void MainSence::backButtonTouched(Ref *pSender, TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		AreYouSureLayer* layer = new AreYouSureLayer();
		layer->initWithFunction(CC_CALLBACK_0(MainSence::backToUpperUI,this));
		layer->autorelease();
		this->addChild(layer);
	}
}

void MainSence::testButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		_winTime = time(0);
		auto newSence = TransitionSlideInR::create(1.f,VictorySence::scene(_jigsawFileName,_winTime-_startTime,_moveStep,_useItem));
		if (newSence)
		{
			Director::getInstance()->replaceScene(newSence);
		}
	}
}

void MainSence::itemButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		if (_inSwapAnyMode)
		{
			_inSwapAnyMode = false;
			_itemButton->setBright(true);
		}
		else
		{
			_inSwapAnyMode = true;
			_itemButton->setBright(false);
		}

		setSwapAnyInvalid();
	}
}

void MainSence::originalButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type)
{
	if (type == TOUCH_EVENT_ENDED)
	{
		if (_containerLayout->getNumberOfRunningActions() == 0)
		{
			auto moveLeftAction = MoveTo::create(0.2f,Point(-640,0));
			auto moveRightAction = MoveTo::create(0.2f,Point(0,0));
			auto delay3SecAction = DelayTime::create(3.f);
			auto orignialSeq = Sequence::create(moveLeftAction,delay3SecAction,moveRightAction,NULL);

			_containerLayout->runAction(orignialSeq);
		}
	}
}

void MainSence::onKeyReleased(EventKeyboard::KeyCode key, Event* event)
{
	if (EventKeyboard::KeyCode::KEY_RETURN == key
		|| EventKeyboard::KeyCode::KEY_ESCAPE == key
		|| EventKeyboard::KeyCode::KEY_BACKSPACE == key)
    {
		AreYouSureLayer* layer = new AreYouSureLayer();
		layer->initWithFunction(CC_CALLBACK_0(MainSence::backToUpperUI,this));
		layer->autorelease();
		this->addChild(layer);
    }
}

