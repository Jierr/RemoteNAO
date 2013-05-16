package de.tuchemnitz.remoteclient;

import android.content.Intent;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.SeekBar;
import android.widget.ToggleButton;
import android.widget.SeekBar.OnSeekBarChangeListener;

import com.actionbarsherlock.app.SherlockActivity;
import com.actionbarsherlock.view.Menu;
import com.actionbarsherlock.view.MenuItem;

/**
 * @file   SettingActivity.java
 * @author Riko Streller
 * 
 * Creates a surface, to justify different settings
 *
 */
public class SettingActivity extends SherlockActivity {
	
	private MenuItem BatteryIcon;
	private MenuItem ConnectIcon;
	private SeekBar seekbar_videosichtbarkeit = null;
	private ToggleButton video_toggle_OnOff = null;
	
	/**
	 * Loads/sets up the Activity layout and registers the callback.
	 */
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_setting);
		
		Callbacksplit.registerSettingActivity(this);
		
		//Referenzen auf Zeug    	
    	init_seekbar_videotransparency();
    	
    	video_toggle_OnOff = (ToggleButton) findViewById(R.id.set_video_toggle);
    	video_toggle_OnOff.setChecked(VideoModule.isVideoThreadStarted());
    	
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
    	Callbacksplit.registerSettingActivity(null);
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
        
        ((MenuItem)menu.findItem(R.id.acb_m_5)).setVisible(false);

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
			intent = new Intent(Callbacksplit.getMainActivity(), SpecialsActivity.class);
			finish();
			startActivity(intent);
			break;
		case R.id.acb_m_4:
			intent = new Intent(Callbacksplit.getMainActivity(), ConfigActivity.class);
			finish();
			startActivity(intent);
			break;
		case R.id.acb_m_5:
			break;
		case R.id.acb_video:
			VideoModule.create_dialog(SettingActivity.this, true);
			break;
		}
		
		
		return true;
	}
	
	/**
	 * Activates when the Close button is pressed.
	 * 
	 * @param view	ignored, ID of the button, which trigger this function
	 * 
	 * Closes the application.
	 */
	public void setbutton_close_event(View view)
	{
		finish();
	}
	
	public void set_videotoggle_on_off(View view)
	{
		if(VideoModule.isVideoThreadStarted())
		{
			VideoModule.stopVideoServer();
		}
		else
		{
			VideoModule.startVideoServer();
		}
		video_toggle_OnOff.setChecked(VideoModule.isVideoThreadStarted());
	}
	
	
	private void init_seekbar_videotransparency()
	{
		seekbar_videosichtbarkeit = (SeekBar) findViewById(R.id.set_videobewalpha_seekBar);
    	seekbar_videosichtbarkeit.setProgress(VideoModule.Videotransparency_bewact);
    	
    	seekbar_videosichtbarkeit.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				VideoModule.Videotransparency_bewact = seekbar_videosichtbarkeit.getProgress();
				Log.v("ConfigAct", "Videotransparency_bewact="+String.valueOf(VideoModule.Videotransparency_bewact));
			}
			
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
				
			}
			
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				// TODO Auto-generated method stub
				
			}
		});
	}
	
	
	/************ Dynamisches Aenderugszeug *****************
     ********************************************************/
    
    /**
     * Sets the Battery Icon in the ActionBar.
     * 
     * @param pic	Drawable of the Battery Icon
     */
    public void setActBarBatteryIcon(Drawable pic){
    	if(pic!=null)
    		BatteryIcon.setIcon(pic);
    }
	
    /**
     * Sets the Connection Icon depending on the current connection state.
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

}
