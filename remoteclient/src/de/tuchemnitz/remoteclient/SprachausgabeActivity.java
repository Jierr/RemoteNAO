package de.tuchemnitz.remoteclient;

import android.content.Intent;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

import com.actionbarsherlock.app.SherlockActivity;
import com.actionbarsherlock.view.Menu;
import com.actionbarsherlock.view.MenuItem;

public class SprachausgabeActivity extends SherlockActivity {

	private MenuItem BatteryIcon;
	private MenuItem ConnectIcon;
	
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
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        BatteryIcon = (MenuItem)menu.findItem(R.id.acb_battery);
        setActBarBatteryIcon(Callbacksplit.getsavedBatteryStateIcon());
        ConnectIcon = (MenuItem)menu.findItem(R.id.acb_connect);
        setActBarConnectIcon();
        
        ((MenuItem)menu.findItem(R.id.acb_m_3)).setVisible(false);
        
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
