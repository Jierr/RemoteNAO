package de.tuchemnitz.remoteclient;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Timer;
import java.util.TimerTask;

import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.Button;

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
	private final int EVENT_MAKRO = 3;
	
	private MenuItem BatteryIcon = null;
	private MenuItem ConnectIcon = null;
	
	private boolean askfor_Makros_once = true;
	
	public float param_MOV_F = 5; ///< Parameter for Movement, defines the width the robot walks forward 
	public float param_MOV_B = 2; ///< Parameter for Movement, defines the width the robot walks backward 
	public int param_MOV_D = 90; ///< Parameter for Movement (D for Drehung/Turning), the angle around the robot turn itself
	public int param_HAD_F = 20; ///< Parameter for the angle the robot inclines his head forward
	public int param_HAD_B = 20; ///< Parameter for the angle the robot inclines his head backward
	public int param_HAD_D = 45; ///< Parameter for the angle the robot turn his head left or right
	public int param_ARM_HR = 45; ///< Parameter specified the angle of rotation for the arms (up/down)
	public int param_ARM_LR = 20; ///< Parameter specified the angle of rotation for the arms (to the body and away)
	
	// variables used for touchgestures
	private float gesture_xa,gesture_xe,gesture_ya,gesture_ye;
	private int displayWidth  = 1;
	private int displayHeight = 1;
	
	/**
	* @class EvtHandler
	*
	* A class to handle the events from the callback-message-queue
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
		    	if(askfor_Makros_once)
		    	{
		    		if(msg.arg1 == NetworkModule.CONN_OPEN)
		    		{
		    			askfor_Makros_once = false;
		    			NetworkModule.AskForMakros();
		    		}
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
			    if( BatteryIcon!=null)
			    	BatteryIcon.setIcon(batt_icon_r);
			    Callbacksplit.saveBatteryStateIcon(batt_icon_r);
			    Callbacksplit.setActBarBatteryIcon(batt_icon_r);
			    
			    break;
		    case EVENT_MAKRO:
		    	addMakroToList( (String)msg.obj );
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
        NetworkModule.RegisterCallback(EvtHandler,	EVENT_MAKRO,	NetworkModule.INFO_MAKRO);
        
        Log.v("MainAct", "Activity started.");
        BattTimer = new Timer();
		//BattTimer.schedule(new BattTmrTask(EvtHandler), 1000, 10000);
        BattTimer.schedule(new BattTmrTask(), 1000, 10000);
        
        //Load Parameters
        FileInputStream fis = null;
        String[] paralist = null;
        try {
			fis = openFileInput("Parameter.sav");
			byte[] readData = new byte [fis.available()];
			fis.read(readData);
			paralist = (new String(readData)).split("\n");
			fis.close();
		}
        catch(FileNotFoundException fnfe){Log.v("MainAct","paraload: File not found exeption");}
		catch (IOException e)
		{
			e.printStackTrace();
			try{ fis.close();} catch (IOException ex){ex.printStackTrace();}
			paralist = null;
			fis = null;
		}
        
        if(paralist!=null && paralist.length == 8)
        {
        	param_MOV_F = Float.valueOf(paralist[0]); 
        	param_MOV_B = Float.valueOf(paralist[1]);
        	param_MOV_D = Integer.valueOf(paralist[2]);
        	param_HAD_F = Integer.valueOf(paralist[3]);
        	param_HAD_B = Integer.valueOf(paralist[4]);
        	param_HAD_D = Integer.valueOf(paralist[5]);
        	param_ARM_HR = Integer.valueOf(paralist[6]);
        	param_ARM_LR = Integer.valueOf(paralist[7]);
        }

        
        VideoModule.init();
        VideoModule.startVideoServer();
        
        newMakroToList();
        
        displayWidth = getWindowManager().getDefaultDisplay().getWidth();
    	displayHeight = getWindowManager().getDefaultDisplay().getHeight();
        
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
    	VideoModule.closeVideoDialog();
    	BattTimer.cancel();
    	NetworkModule.CloseConnection();
    	VideoModule.stopVideoServer();
    	try {
    		FileOutputStream fos = openFileOutput("Parameter.sav", Context.MODE_PRIVATE);
			fos.write( (String.valueOf(param_MOV_F)+"\n").getBytes());
			fos.write( (String.valueOf(param_MOV_B)+"\n").getBytes());
			fos.write( (String.valueOf(param_MOV_D)+"\n").getBytes());
			fos.write( (String.valueOf(param_HAD_F)+"\n").getBytes());
			fos.write( (String.valueOf(param_HAD_B)+"\n").getBytes());
			fos.write( (String.valueOf(param_HAD_D)+"\n").getBytes());
			fos.write( (String.valueOf(param_ARM_HR)+"\n").getBytes());
			fos.write( (String.valueOf(param_ARM_LR)).getBytes());
			Log.v("MainAct","parasave:" + String.valueOf(param_ARM_LR));
			fos.close();
		}
		catch(Exception e){	e.printStackTrace();}
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
		case R.id.acb_video:
			VideoModule.create_dialog(MainActivity.this, true);
			break;
		case R.id.acb_m_1:
			intent = new Intent(this, BewegungActivity.class);
			startActivity(intent);
			break;
		case R.id.acb_m_2:
			intent = new Intent(this, SprachausgabeActivity.class);
			startActivity(intent);
			break;
		case R.id.acb_m_3:
			intent = new Intent(this, SpecialsActivity.class);
			startActivity(intent);
			break;
		case R.id.acb_connect:
		case R.id.acb_m_4:
			intent = new Intent(this, ConfigActivity.class);
			startActivity(intent);
			break;
		case R.id.acb_m_5:
			intent = new Intent(this, SettingActivity.class);
			startActivity(intent);
			break;
		}
		
		return true;
	}
    
    /**
     * Refreshes the ActionBar's network state icon.
     */
    public void setActBarConnectIcon(){
    	if(ConnectIcon == null && BatteryIcon != null)
    		return;
    	
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
    
    
    
    /* ********************* Menu *************************
     **************************************************** */
    
    /**
	 * Opens and starts the BewegungActivity 
	 * 
	 * @param view		ignored, ID of the button, which triggers this function
	 */
    public void menu_button1_event(View view) {
    	final Intent bewintent = new Intent (this,BewegungActivity.class);
    	//Log.v("main button1", "bewIntent: " + bewintent.toString());
    	startActivity(bewintent);
    }
    
    /**
	 * Opens and starts the SprachausgabeActivity 
	 * 
	 * @param view		ignored, ID of the button, which triggers this function
	 */
    public void menu_button2_event(View view) {
    	final Intent spkintent = new Intent (this,SprachausgabeActivity.class);
    	startActivity(spkintent);
    }
    
    /**
	 * Opens and starts the SpecialsActivity 
	 * 
	 * @param view		ignored, ID of the button, which triggers this function
	 */
    public void menu_button3_event(View view) {
    	final Intent specialsintent = new Intent (this,SpecialsActivity.class);
    	startActivity(specialsintent);
    }
    
    /**
	 * Opens and starts the ConfigActivity 
	 * 
	 * @param view		ignored, ID of the button, which triggers this function
	 */
    public void menu_button4_event(View view) {
    	final Intent cfgintent = new Intent (this,ConfigActivity.class);
    	startActivity(cfgintent);
    }
    
    /**
	 * Only shows the about dialoge 
	 * 
	 * @param view		ignored, ID of the button, which triggers this function
	 */
    public void menu_button5_event(View view) {
    	final Dialog about_dialog = new Dialog(MainActivity.this);
    	about_dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
    	about_dialog.setContentView(R.layout.dialog_about);
    	Button Button_close = (Button) about_dialog.findViewById(R.id.aboutdial_closebutton);
		/**
		 * By pressing the related buttton the about dialog will be closed
		 * 
		 */
		Button_close.setOnClickListener(new OnClickListener() {			
			@Override
			public void onClick(View v) {		    	
		    	about_dialog.dismiss();
			}
		});
    	about_dialog.show();
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
	
	//______ Funktions for Makros _________
	
	/**
	 * Appents names of makros which are received from the robot to a list saved in a file (filename is MakroListe.txt).
	 * With makro the motion sequence created with coreographe were meant
	 * 
	 * @param neutext Name of the makro
	 */
	private void addMakroToList(String neutext)
	{
		FileOutputStream fos = null;
		
		try {
			fos = openFileOutput("MakroListe.txt", Context.MODE_APPEND|Context.MODE_PRIVATE);
			fos.write( (neutext+"\n").getBytes() );
			fos.close();
		}
		catch(FileNotFoundException e){
			e.printStackTrace();
		}
		catch(IOException e)
		{
			e.printStackTrace();
		}
	}
	
	/**
	 * Creates a new empty file with the filename MakroListe.txt.
	 * Later there can be saved names of motion sequence as a list in it.
	 */
	private void newMakroToList()
	{
		FileOutputStream fos = null;
		
		try {
			fos = openFileOutput("MakroListe.txt", Context.MODE_PRIVATE);
			fos.close();
		}
		catch(FileNotFoundException e){
			e.printStackTrace();
		}
		catch(IOException e)
		{
			e.printStackTrace();
		}
	}
	
    
    /**
     * Modify the touchlistener for the whole activity to accept a selfdefined wipegesture
     * This wipe gesture is for changig to another activity with a wipe
     */
    @Override
    public boolean dispatchTouchEvent(MotionEvent event)
    {
    	switch(event.getAction())
		{
		case MotionEvent.ACTION_DOWN:
			gesture_xa=event.getX();
			gesture_ya=event.getY();
			break;
		case MotionEvent.ACTION_UP:
			gesture_xe=event.getX();
			gesture_ye=event.getY();
			//Toast.makeText(SpecialsActivity.this, String.valueOf(gesture_xe-gesture_xa)+"/"+String.valueOf(gesture_ye-gesture_ya)+"/"+String.valueOf(displayWidth/2)+"/"+String.valueOf(displayHeight*0.1), Toast.LENGTH_SHORT).show();
			if(Math.abs(gesture_ye-gesture_ya) < displayHeight*0.1)
			{
				if( gesture_xe-gesture_xa > displayWidth/2 )
				{
					// wipe from left to right
					Intent intent = new Intent(Callbacksplit.getMainActivity(), SettingActivity.class);
					startActivity(intent);
					return true;
				}
				else if( -(gesture_xe-gesture_xa) > displayWidth/2 )
				{
					// wipe from right to left
					Intent intent = new Intent(Callbacksplit.getMainActivity(), BewegungActivity.class);
					startActivity(intent);
					return true;
				}
			}
			break;
		}
    	super.dispatchTouchEvent(event);
    	return true;
    }
}
