package de.tuchemnitz.remoteclient;

import java.util.Timer;
import java.util.TimerTask;

import android.app.AlertDialog;
import android.content.Intent;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;

import com.actionbarsherlock.app.SherlockActivity;
import com.actionbarsherlock.view.Menu;
import com.actionbarsherlock.view.MenuItem;

public class MainActivity extends SherlockActivity {

	private Timer BattTimer = null;
	private final int EVENT_BATT = 0;
	private final int EVENT_STATE = 1;
	private final int EVENT_CONN = 2;
	
	private MenuItem BatteryIcon;
	private MenuItem ConnectIcon;
	
	
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
			    	
			    	Callbacksplit.setActBarConnectIcon();
		    	}
		    	break;
		    case EVENT_STATE:
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
			    BatteryIcon.setIcon(batt_icon_r);
			    Callbacksplit.saveBatteryStateIcon(batt_icon_r);
			    Callbacksplit.setActBarBatteryIcon(batt_icon_r);
			    
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
        //NetworkModule.SetIPAddress("192.168.5.20");
        menu_button4_event(null);
        
        NetworkModule.RegisterCallback(null,		-1,				0);
		NetworkModule.RegisterCallback(EvtHandler,	EVENT_CONN,		NetworkModule.INFO_CONN);
		NetworkModule.RegisterCallback(EvtHandler,	EVENT_STATE,	NetworkModule.INFO_STATE);
        NetworkModule.RegisterCallback(EvtHandler,	EVENT_BATT,		NetworkModule.INFO_BATT);
        
        Log.v("MainAct", "Activity started.");
        BattTimer = new Timer();
		//BattTimer.schedule(new BattTmrTask(EvtHandler), 1000, 10000);
        BattTimer.schedule(new BattTmrTask(), 1000, 10000);
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
    public void onDestroy(){
    	
    	BattTimer.cancel();
    	NetworkModule.Rest();
    	NetworkModule.CloseConnection();
    	super.onDestroy();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        //getMenuInflater().inflate(R.menu.activity_main, menu);
    	super.onCreateOptionsMenu(menu);
        getSupportMenuInflater().inflate(R.menu.actionbar, menu);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        ConnectIcon = (MenuItem)menu.findItem(R.id.acb_connect);
        BatteryIcon = (MenuItem)menu.findItem(R.id.acb_battery);
        setActBarConnectIcon();
        
        ((MenuItem)menu.findItem(R.id.acb_m_1)).setVisible(false);
        
        //getSupportActionBar().setNavigationMode(ActionBar.NAVIGATION_MODE_LIST);
    	//setContentView(R.menu.actionbar);
//        menu.add("Save")
//        .setIcon(R.drawable.bat100)
//        .setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);
        
        return true;
    }
    
    @Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		super.onOptionsItemSelected(item);
		Intent intent;
		switch(item.getItemId()){
		case android.R.id.home:
			finish();
			break;
		case R.id.acb_m_1:
			break;
		case R.id.acb_m_2:
			intent = new Intent(this, BewegungActivity.class);
			startActivity(intent);
			break;
		case R.id.acb_m_3:
			intent = new Intent(this, SprachausgabeActivity.class);
			startActivity(intent);
			break;
		case R.id.acb_m_4:
			intent = new Intent(this, SpecialsActivity.class);
			startActivity(intent);
			break;
		case R.id.acb_m_5:
			intent = new Intent(this, ConfigActivity.class);
			startActivity(intent);
			break;
		}
		
		return true;
	}
    
    public void setActBarConnectIcon(){
    	if(NetworkModule.IsConnected()==0)
    	{
    		ConnectIcon.setIcon(R.drawable.network_disconnected);
    	}
    	else
    	{
    		ConnectIcon.setIcon(R.drawable.network_connected);
    	}
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
    
	class BattTmrTask extends TimerTask
	{
		public void run()  
		{
			NetworkModule.RequestBatteryState();
		}
	}
}
