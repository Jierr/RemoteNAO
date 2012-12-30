package de.tuchemnitz.remoteclient;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.app.Activity;
import android.view.Gravity;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;

public class SpecialsActivity extends Activity {

	private final int EVENT_SIT = 1;
	
	
	private Handler EvtHandler = new Handler()
	{
		@Override public void dispatchMessage(Message msg)
		{
		    //super.dispatchMessage(msg);
		    
		    switch(msg.what)
		    {
		    case EVENT_SIT:
				final Button sit_text = (Button)findViewById(R.id.specials_button2);
		    	final String SitStatus = (String)msg.obj;
		    	
		    	if (SitStatus.equals("STAND"))
		    		sit_text.setText("Setzen");
		    	else
		    		sit_text.setText("Aufstehen");
		    	break;
		    }
		    
		    return;
		}
	};
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_specials);
		
		
		NetworkModule.RegisterCallback(EvtHandler,	EVENT_SIT,	NetworkModule.INFO_SIT);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_specials, menu);
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

}
