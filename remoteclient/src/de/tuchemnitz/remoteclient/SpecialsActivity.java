package de.tuchemnitz.remoteclient;

import android.content.Intent;
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
	private MenuItem ConnectIcon;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_specials);
		
		Callbacksplit.registerSpecialsActivity(this);
		
		changeSitButtonText( (String)NetworkModule.GetInfoData(NetworkModule.INFO_STATE) );
		
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
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        BatteryIcon = (MenuItem)menu.findItem(R.id.acb_battery);
        setActBarBatteryIcon(Callbacksplit.getsavedBatteryStateIcon());
        ConnectIcon = (MenuItem)menu.findItem(R.id.acb_connect);
        setActBarConnectIcon();
        
        ((MenuItem)menu.findItem(R.id.acb_m_4)).setVisible(false);
        
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
			intent = new Intent(Callbacksplit.getMainActivity(), BewegungActivity.class);
			finish();
			startActivity(intent);
			break;
		case R.id.acb_m_3:
			intent = new Intent(Callbacksplit.getMainActivity(), SprachausgabeActivity.class);
			finish();
			startActivity(intent);
			break;
		case R.id.acb_m_4:
			break;
		case R.id.acb_m_5:
			intent = new Intent(Callbacksplit.getMainActivity(), ConfigActivity.class);
			finish();
			startActivity(intent);
			break;
		}
		
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
		String state = (String) NetworkModule.GetInfoData(NetworkModule.INFO_STATE);
		if(state == null)
		{
			NetworkModule.StandUp();
		}
		else if (state.equals("STANDING") || state.equals("WALKING") || state.equals("STOPPING"))
		{
			NetworkModule.SitToggle();
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
    
    public void specials_button3_event(View view) {
    	Toast toast = Toast.makeText(SpecialsActivity.this, "Tanz", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    	NetworkModule.Dance(null);
    }
    
    public void specials_button4_event(View view) {
    	Toast toast = Toast.makeText(SpecialsActivity.this, "Winken", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    	NetworkModule.Wink();
    }
    
    public void specials_button5_event(View view) {
    	NetworkModule.Wipe();
    	Toast toast = Toast.makeText(SpecialsActivity.this, "PUH", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    }
    
    /************ Dynamisches Aenderugszeug *****************
     ********************************************************/
    
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
