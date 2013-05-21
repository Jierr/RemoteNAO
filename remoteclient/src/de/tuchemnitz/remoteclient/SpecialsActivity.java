package de.tuchemnitz.remoteclient;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;

import android.content.Intent;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.Toast;

import com.actionbarsherlock.app.SherlockActivity;
import com.actionbarsherlock.view.Menu;
import com.actionbarsherlock.view.MenuItem;

/**
 * 
 * @file   BewegungActivity.java
 * 
 * @author Riko Streller
 * 
 * Creates a surface to make the robot execute complex motion sequences by pressing one of the buttons
 *
 */
public class SpecialsActivity extends SherlockActivity {
	
	private MenuItem BatteryIcon = null;
	private MenuItem ConnectIcon = null;
	
	/**
	 * Loads/sets up the Activity layout and registers the callback.
	 */
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_specials);
		
		Callbacksplit.registerSpecialsActivity(this);
		
		changeSitButtonText( (String)NetworkModule.GetInfoData(NetworkModule.INFO_STATE) );
		
		final LinearLayout linlist = (LinearLayout) findViewById(R.id.spec_linLayout_list);
		
		
		// ------------ add Buttons dynamicly -------
		String[] makrobehaviors = getMakroList();
		if(makrobehaviors!=null)
		{
			int anzahl_behaviors = makrobehaviors.length;
			for(int i=0; i<anzahl_behaviors;i++)
			{
				final String makroname = makrobehaviors[i];
				Button btn = new Button(this);
				btn.setText(makroname);
				btn.setWidth(LayoutParams.MATCH_PARENT);
				btn.setHeight(LayoutParams.WRAP_CONTENT);
				btn.setOnClickListener(new OnClickListener() {
					@Override
					public void onClick(View v) {
						NetworkModule.ExecMakro(makroname);
					}
				});
				linlist.addView(btn);
			}
		}
		// ------------------------------------------
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
		super.onDestroy();
    	Callbacksplit.registerSpecialsActivity(null);
    	VideoModule.closeVideoDialog();
    }

	/**
	 * Loads ActionBar, enables Back button and
	 * gets and refreshes Battery and Connection Icons.
	 */
	@Override
    public boolean onCreateOptionsMenu(Menu menu) {
        //getMenuInflater().inflate(R.menu.activity_main, menu);
    	super.onCreateOptionsMenu(menu);
        getSupportMenuInflater().inflate(R.menu.actionbar, menu);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        BatteryIcon = (MenuItem)menu.findItem(R.id.acb_battery);
        setActBarBatteryIcon(Callbacksplit.getsavedBatteryStateIcon());
        ConnectIcon = (MenuItem)menu.findItem(R.id.acb_connect);
        setActBarConnectIcon();
        
        ((MenuItem)menu.findItem(R.id.acb_m_3)).setVisible(false);
        
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
			intent = new Intent(Callbacksplit.getMainActivity(), BewegungActivity.class);
			finish();
			startActivity(intent);
			break;
		case R.id.acb_m_2:
			intent = new Intent(Callbacksplit.getMainActivity(), SprachausgabeActivity.class);
			finish();
			startActivity(intent);
			break;
		case R.id.acb_m_3:
			break;
		case R.id.acb_m_4:
			intent = new Intent(Callbacksplit.getMainActivity(), ConfigActivity.class);
			finish();
			startActivity(intent);
			break;
		case R.id.acb_m_5:
			intent = new Intent(Callbacksplit.getMainActivity(), SettingActivity.class);
			finish();
			startActivity(intent);
			break;
		case R.id.acb_video:
			VideoModule.create_dialog(SpecialsActivity.this, true);
			break;
		}
		
		return true;
	}
	
	
	/*********************** Auswahl ************************
     ********************************************************/
	
	/**
	 * Function triggered by Rest-Button
	 * Makes the Networkmodule send a "RST" -> robot crouches and motors getting limp
	 * 
	 * @param view		ignored, ID of the button, which trigger this function
	 *  
	 */
	public void specials_button1_event(View view) {
    	Toast toast = Toast.makeText(SpecialsActivity.this, "Motoren entspannen", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    	NetworkModule.Rest();    	
    }
	
	/**
	 * Function triggered by Stand-/Sit-Button
	 * Makes the Networkmodule send a "SIT" or "AUF" -> robot stands up or sit down
	 * functionality of the button depending on the last state reported by the robot
	 * 
	 * @param view		ignored, ID of the button, which trigger this function
	 *  
	 */
	public void specials_button2_event(View view) {
		String state = (String) NetworkModule.GetInfoData(NetworkModule.INFO_STATE);
		if(state == null)
		{
			NetworkModule.StandUp();
		}
		else if (state.equals("STANDING") || state.equals("WALKING") || state.equals("STOPPING"))
		{
			NetworkModule.Sit();
			Toast toast = Toast.makeText(SpecialsActivity.this, "setzen", Toast.LENGTH_SHORT);
	    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
	    	toast.show();
		}
    	else
    	{
    		NetworkModule.StandUp();
    		Toast toast = Toast.makeText(SpecialsActivity.this, "aufstehen", Toast.LENGTH_SHORT);
	    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
	    	toast.show();
    	}
    }
    
	/**
	 * Function triggered by Dance-Button
	 * Makes the Networkmodule send a "DNC" -> robot begins to dance
	 * 
	 * @param view		ignored, ID of the button, which trigger this function
	 *  
	 */
    public void specials_button3_event(View view) {
    	Toast toast = Toast.makeText(SpecialsActivity.this, "Tanz", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    	NetworkModule.Dance(null);
    }
    
    /**
	 * Function triggered by Wave-Button
	 * Makes the Networkmodule send a "WNK" -> robot begins to wave with his arm and says hallo
	 * 
	 * @param view		ignored, ID of the button, which trigger this function
	 *  
	 */
    public void specials_button4_event(View view) {
    	Toast toast = Toast.makeText(SpecialsActivity.this, "Winken", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    	NetworkModule.Wink();
    }
    
    /**
	 * Function triggered by Wipe-Button
	 * Makes the Networkmodule send a "WIP" -> robot wipes his forehead
	 * 
	 * @param view		ignored, ID of the button, which trigger this function
	 *  
	 */
    public void specials_button5_event(View view) {
    	NetworkModule.Wipe();
    	Toast toast = Toast.makeText(SpecialsActivity.this, "PUH", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    }
    
    /************ Dynamisches Aenderugszeug *****************
     ********************************************************/
    
    /**
     * Changes the text on the button
     * 
     * @param state	State reported by robot
     */
    public void changeSitButtonText(String state){
    	final Button sitButton_text = (Button)findViewById(R.id.specials_button2);
    	
    	if(state == null)
		{
			return;
		}
		else if (state.equals("STANDING") || state.equals("WALKING") || state.equals("STOPPING"))
    		sitButton_text.setText("Setzen");
    	else
    		sitButton_text.setText("Aufstehen");
    }
    
    /**
     * Sets the Battery Icon in the ActionBar.
     * 
     * @param pic	Drawable of the Battery Icon
     */
    public void setActBarBatteryIcon(Drawable pic){
    	if(pic!=null && BatteryIcon != null)
    		BatteryIcon.setIcon(pic);
    }
    
    /**
     * Refreshes the ActionBar's network state icon.
     */
    public void setActBarConnectIcon(){
    	if(ConnectIcon == null)
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

    ///__________________ Makros _______________________
    /**
     * Loads a List of names of motion sequence from a file (MakroListe.txt) and returns it.
     * 
     * @return Array of Strings which are the names of the makros.
     */
    public String[] getMakroList()
	{
    	FileInputStream fis = null;
    	String collected = null;
		
		try {
			fis = openFileInput("MakroListe.txt");
			byte[] readData = new byte [fis.available()];
			while(fis.read(readData) > 0)
			{
				collected = new String(readData);
			}
		}
		catch (FileNotFoundException e)
		{
			e.printStackTrace();
			collected = null;
			fis = null;
		}
		catch (IOException e)
		{
			e.printStackTrace();
			collected = null;
		}
		finally
		{
			if(fis != null)
			{
				try{
					fis.close();
				}
				catch (IOException e)
				{
					e.printStackTrace();
				}
				fis = null;
			}
		}
		
		if(collected==null)
			return null;

		return collected.split("\n");
	}
	
}



