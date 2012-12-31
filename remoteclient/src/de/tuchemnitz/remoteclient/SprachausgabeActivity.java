package de.tuchemnitz.remoteclient;

import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

import com.actionbarsherlock.app.SherlockActivity;
import com.actionbarsherlock.view.Menu;
import com.actionbarsherlock.view.MenuItem;

public class SprachausgabeActivity extends SherlockActivity {

	private MenuItem BatteryIcon;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_sprachausgabe);
		
		Callbacksplit.registerSprachausgabeActivity(this);
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
    	Callbacksplit.registerSprachausgabeActivity(null);
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
	
	
	public void okbutton_event(View view){
		EditText textfeld = (EditText)findViewById(R.id.spk_editText1);
    	String words = textfeld.getText().toString();
    	NetworkModule.Speak(words);
    	textfeld.setText(null);
    	finish();
    	
	}
	
	
    /************ Dynamisches Aenderugszeug *****************
     ********************************************************/
    
	public void setActBarBatteryIcon(Drawable pic){
    	if(pic!=null)
    		BatteryIcon.setIcon(pic);
    }
	
	
//	/* Dialogbox erstellen*/
//	final Dialog spk_dialog = new Dialog(MainActivity.this);
//	spk_dialog.setContentView(R.layout.activity_sprachausgabe);
//	spk_dialog.setTitle("Sprachausgabe");
//	
//	/* ------------ Funktion bei OK Button ------------ */
//	Button dial_button_ok = (Button) spk_dialog.findViewById(R.id.spk_ok_button);
//	dial_button_ok.setOnClickListener(new OnClickListener() {
//	
//		@Override
//		public void onClick(View v) {
//			spk_dialog.dismiss();
//			/*Text aus dem Texteingabefeld*/
//			
//		}
//	});
//	/*Dialogbox anzeigen*/
//	spk_dialog.show();

}
