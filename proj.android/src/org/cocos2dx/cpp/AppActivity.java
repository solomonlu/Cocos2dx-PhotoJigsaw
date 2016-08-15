/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.cpp;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.Gravity;
import android.view.Menu;
import android.view.ViewGroup.LayoutParams;
import android.widget.FrameLayout;
import android.widget.Toast;

//import com.qq.e.ads.AdRequest;
//import com.qq.e.ads.AdSize;
//import com.qq.e.ads.AdView;
import com.solomon.photo_jigsaw.R;

public class AppActivity extends Cocos2dxActivity {
	
	// PageView green problem
	public Cocos2dxGLSurfaceView onCreateView() { 
        Cocos2dxGLSurfaceView glSurfaceView = new Cocos2dxGLSurfaceView(this); 
        glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8); 
 
        return glSurfaceView; 
    } 
	
	public static final int SHOW_AD = 0x0001;
	public static final int HIDE_AD = 0x0002;
	public static final int SHOW_TOAST = 0x0003;
	//private AdView bannerAD;
	private FrameLayout bannerContainer;
	@SuppressLint("HandlerLeak")
	private Handler mHandler = new Handler()
	{
		@Override
		public void handleMessage(Message msg)
		{
			switch(msg.what)
			{
				case SHOW_AD:
					showAdvertImpl();
					break;
				case HIDE_AD:
					hideAdvertImpl();
					break;
				case SHOW_TOAST:
					showToastImpl();
					break;
				default:
					break;
			}
		}
	};
	
	@Override
	protected void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);
		AdvertHelper.init(mHandler);
		ToastHelper.init(mHandler);
		
		//this.bannerAD = new AdView(this, AdSize.BANNER, Constants.APPId, Constants.BannerPosId);
		//AdRequest adr = new AdRequest();
		//adr.setTestAd(true);
		//bannerAD.fetchAd(adr);
	}
	
	private void showAdvertImpl()
	{
		bannerContainer = new FrameLayout(this);
		FrameLayout.LayoutParams param = new FrameLayout.LayoutParams(LayoutParams.MATCH_PARENT,LayoutParams.WRAP_CONTENT);
		param.gravity = Gravity.BOTTOM|Gravity.CENTER_HORIZONTAL;
		addContentView(bannerContainer,param);
		
		this.bannerContainer.removeAllViews();
		//this.bannerContainer.addView(bannerAD);
	}
	
	private void hideAdvertImpl()
	{
		this.bannerContainer.removeAllViews();
	}
	
	private void showToastImpl()
	{
		Toast.makeText(this, R.string.toast_text, Toast.LENGTH_LONG).show();
	}
	
	@Override
	protected void onResume() {
		super.onResume();
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		return true;
	}
}
