package de.tuchemnitz.remoteclient;

import android.content.Intent;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.widget.RadioGroup;
import android.widget.RadioGroup.OnCheckedChangeListener;
import android.widget.Toast;

import com.actionbarsherlock.app.SherlockActivity;
import com.actionbarsherlock.view.Menu;
import com.actionbarsherlock.view.MenuItem;

public class BewegungActivity extends SherlockActivity {

	static private int bewegungsart = R.id.bewa_rbutton_LAUFEN;
	private RadioGroup bewart_radiogroup1;
	private RadioGroup bewart_radiogroup2;
	private RadioGroup bewart_radiogroupA;
	private OnCheckedChangeListener Grp1Listener = null;
	private OnCheckedChangeListener Grp2Listener = null;

	private MenuItem BatteryIcon;
	private MenuItem ConnectIcon;
	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_bewegung);
		
		Callbacksplit.registerBewegungActivity(this);
		
		Log.v("BewAct", "init BewActivity");
		bewart_radiogroup1 = (RadioGroup) findViewById(R.id.bew_radioGroup1);
		if (bewart_radiogroup1 != null)
			bewart_radiogroup1.check(bewegungsart);
		bewart_radiogroup2 = (RadioGroup) findViewById(R.id.bew_radioGroup2);
		if (bewart_radiogroup2 != null)
			bewart_radiogroup2.check(bewegungsart);
		bewart_radiogroupA = (RadioGroup) findViewById(R.id.bew_radioGroupA);
		if (bewart_radiogroupA != null)
			bewart_radiogroupA.check(bewegungsart);
		setListeners();

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
    	Callbacksplit.registerBewegungActivity(null);
    }

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
        
        ((MenuItem)menu.findItem(R.id.acb_m_2)).setVisible(false);
        
        return true;
    }
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		super.onOptionsItemSelected(item);
		Intent intent;
		switch(item.getItemId()){
		case android.R.id.home:
		case R.id.acb_m_1:
			finish();
			break;
		case R.id.acb_m_2:
			break;
		case R.id.acb_m_3:
			intent = new Intent(Callbacksplit.getMainActivity(), SprachausgabeActivity.class);
			finish();
			startActivity(intent);
			break;
		case R.id.acb_m_4:
			intent = new Intent(Callbacksplit.getMainActivity(), SpecialsActivity.class);
			finish();
			startActivity(intent);
			break;
		case R.id.acb_m_5:
			intent = new Intent(Callbacksplit.getMainActivity(), ConfigActivity.class);
			finish();
			startActivity(intent);
			break;
		}
		
		return true;
	}
	
	private void setListeners(){
		
		if (bewart_radiogroupA != null)
			bewart_radiogroupA.setOnCheckedChangeListener(new OnCheckedChangeListener() {
				@Override
				public void onCheckedChanged(RadioGroup group, int checkedId) {
					bewegungsart = checkedId;
					Log.v("Check", "Clicked on All: " + Integer.toString(checkedId));
				}
			});
		if (bewart_radiogroup1 != null)
		{
			Grp1Listener = new OnCheckedChangeListener() {
				@Override
				public void onCheckedChanged(RadioGroup group, int checkedId) {
					if (checkedId != -1)
					{
						bewegungsart = checkedId;
						bewart_radiogroup2.setOnCheckedChangeListener(null);
						bewart_radiogroup2.clearCheck();
						bewart_radiogroup2.setOnCheckedChangeListener(Grp2Listener);
					}
					Log.v("Check", "Clicked on 1: " + Integer.toString(checkedId));
				}
			};
			bewart_radiogroup1.setOnCheckedChangeListener(Grp1Listener);
		}
		if (bewart_radiogroup2 != null)
		{
			Grp2Listener = new OnCheckedChangeListener() {
				@Override
				public void onCheckedChanged(RadioGroup group, int checkedId) {
					if (checkedId != -1)
					{
						bewegungsart = checkedId;
						bewart_radiogroup1.setOnCheckedChangeListener(null);
						bewart_radiogroup1.clearCheck();
						bewart_radiogroup1.setOnCheckedChangeListener(Grp1Listener);
					}
					Log.v("Check", "Clicked on 2: " + Integer.toString(checkedId));
				}
			};
			bewart_radiogroup2.setOnCheckedChangeListener(Grp2Listener);
		}
	}
	
	/*bewa_radiogroup.setOnCheckedChangeListener(new OnCheckedChangeListener() {
	
	@Override
	public void onCheckedChanged(RadioGroup group, int checkedId) {
		Toast toast = Toast.makeText(MainActivity.this, "checked: " + Integer.toString(checkedId), Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.TOP | Gravity.RIGHT, 0, 0);
    	toast.show();
	}
	});*/
	/* ------------ Funktion bei OK Button ------------ */
	/*Button dial_button_ok = (Button) bewa_dialog.findViewById(R.id.bewa_OKbutton);
	dial_button_ok.setOnClickListener(new OnClickListener() {
		@Override
		public void onClick(View view) {
			bewa_dialog.dismiss();
			final RadioGroup bewa_radiogroup = (RadioGroup)bewa_dialog.findViewById(R.id.bew_radioGroup1);
			int checkedId = bewa_radiogroup.getCheckedRadioButtonId();
			
			bewegungsart = checkedId;
		}
	});*/
	
	
	/************** BEW_BUTTONS ******************
     *********************************************/
    
    public void bew_button1_event(View view) {
    	int MoveType;
    	String ToastStr;
    	
    	switch(bewegungsart)
    	{
    	case R.id.bewa_rbutton_LAUFEN:
    		MoveType = 0;	// MOVE
    		ToastStr = "vorwärts";
    		break;
    	case R.id.bewa_rbutton_ARM_L:
    		MoveType = 1;	// ARM
    		ToastStr = "l. Arm hoch";
    		break;
    	case R.id.bewa_rbutton_ARM_R:
    		MoveType = 2;	// ARM
    		ToastStr = "r. Arm hoch";
    		break;
    	default:
    		MoveType = 3;	// HEAD
    		ToastStr = "Kopf vor";
    		break;
    	}
		NetworkModule.Move(MoveType, NetworkModule.MOVE_UP);
    	Toast toast = Toast.makeText(BewegungActivity.this, ToastStr, Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    }
    
    public void bew_button2_event(View view) {
    	int MoveType;
    	String ToastStr;
    	
    	switch(bewegungsart)
    	{
    	case R.id.bewa_rbutton_LAUFEN:
    		MoveType = 0;	// MOVE
    		ToastStr = "links";
    		break;
    	case R.id.bewa_rbutton_ARM_L:
    		MoveType = 1;	// ARM
    		ToastStr = "l. Arm links";
    		break;
    	case R.id.bewa_rbutton_ARM_R:
    		MoveType = 2;	// ARM
    		ToastStr = "r. Arm links";
    		break;
    	default:
    		MoveType = 3;	// HEAD
    		ToastStr = "Kopf links";
    		break;
    	}
		NetworkModule.Move(MoveType, NetworkModule.MOVE_LEFT);
    	Toast toast = Toast.makeText(BewegungActivity.this, ToastStr, Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    }
    
    public void bew_button3_event(View view) {
    	int MoveType;
    	String ToastStr;
    	
    	switch(bewegungsart)
    	{
    	case R.id.bewa_rbutton_LAUFEN:
    		MoveType = 0;	// MOVE
    		ToastStr = "rechts";
    		break;
    	case R.id.bewa_rbutton_ARM_L:
    		MoveType = 1;	// ARM
    		ToastStr = "l. Arm rechts";
    		break;
    	case R.id.bewa_rbutton_ARM_R:
    		MoveType = 2;	// ARM
    		ToastStr = "r. Arm rechts";
    		break;
    	default:
    		MoveType = 3;	// HEAD
    		ToastStr = "Kopf rechts";
    		break;
    	}
		NetworkModule.Move(MoveType, NetworkModule.MOVE_RIGHT);
    	Toast toast = Toast.makeText(BewegungActivity.this, ToastStr, Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    }
    
    public void bew_button4_event(View view) {
    	int MoveType;
    	String ToastStr;
    	
    	switch(bewegungsart)
    	{
    	case R.id.bewa_rbutton_LAUFEN:
    		MoveType = 0;	// MOVE
    		ToastStr = "rückwärts";
    		break;
    	case R.id.bewa_rbutton_ARM_L:
    		MoveType = 1;	// ARM
    		ToastStr = "l. Arm runter";
    		break;
    	case R.id.bewa_rbutton_ARM_R:
    		MoveType = 2;	// ARM
    		ToastStr = "r. Arm runter";
    		break;
    	default:
    		MoveType = 3;	// HEAD
    		ToastStr = "Kopf hinter";
    		break;
    	}
		NetworkModule.Move(MoveType, NetworkModule.MOVE_DOWN);
    	Toast toast = Toast.makeText(BewegungActivity.this, ToastStr, Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    }
    
    public void bew_button5_event(View view) {
    	
    	NetworkModule.Stop();
    	Toast toast = Toast.makeText(BewegungActivity.this, "STOP", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    }
    
    
    /************ Dynamisches Aenderugszeug *****************
     ********************************************************/
    
    public void setActBarBatteryIcon(Drawable pic){
    	if(pic!=null)
    		BatteryIcon.setIcon(pic);
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

}
