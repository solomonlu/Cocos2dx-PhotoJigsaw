#ifndef __FRAME_SLOGANSCENE_H__
#define __FRAME_SLOGANSCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"

class SloganSence : public cocos2d::Layer
{
public:
    static cocos2d::Scene* scene();
	CREATE_FUNC(SloganSence);

	virtual bool init() override;
	void loadingCallBack(cocos2d::Texture2D *texture);
private:
	int _numberOfSprites;
    int _numberOfLoadedSprites;
};

#endif // __FRAME_SLOGANSCENE_H__