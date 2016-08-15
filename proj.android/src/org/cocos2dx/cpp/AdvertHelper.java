package org.cocos2dx.cpp;

import android.os.Handler;
import android.os.Message;

public class AdvertHelper
{
	private static Handler mHandler;
	
	public static void init(Handler handler)  
    {  
		AdvertHelper.mHandler = handler;
    }
	
	@SuppressWarnings("unused")
	private static void showAdvert()  
    {  
        Message msg = mHandler.obtainMessage();
        msg.what = AppActivity.SHOW_AD;
        msg.sendToTarget();
    }
	
	@SuppressWarnings("unused")
	private static void hideAdvert()  
    {  
        Message msg = mHandler.obtainMessage();
        msg.what = AppActivity.HIDE_AD;
        msg.sendToTarget();
    }
}
