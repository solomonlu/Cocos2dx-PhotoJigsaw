package org.cocos2dx.cpp;

import android.os.Handler;
import android.os.Message;

public class ToastHelper 
{
	private static Handler mHandler;
	
	public static void init(Handler handler)  
    {  
		ToastHelper.mHandler = handler;
    }
	
	@SuppressWarnings("unused")
	private static void showToast()  
    {  
        Message msg = mHandler.obtainMessage();
        msg.what = AppActivity.SHOW_TOAST;
        msg.sendToTarget();
    }
}
