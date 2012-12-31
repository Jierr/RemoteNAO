package de.tuchemnitz.remoteclient;

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
	static private RadioGroup bewart_radiogroup;

	private MenuItem BatteryIcon;
	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_bewegung);
		
		Callbacksplit.registerBewegungActivity(this);
		
		Log.v("BewAct", "init BewActivity");
		bewart_radiogroup = (RadioGroup) findViewById(R.id.bew_radioGroup1);
		bewart_radiogroup.check(bewegungsart);
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
        BatteryIcon = (MenuItem)menu.findItem(R.id.acb_battery);
        setActBarBatteryIcon(Callbacksplit.getsavedBatteryStateIcon());
        return true;
    }
	
	private void setListeners(){
		
		bewart_radiogroup.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			
			@Override
			public void onCheckedChanged(RadioGroup group, int checkedId) {
				bewegungsart = checkedId;
			}
		});
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
    	case R.id.bewa_rbutton_ARME:
    		MoveType = 1;	// ARM
    		ToastStr = "Arm hoch";
    		break;
    	default:
    		MoveType = 2;	// HEAD
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
    	case R.id.bewa_rbutton_ARME:
    		MoveType = 1;	// ARM
    		ToastStr = "Arm links";
    		break;
    	default:
    		MoveType = 2;	// HEAD
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
    	case R.id.bewa_rbutton_ARME:
    		MoveType = 1;	// ARM
    		ToastStr = "Arm rechts";
    		break;
    	default:
    		MoveType = 2;	// HEAD
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
    	case R.id.bewa_rbutton_ARME:
    		MoveType = 1;	// ARM
    		ToastStr = "Arm runter";
    		break;
    	default:
    		MoveType = 2;	// HEAD
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

}
