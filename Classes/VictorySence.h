#ifndef __FRAME_VICTORYSCENE_H__
#define __FRAME_VICTORYSCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

class VictorySence : public cocos2d::Layer
{
public:
    static cocos2d::Scene* scene(const std::string& jigsawFileName,int useTime,int useStep,int useItem);
	CREATE_FUNC(VictorySence);
	
	virtual bool init() override;

	void playAgainButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
	void illustrationButtonTouched(Ref *pSender, cocos2d::ui::TouchEventType type);
private:
	enum class EvaluationClass
    {
		EVALUATION_CLASS_S = 0,		// S级
		EVALUATION_CLASS_A,			// A级
		EVALUATION_CLASS_B,			// B级
		EVALUATION_CLASS_C,			// C级
    };

	void setScore(const std::string& jigsawFileName,int useTime,int useStep,int useItem);
	void numberRoll(float dt);
	EvaluationClass getEvaluationClass();
	bool getFragmentByEvaluation(EvaluationClass evaluationClass);
	void setImageAndButtonVisible();

	// 成绩相关
	std::string _jigsawFileName;
	int _useTime;
	int _useStep;
	int _useItem;
	bool _getFragment;

	// 滚动相关
	int _totalRollCount;
	int _currentRollCount;

	cocos2d::Sprite* _evaluationClassSprite;
	cocos2d::ui::TextBMFont* _useTimeText;
	cocos2d::ui::TextBMFont* _useStepText;
	cocos2d::ui::TextBMFont* _useItemText;
	cocos2d::ui::Layout* _evaluationClassPanel;
	cocos2d::ui::Layout* _fragmentDisplayPanel;
	cocos2d::ui::ImageView* _getFragmentImage;
	cocos2d::ui::ImageView* _noGetFragmentImage;
	cocos2d::ui::Button* _playAgainButton;
	cocos2d::ui::Button* _illustrationButton;
};

#endif // __FRAME_VICTORYSCENE_H__