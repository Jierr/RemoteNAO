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

/**
 * 
 * @file   MainActivity.java
 * @author Riko Streller 
 * 
 * This is the acivity the whole clientprogramm is depending on
 * All callback-functions and timers are running in this activity and the MainActivity
 * is the main spot to get to the other activities.
 *
 */
public class MainActivity extends SherlockActivity {

	private Timer BattTimer = null;
	private final int EVENT_BATT = 0;
	private final int EVENT_STATE = 1;
	private final int EVENT_CONN = 2;
	
	private MenuItem BatteryIcon;
	private MenuItem ConnectIcon;
	
	/**
	* @class EvtHandler
	*
	* A class to handle the events vom the callback-message-queue
	*/ 
	private Handler EvtHandler = new Handler()
	{
		/**
		 * Handles callback messages from NetworkModule and
		 * calls the Callbacksplit.
		 */
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
				Log.v("MainAct.Event", "State: " + SitStatus);
		    	
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
	
	/**
	 * Callback and timer setup.  
	 */
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
	
	/**
	 * Kills the timer and closes the connection.
	 */
	@Override
    public void onDestroy(){
    	super.onDestroy();
    	BattTimer.cancel();
    	NetworkModule.CloseConnection();
    }

	/**
	 * Draws the ActionBar.
	 */
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
    
    /**
     * Handles option menu clicks.
     * Shows other activities or closes the application.
     */
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
    
    /**
     * Refreshes the ActionBar's network state icon.
     */
    public void setActBarConnectIcon(){
    	if(NetworkModule.IsConnected()==NetworkModule.CONN_CLOSED)
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
    	
    	//NetworkModule.net_test();
    	//NetworkModule.CloseConnection();
    	/*new AlertDialog.Builder(this)
			.setMessage("Test done.")
			.setNeutralButton("och", null)
			.show();*/
    	
    	return;
    }
    
    
    
    /********************** Menu *************************
     *****************************************************/
    
    /**
	 * Opens and starts the BewegngActivity 
	 * 
	 * @param view		ignored, ID of the button, which trigger this function
	 */
    public void menu_button1_event(View view) {
    	final Intent bewintent = new Intent (this,BewegungActivity.class);
    	//Log.v("main button1", "bewIntent: " + bewintent.toString());
    	startActivity(bewintent);
    }
    
    /**
	 * Opens and starts the SprachausgabeActivity 
	 * 
	 * @param view		ignored, ID of the button, which trigger this function
	 */
    public void menu_button2_event(View view) {
    	final Intent spkintent = new Intent (this,SprachausgabeActivity.class);
    	startActivity(spkintent);
    }
    
    /**
	 * Opens and starts the SpecialsActivity 
	 * 
	 * @param view		ignored, ID of the button, which trigger this function
	 */
    public void menu_button3_event(View view) {
    	final Intent specialsintent = new Intent (this,SpecialsActivity.class);
    	startActivity(specialsintent);
    }
    
    /**
	 * Opens and starts the ConfigActivity 
	 * 
	 * @param view		ignored, ID of the button, which trigger this function
	 */
    public void menu_button4_event(View view) {
    	final Intent cfgintent = new Intent (this,ConfigActivity.class);
    	startActivity(cfgintent);
    }
    
    /**
	 * Only shows the about dialoge 
	 * 
	 * @param view		ignored, ID of the button, which trigger this function
	 */
    public void menu_button5_event(View view) {
    	new AlertDialog.Builder(this)
		.setMessage("Das ist eine App um den NAO zu steuern")
		.setNeutralButton("Wer weiss?", null)
		.show();
    }
    
    /**
    * @class BattTmrTask
    *
    * Provides functionality of the timer 
    */ 
	class BattTmrTask extends TimerTask
	{
		public void run()  
		{
			NetworkModule.RequestBatteryState();
		}
	}
}
