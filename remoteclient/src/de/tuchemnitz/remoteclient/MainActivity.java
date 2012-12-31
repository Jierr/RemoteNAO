package de.tuchemnitz.remoteclient;

import java.util.Timer;
import java.util.TimerTask;

import android.app.AlertDialog;
import android.content.Intent;
import android.content.res.Resources;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;

import com.actionbarsherlock.app.SherlockActivity;
import com.actionbarsherlock.view.Menu;
import com.actionbarsherlock.view.MenuItem;

public class MainActivity extends SherlockActivity {

	private Timer BattTimer = null;
	private final int EVENT_BATT = 0;
	private final int EVENT_SIT = 1;
	private final int EVENT_CONN = 2;
	
	private MenuItem BatteryIcon;
	
	private Handler EvtHandler = new Handler()
	{
		@Override public void dispatchMessage(Message msg)
		{
		    //super.dispatchMessage(msg);
		    
		    switch(msg.what)
		    {
		    case EVENT_CONN:
		    	Log.v("MainAct.Event", "Connection Status: " + String.valueOf(msg.arg1));
		    	if (msg.arg1 < 0)
		    	{
		    		Callbacksplit.showConnectionLostDialog();
		    	}
		    	else
		    	{
			    	if(Callbacksplit.getConfigActivity() != null)
			    		Callbacksplit.getConfigActivity().changeConnectionView(msg.arg1);
		    	}
		    	break;
		    case EVENT_SIT:
				final String SitStatus = (String)msg.obj;
		    	
				if(Callbacksplit.getSpecialsActivity() != null)
					Callbacksplit.getSpecialsActivity().changeSitButtonText(SitStatus);
		    	
		    	break;
		    case EVENT_BATT:
				final int batt_pic;
				Drawable batt_icon_r;
				
			    if (msg.arg1 > 90)
			    	batt_pic = R.drawable.bat100;
			    else if (msg.arg1 <= 90 && msg.arg1 > 65)
			    	batt_pic = R.drawable.bat75;
			    else if (msg.arg1 <= 65 && msg.arg1 > 35)
			    	batt_pic = R.drawable.bat50;
			    else if (msg.arg1 <= 35 && msg.arg1 > 10)
			    	batt_pic = R.drawable.bat25;
			    else
			    	batt_pic = R.drawable.bat0;
			    batt_icon_r = getResources().getDrawable(batt_pic);
			    Callbacksplit.saveBatteryStateIcon(batt_icon_r);
			    BatteryIcon.setIcon(batt_icon_r);
			    if(Callbacksplit.getBewegungActivity()!= null)
				    Callbacksplit.getBewegungActivity().setActBarBatteryIcon(batt_icon_r);
			    if(Callbacksplit.getConfigActivity()!= null)
				    Callbacksplit.getConfigActivity().setActBarBatteryIcon(batt_icon_r);
			    if(Callbacksplit.getSpecialsActivity()!= null)
				    Callbacksplit.getSpecialsActivity().setActBarBatteryIcon(batt_icon_r);
			    if(Callbacksplit.getSprachausgabeActivity()!= null)
				    Callbacksplit.getSprachausgabeActivity().setActBarBatteryIcon(batt_icon_r);
			    
			    break;
		    }
		    
		    return;
		}
	};
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        Callbacksplit.registerMainActivity(this);
        Callbacksplit.saveBatteryStateIcon(null);
        
    	//NetworkModule.SetIPAddress("134.109.97.52");
        //NetworkModule.SetIPAddress("134.109.151.142");
        NetworkModule.SetIPAddress("192.168.5.20");
        menu_button4_event(null);
        
        NetworkModule.RegisterCallback(null,		-1,			0);
		NetworkModule.RegisterCallback(EvtHandler,	EVENT_CONN,	NetworkModule.INFO_CONN);
		NetworkModule.RegisterCallback(EvtHandler,	EVENT_SIT,	NetworkModule.INFO_SIT);
        NetworkModule.RegisterCallback(EvtHandler,	EVENT_BATT,	NetworkModule.INFO_BATT);
        
        Log.v("MainAct", "Activity started.");
        BattTimer = new Timer();
		BattTimer.schedule(new BattTmrTask(EvtHandler), 1000, 10000);
    }
    
	@Override
	protected void onResume(){
		super.onResume();
		Callbacksplit.setActiveActivity(this);
	}
	@Override
	protected void onPause(){
		super.onPause();
		Callbacksplit.unsetActiveActivity();
	}

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        //getMenuInflater().inflate(R.menu.activity_main, menu);
    	super.onCreateOptionsMenu(menu);
        getSupportMenuInflater().inflate(R.menu.actionbar, menu);
        BatteryIcon = (MenuItem)menu.findItem(R.id.acb_battery);
    	//setContentView(R.menu.actionbar);
//        menu.add("Save")
//        .setIcon(R.drawable.bat100)
//        .setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);
        return true;
    }
    
    @Override
    public void onDestroy(){
    	
    	BattTimer.cancel();
    	NetworkModule.Rest();
    	NetworkModule.CloseConnection();
    	super.onDestroy();
    }
    
    /** Called when the user clicks the Send button */
    public void sendMessage(View view) {
        // Do something in response to button
    	/*boolean RetVal;
    	
    	RetVal = NetworkModule.OpenConnection();
    	if (! RetVal)
    	{
	    	new AlertDialog.Builder(this)
			.setMessage("Connection failed")
			.setNeutralButton("och", null)
			.show();
    	}*/
    	
    	NetworkModule.net_test();
    	//NetworkModule.CloseConnection();
    	/*new AlertDialog.Builder(this)
			.setMessage("Test done.")
			.setNeutralButton("och", null)
			.show();*/
    	
    	return;
    }
    
    
    
    /********************** Menu *************************
     *****************************************************/
    public void menu_button1_event(View view) {
    	final Intent bewintent = new Intent (this,BewegungActivity.class);
    	//Log.v("main button1", "bewIntent: " + bewintent.toString());
    	startActivity(bewintent);
    }
    
    
    public void menu_button2_event(View view) {
    	final Intent spkintent = new Intent (this,SprachausgabeActivity.class);
    	startActivity(spkintent);
    }
    
    public void menu_button3_event(View view) {
    	final Intent specialsintent = new Intent (this,SpecialsActivity.class);
    	startActivity(specialsintent);
    }
    
    public void menu_button4_event(View view) {
    	final Intent cfgintent = new Intent (this,ConfigActivity.class);
    	startActivity(cfgintent);
    }
    
    public void menu_button5_event(View view) {
    	new AlertDialog.Builder(this)
		.setMessage("Das ist eine App um den NAO zu steuern")
		.setNeutralButton("Wer weiss?", null)
		.show();
    }
    
    public void menu_button6_event(View view) {
    	//jetzt in onDestroy
    	//BattTimer.cancel();
    	//NetworkModule.Rest();
    	//NetworkModule.CloseConnection();
    	MainActivity.this.finish();
    }
    
	class BattTmrTask extends TimerTask
	{
		private Handler updateUI/* = new Handler(){
			@Override
			public void dispatchMessage(Message msg) {
			    super.dispatchMessage(msg);
			    if(msg.what > 90)
			    {
			    	battery_view.setImageDrawable(getResources().getDrawable(R.drawable.bat100));
			    }
			    else if (msg.what <= 90 && msg.what > 65) {
			    	battery_view.setImageDrawable(getResources().getDrawable(R.drawable.bat75));
				}
			    else if (msg.what <= 65 && msg.what > 35) {
			    	battery_view.setImageDrawable(getResources().getDrawable(R.drawable.bat50));
				}
			    else if (msg.what <= 35 && msg.what > 10) {
			    	battery_view.setImageDrawable(getResources().getDrawable(R.drawable.bat25));
				}
			    else
			    {
			    	battery_view.setImageDrawable(getResources().getDrawable(R.drawable.bat0));
			    }
			}
		}*/;
		
		BattTmrTask(Handler EvtHandler)
		{
			updateUI = EvtHandler;
			
			return;
		}
		
		public void run()  
		{
			/*if (NetworkModule.IsConnected() != 1)
				return;
			
			int BattState;
			
			//System.out.println("Make my day.");
			//NetworkModule.Speak(MainActivity.NOTIFICATION_SERVICE);
			BattState = NetworkModule.GetBatteryState();
			updateUI.sendEmptyMessage(BattState);*/
			
			NetworkModule.RequestBatteryState();
		}
	}
}
