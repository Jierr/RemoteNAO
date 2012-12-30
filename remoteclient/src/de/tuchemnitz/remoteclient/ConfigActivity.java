package de.tuchemnitz.remoteclient;

import android.app.Activity;
import android.app.AlertDialog;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class ConfigActivity extends Activity {

	private TextView text_verbindungsstatus;
	private EditText textfeld_ipeingabe;
	/*Buttons*/
	private Button button_verbindenbutton;
	private final int EVENT_CONN = 2;
	private boolean is_activity_active = false;
	
	private Handler EvtHandler = new Handler()
	{
		@Override public void dispatchMessage(Message msg)
		{
		    //super.dispatchMessage(msg);
		    
		    switch(msg.what)
		    {
		    case EVENT_CONN:
		    	Log.v("MainAct.Event", "Connection Status: " + String.valueOf(msg.arg1));
		    	if (msg.arg1 < 0)
		    	{
		    		if(is_activity_active){
			    		switch(msg.arg1)
			    		{
			    		case -1:
				    		new AlertDialog.Builder(ConfigActivity.this)
								.setMessage("Verbindung verloren.")
								.setNeutralButton("Mist", null)
								.show();
				    		break;
			    		}
		    		}
		    	}
		    	else
		    	{
			    	//if (verbindungs_dialog == null)
			    		//return;
			    	
			    	switch(msg.arg1)
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
		    	break;
		    }
		    
		    return;
		}
	};
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_config);
		
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
    	
    	//Callbackzeug
    	NetworkModule.RegisterCallback(EvtHandler,	EVENT_CONN,	NetworkModule.INFO_CONN);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_config, menu);
		return true;
	}
	
	@Override
	protected void onResume(){
		super.onResume();
		is_activity_active = true;
	}
	@Override
	protected void onPause(){
		super.onPause();
		is_activity_active = false;
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
	

}
