package de.tuchemnitz.remoteclient;

import android.content.Intent;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.ToggleButton;

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
	
	private MenuItem BatteryIcon = null;
	private MenuItem ConnectIcon = null;
	private SeekBar seekbar_videovisibility = null;
	private ToggleButton video_toggle_OnOff = null;
	
	private TextView text_value_pMovF = null;
	private TextView text_value_pMovB = null;
	private TextView text_value_pMovD = null;
	private TextView text_value_pHadF = null;
	private TextView text_value_pHadB = null;
	private TextView text_value_pHadD = null;
	private TextView text_value_pArmHR = null;
	private TextView text_value_pArmLR = null;
	
	// variables used for touchgestures
	private float gesture_xa,gesture_xe,gesture_ya,gesture_ye;
	private int displayWidth  = 1;
	private int displayHeight = 1;
	
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
    	
    	//Parametereinstellungen
    	text_value_pMovF = (TextView) findViewById(R.id.set_val_movF);
    	text_value_pMovB = (TextView) findViewById(R.id.set_val_movB);
    	text_value_pMovD = (TextView) findViewById(R.id.set_val_movD);
    	text_value_pHadF = (TextView) findViewById(R.id.set_val_hadF);
    	text_value_pHadB = (TextView) findViewById(R.id.set_val_hadB);
    	text_value_pHadD = (TextView) findViewById(R.id.set_val_hadD);
    	text_value_pArmHR = (TextView) findViewById(R.id.set_val_armHR);
    	text_value_pArmLR = (TextView) findViewById(R.id.set_val_armLR);
    	init_seekbar_parameters();
    	
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
		Callbacksplit.unsetActiveActivity();
		super.onPause();
	}
	
	@Override
    public void onDestroy(){
		Callbacksplit.registerSettingActivity(null);
    	VideoModule.closeVideoDialog();
		super.onDestroy();
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
		case R.id.acb_connect:
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
	
	/**
	 * Starts and Stops the Videoserver with the udp-socket in a thread.
	 * 
	 * @param view ignored, ID of the button, which triggers this function
	 */
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
	
	/**
	 * Init the seekbar for setting the transparancy of 
	 * the video, showed in the background of the BewegungActivity.
	 * All listeners will be initialized.
	 * 
	 * This function is used in the oncreate-function of the Activity.
	 */
	private void init_seekbar_videotransparency()
	{
		seekbar_videovisibility = (SeekBar) findViewById(R.id.set_videobewalpha_seekBar);
    	seekbar_videovisibility.setProgress(VideoModule.Videotransparency_bewact);
    	
    	seekbar_videovisibility.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				VideoModule.Videotransparency_bewact = seekbar_videovisibility.getProgress();
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
	
	/**
	 * 	Help function to transform the parameter of
	 *  the movement to the right integer for te seekbar.
	 *  
	 * @param val The value to transform
	 * @return the transformed value useable by the seekbar
	 */
	private int parameter_float_to_int_mov(float val)
	{
		if(val > 5.0) return (((int)val)+50);
		else return ((int)(val*10));
	}
	/**
	 * Help function to transform the integer deliver
	 * by the seekbar to the parameter of the movement.
	 *  
	 * @param val The value to transform
	 * @return the transformed float value 
	 */
	private float parameter_int_to_float_mov(int val)
	{
		if(val > 50) return (float)val-50;
		else return (((float)val)/10.0f);
	}
	
	/**
	 * Initializes the seekbars. Set up the listeners
	 * and adjust the right value to avery seekbar
	 */
	private void init_seekbar_parameters()
	{
		final SeekBar seekbar_pMovF = (SeekBar) findViewById(R.id.set_param_movF_seekBar);
		seekbar_pMovF.setProgress(parameter_float_to_int_mov(Callbacksplit.getMainActivity().param_MOV_F));
		text_value_pMovF.setText(String.valueOf(Callbacksplit.getMainActivity().param_MOV_F)+" m");
		seekbar_pMovF.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				text_value_pMovF.setText(String.valueOf(parameter_int_to_float_mov(seekBar.getProgress()))+" m");
				Callbacksplit.getMainActivity().param_MOV_F = parameter_int_to_float_mov(seekBar.getProgress());
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				text_value_pMovF.setText(String.valueOf(parameter_int_to_float_mov(progress))+" m");
			}
		});
		
		final SeekBar seekbar_pMovB = (SeekBar) findViewById(R.id.set_param_movB_seekBar);
		seekbar_pMovB.setProgress(parameter_float_to_int_mov(Callbacksplit.getMainActivity().param_MOV_B));
		text_value_pMovB.setText(String.valueOf(Callbacksplit.getMainActivity().param_MOV_B)+" m");
		seekbar_pMovB.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				text_value_pMovB.setText(String.valueOf(parameter_int_to_float_mov(seekBar.getProgress()))+" m");
				Callbacksplit.getMainActivity().param_MOV_B = parameter_int_to_float_mov(seekBar.getProgress());
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				text_value_pMovB.setText(String.valueOf(parameter_int_to_float_mov(progress))+" m");
			}
		});
		
		final SeekBar seekbar_pMovD = (SeekBar) findViewById(R.id.set_param_movD_seekBar);
		seekbar_pMovD.setProgress(Callbacksplit.getMainActivity().param_MOV_D);
		text_value_pMovD.setText(String.valueOf(Callbacksplit.getMainActivity().param_MOV_D)+"°");
		seekbar_pMovD.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				text_value_pMovD.setText(String.valueOf(seekBar.getProgress())+"°");
				Callbacksplit.getMainActivity().param_MOV_D = seekBar.getProgress();
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				text_value_pMovD.setText(String.valueOf(progress)+"°");
			}
		});
		
		final SeekBar seekbar_pHadF = (SeekBar) findViewById(R.id.set_param_hadF_seekBar);
		seekbar_pHadF.setProgress(Callbacksplit.getMainActivity().param_HAD_F);
		text_value_pHadF.setText(String.valueOf(Callbacksplit.getMainActivity().param_HAD_F)+"°");
		seekbar_pHadF.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				text_value_pHadF.setText(String.valueOf(seekBar.getProgress())+"°");
				Callbacksplit.getMainActivity().param_HAD_F = seekBar.getProgress();
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				text_value_pHadF.setText(String.valueOf(progress)+"°");
			}
		});
		
		final SeekBar seekbar_pHadB = (SeekBar) findViewById(R.id.set_param_hadB_seekBar);
		seekbar_pHadB.setProgress(Callbacksplit.getMainActivity().param_HAD_B);
		text_value_pHadB.setText(String.valueOf(Callbacksplit.getMainActivity().param_HAD_B)+"°");
		seekbar_pHadB.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				text_value_pHadB.setText(String.valueOf(seekBar.getProgress())+"°");
				Callbacksplit.getMainActivity().param_HAD_B = seekBar.getProgress();
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				text_value_pHadB.setText(String.valueOf(progress)+"°");
			}
		});
		
		final SeekBar seekbar_pHadD = (SeekBar) findViewById(R.id.set_param_hadD_seekBar);
		seekbar_pHadD.setProgress(Callbacksplit.getMainActivity().param_HAD_D);
		text_value_pHadD.setText(String.valueOf(Callbacksplit.getMainActivity().param_HAD_D)+"°");
		seekbar_pHadD.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				text_value_pHadD.setText(String.valueOf(seekBar.getProgress())+"°");
				Callbacksplit.getMainActivity().param_HAD_D = seekBar.getProgress();
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				text_value_pHadD.setText(String.valueOf(progress)+"°");
			}
		});
		
		final SeekBar seekbar_pArmHR = (SeekBar) findViewById(R.id.set_param_armHR_seekBar);
		seekbar_pArmHR.setProgress(Callbacksplit.getMainActivity().param_ARM_HR);
		text_value_pArmHR.setText(String.valueOf(Callbacksplit.getMainActivity().param_ARM_HR)+"°");
		seekbar_pArmHR.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				text_value_pArmHR.setText(String.valueOf(seekBar.getProgress())+"°");
				Callbacksplit.getMainActivity().param_ARM_HR = seekBar.getProgress();
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				text_value_pArmHR.setText(String.valueOf(progress)+"°");
			}
		});
		
		final SeekBar seekbar_pArmLR = (SeekBar) findViewById(R.id.set_param_armLR_seekBar);
		seekbar_pArmLR.setProgress(Callbacksplit.getMainActivity().param_ARM_LR);
		text_value_pArmLR.setText(String.valueOf(Callbacksplit.getMainActivity().param_ARM_LR)+"°");
		seekbar_pArmLR.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				text_value_pArmLR.setText(String.valueOf(seekBar.getProgress())+"°");
				Callbacksplit.getMainActivity().param_ARM_LR = seekBar.getProgress();
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
			}
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				text_value_pArmLR.setText(String.valueOf(progress)+"°");
			}
		});
		
	}
	
	/* *********** Dynamisches Aenderugszeug *****************
     ******************************************************* */
    
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
					Intent intent = new Intent(Callbacksplit.getMainActivity(), ConfigActivity.class);
					finish();
					startActivity(intent);
					return true;
				}
				else if( -(gesture_xe-gesture_xa) > displayWidth/2 )
				{
					// wipe from right to left
					finish();
					return true;
				}
			}
			break;
		}
    	super.dispatchTouchEvent(event);
    	return true;
    }

}
