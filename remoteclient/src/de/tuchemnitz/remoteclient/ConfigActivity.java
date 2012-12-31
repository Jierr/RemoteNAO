package de.tuchemnitz.remoteclient;

import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.actionbarsherlock.app.SherlockActivity;
import com.actionbarsherlock.view.Menu;
import com.actionbarsherlock.view.MenuItem;

public class ConfigActivity extends SherlockActivity {

	private TextView text_verbindungsstatus;
	private EditText textfeld_ipeingabe;
	/*Buttons*/
	private Button button_verbindenbutton;
	
	private MenuItem BatteryIcon;
	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_config);
		
		Callbacksplit.registerConfigActivity(this);
		
		//Referenzen auf Zeug
		text_verbindungsstatus = (TextView)findViewById(R.id.verbindungsstatus_wert);
		textfeld_ipeingabe = (EditText)findViewById(R.id.verbindungsip_eingabe);
		button_verbindenbutton = (Button)findViewById(R.id.verbindungsbutton);
		
		//setzen von ansichten
		switch(NetworkModule.IsConnected())
    	{
    	case 0:
    		text_verbindungsstatus.setText("nicht verbunden");
        	text_verbindungsstatus.setTextColor(Color.RED);
        	break;
    	case 1:
    		text_verbindungsstatus.setText("verbinden ...");
        	text_verbindungsstatus.setTextColor(Color.YELLOW);
        	break;
    	case 2:
    		text_verbindungsstatus.setText("verbunden");
        	text_verbindungsstatus.setTextColor(Color.GREEN);
        	button_verbindenbutton.setText("trennen");
        	break;
    	}
    	textfeld_ipeingabe.setText(NetworkModule.GetIPAddress());
    	
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
    	Callbacksplit.registerConfigActivity(null);
    }
	
	@Override
    public boolean onCreateOptionsMenu(Menu menu) {
        //getMenuInflater().inflate(R.menu.activity_main, menu);
    	super.onCreateOptionsMenu(menu);
        getSupportMenuInflater().inflate(R.menu.actionbar, menu);
        BatteryIcon = (MenuItem)menu.findItem(R.id.acb_battery);
        setActBarBatteryIcon(Callbacksplit.getsavedBatteryStateIcon());
    	//setContentView(R.menu.actionbar);
//        menu.add("Save")
//        .setIcon(R.drawable.bat100)
//        .setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);
        return true;
    }
	
//	   /* Dialogbox erstellen*/
//    	verbindungs_dialog = new Dialog(MainActivity.this);
//    	verbindungs_dialog.setContentView(R.layout.netzwerkverbindung);
//    	verbindungs_dialog.setTitle("Netzwerkverbindung");
//    	
//    	final TextView text_verbindungsstatus = (TextView)verbindungs_dialog.findViewById(R.id.verbindungsstatus_wert);
//    	final EditText textfeld_ipeingabe = (EditText)verbindungs_dialog.findViewById(R.id.verbindungsip_eingabe);
//    	/*Buttons*/
//    	final Button dial_button_verbinden = (Button) verbindungs_dialog.findViewById(R.id.verbindungsbutton);
//    	final Button dial_button_close = (Button) verbindungs_dialog.findViewById(R.id.verbindungsbutton_close);
//    	/* ----------- Funktionen der Button ------------- */
//    	/* Verbinden */
//    	dial_button_verbinden.setOnClickListener(new OnClickListener() {
//			@Override
//			public void onClick(View v) {
//				
//			}
//		});
//    	/* schließen */
//    	dial_button_close.setOnClickListener(new OnClickListener() {
//			@Override
//			public void onClick(View v) {
//				verbindungs_dialog.dismiss();
//				verbindungs_dialog = null;
//			}
//		});
//    	
//    	
//    	/*Dialogbox anzeigen*/
//    	verbindungs_dialog.show();
//    	*/
	
	public void verbindungsbutton_event(View view)
	{
		/*Text aus dem Texteingabefeld*/
    	String ip_word = textfeld_ipeingabe.getText().toString();
    	ip_word = ip_word.trim();
    	
    	if (NetworkModule.IsConnected() == NetworkModule.CONN_OPEN)
    	{
    		NetworkModule.CloseConnection();
    	}
    	else if (NetworkModule.IsConnected() == NetworkModule.CONN_CLOSED)
    	{
	    	NetworkModule.SetIPAddress(ip_word);
	    	NetworkModule.OpenConnection();
    	}
	}
	
	public void cfgbutton_close_event(View view)
	{
		finish();
	}
	
	
	/************ Dynamisches Aenderugszeug *****************
     ********************************************************/
    
    public void changeConnectionView(int state){
    	switch(state)
    	{
    	case 0:
    		text_verbindungsstatus.setText("nicht verbunden");
        	text_verbindungsstatus.setTextColor(Color.RED);
        	button_verbindenbutton.setText("verbinden");
        	break;
    	case 1:
    		text_verbindungsstatus.setText("verbinden ...");
        	text_verbindungsstatus.setTextColor(Color.YELLOW);
        	break;
    	case 2:
    		text_verbindungsstatus.setText("verbunden");
        	text_verbindungsstatus.setTextColor(Color.GREEN);
        	button_verbindenbutton.setText("trennen");
        	break;
    	}
    }
    
    public void setActBarBatteryIcon(Drawable pic){
    	if(pic!=null)
    		BatteryIcon.setIcon(pic);
    }
	
	
}
