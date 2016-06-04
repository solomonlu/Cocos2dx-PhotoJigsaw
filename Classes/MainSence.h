#ifndef __FRAME_MAINSCENE_H__
#define __FRAME_MAINSCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

struct PointCmp
{
	bool operator()(const cocos2d::Point& _Left, const cocos2d::Point& _Right)
	{
		if (_Left.x == _Right.x)
		{
			return _Left.y < _Right.y;
		}
		else
		{
			return _Left.x < _Right.x;
		}
	}
};

class MainSence : public cocos2d::Layer
{
public:
    static cocos2d::Scene* scene(const std::string& jigsawFileName);
	CREATE_FUNC(MainSence);

	virtual bool init() override;
	virtual void onExit() override;

	void backButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void jigsawLayerTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void testButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void itemButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void originalButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event* event);
private:
	MainSence();
	// 初始化拼图，包括打乱顺序
	void initJigsaw(const std::string& jigsawFileName);

	// 根据索引位置获取碎片
	cocos2d::Sprite* getSpriteByIndexPoint(cocos2d::Point index);

	// 交换2个索引位置的碎片,swap函数中会调用swapBegin和swapEnd
	void swap(cocos2d::Point from,cocos2d::Point fromOriginalPosition,cocos2d::Point to,bool instant = true);
	void swapBegin();
	void swapEnd();

	// 设置1和2图标为不可用
	void setSwapAnyInvalid();

	// 检查所有拼图的透明度，如果位置正确，那么设置高透明度，否则设置低透明度
	void setAllJigsawOpacity();

	// 判断是否胜利
	void checkVictory();

	// 返回到上级界面
	void backToUpperUI();

	// 拼图属性相关
	std::string _jigsawFileName;			// 拼图的文件名
	cocos2d::ui::Layout* _jigsawLayout;		// 存放拼图的layout
	int _photoWidth;						// 照片宽度
	int _photoHeight;						// 照片高度
	int _piecesInWidth;						// 横向碎片数量
	int _piecesInHeight;					// 纵向碎片数量
	float _jigsawWidth;						// 拼图宽度
	float _jigsawHeight;					// 拼图高度
	int _gapWidth;							// 缝隙宽度

	//				 碎片当前的索引位置		  碎片Sprite	   碎片正确的索引位置
	typedef std::map<cocos2d::Point,std::pair<cocos2d::Sprite*,cocos2d::Point>,PointCmp> JigsawMap;
	JigsawMap _jigsawMap;

	// 移动与交换相关的类成员变量
	bool _isValidSwap;		// 只有在空白块周围的拼图移动时，才算有效的移动
	bool _isSwapLock;		// 交换是否锁定了，在交换动画播放时，会锁住
	int _swapEndCount;
	enum class BlankPosition
    {
		ON_YOUR_RIGHT = 0,			// 空白块在被点选的碎片的右边
        ON_YOUR_LEFT,				// 空白块在被点选的碎片的左边
		ON_YOUR_UP,					// 空白块在被点选的碎片的上边
		ON_YOUR_DOWN,				// 空白块在被点选的碎片的下边
    };
	BlankPosition _blankPosition;
	cocos2d::Sprite* _fromSprite;			// from的Sprite（只是为了Move时可以直接拿来用，不用再查了）
	cocos2d::Point _fromOriginalPosition;	// from的原始位置（Move时该位置会变，因此备份在这里）
	cocos2d::Point _swapFromJigsaw;	// 真正交换时from的索引位置
	cocos2d::Point _swapToJigsaw;	// 真正交换时to的索引位置

	// 任意交换模式
	cocos2d::ui::Button* _itemButton;		// 道具按钮
	cocos2d::Sprite* _swap1Sprite;
	cocos2d::Sprite* _swap2Sprite;
	bool _inSwapAnyMode;
	bool _swap1Touched;			// 任意交换模式下，第一块是否已经按下

	// 原图功能
	cocos2d::ui::Layout* _containerLayout;	// 拼图和原图的容器层
	cocos2d::ui::Layout* _originalLayout;	// 存放原图的layout
	cocos2d::ui::Button* _originalButton;		// 道具按钮

	// 成绩相关
	int _moveStep;
	unsigned long _startTime;
	unsigned long _winTime;
	int _useItem;
};

#endif // __FRAME_MAINSCENE_H__