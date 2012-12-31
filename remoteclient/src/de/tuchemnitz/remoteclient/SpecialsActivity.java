package de.tuchemnitz.remoteclient;

import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.actionbarsherlock.app.SherlockActivity;
import com.actionbarsherlock.view.Menu;
import com.actionbarsherlock.view.MenuItem;

public class SpecialsActivity extends SherlockActivity {
	
	private MenuItem BatteryIcon;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_specials);
		
		Callbacksplit.registerSpecialsActivity(this);
		
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
	
	
	/*********************** Auswahl ************************
     ********************************************************/
	
	public void specials_button1_event(View view) {
    	Toast toast = Toast.makeText(SpecialsActivity.this, "Motoren entspannen", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    	NetworkModule.Rest();    	
    }
	
	public void specials_button2_event(View view) {
    	NetworkModule.SitToggle();
    }
    
    public void specials_button3_event(View view) {
    	Toast toast = Toast.makeText(SpecialsActivity.this, "Tanz", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    	NetworkModule.Dance("Rumba");
    }
    
    public void specials_button4_event(View view) {
    	Toast toast = Toast.makeText(SpecialsActivity.this, "nichts passiert", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    }
    
    public void specials_button5_event(View view) {
    	Toast toast = Toast.makeText(SpecialsActivity.this, "nichts passiert", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    }
    
    /************ Dynamisches Aenderugszeug *****************
     ********************************************************/
    
    public void changeSitButtonText(String state){
    	final Button sitButton_text = (Button)findViewById(R.id.specials_button2);
    	
    	if (state.equals("STAND"))
    		sitButton_text.setText("Setzen");
    	else
    		sitButton_text.setText("Aufstehen");
    }
    
    public void setActBarBatteryIcon(Drawable pic){
    	if(pic!=null)
    		BatteryIcon.setIcon(pic);
    }

}
