package de.tuchemnitz.remoteclient;

import android.os.Bundle;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.graphics.Color;
import android.view.Gravity;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    	//NetworkModule.SetIPAddress("134.109.146.139");
        NetworkModule.SetIPAddress("134.109.151.142");
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_main, menu);
        return true;
    }
    
    /** Called when the user clicks the Send button */
    public void sendMessage(View view) {
        // Do something in response to button
    	boolean RetVal;
    	
    	RetVal = NetworkModule.OpenConnection(this);
    	if (! RetVal)
    	{
	    	new AlertDialog.Builder(this)
			.setMessage("Connection failed")
			.setNeutralButton("och", null)
			.show();
    	}
    	
    	NetworkModule.net_test();
    	NetworkModule.CloseConnection();
    	/*new AlertDialog.Builder(this)
			.setMessage("Test done.")
			.setNeutralButton("och", null)
			.show();*/
    	
    	return;
    }
    
    public void bew_button1_event(View view) {
    	NetworkModule.Move(true, NetworkModule.MOVE_UP);
    	Toast toast = Toast.makeText(MainActivity.this, "vorwärts", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    	}
    public void bew_button2_event(View view) {
    	NetworkModule.Move(true, NetworkModule.MOVE_LEFT);
    	Toast toast = Toast.makeText(MainActivity.this, "links", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    	}
    public void bew_button3_event(View view) {
    	NetworkModule.Move(true, NetworkModule.MOVE_RIGHT);
    	Toast toast = Toast.makeText(MainActivity.this, "rechts", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    	}
    public void bew_button4_event(View view) {
    	NetworkModule.Move(true, NetworkModule.MOVE_DOWN);
    	Toast toast = Toast.makeText(MainActivity.this, "rückwärts", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    	}
    
    
    
    public void menu_button2_event(View view) {
    	/* Dialogbox erstellen*/
    	final Dialog spk_dialog = new Dialog(MainActivity.this);
    	spk_dialog.setContentView(R.layout.sprachausgabe);
    	spk_dialog.setTitle("Sprachausgabe");
    	
    	/* ------------ Funktion bei OK Button ------------ */
    	Button dial_button_ok = (Button) spk_dialog.findViewById(R.id.spk_ok_button);
    	dial_button_ok.setOnClickListener(new OnClickListener() {
		
			@Override
			public void onClick(View v) {
				spk_dialog.dismiss();
				/*Text aus dem Texteingabefeld*/
				EditText textfeld = (EditText)spk_dialog.findViewById(R.id.spk_editText1);
		    	String words = textfeld.getText().toString();
	        	NetworkModule.Speak(words);
			}
		});
    	
    	/*Dialogbox anzeigen*/
    	spk_dialog.show();
    }
    
    
    public void menu_button5_event(View view) {
    	/* Dialogbox erstellen*/
    	final Dialog verbindungs_dialog = new Dialog(MainActivity.this);
    	verbindungs_dialog.setContentView(R.layout.netzwerkverbindung);
    	verbindungs_dialog.setTitle("Netzwerkverbindung");
    	
    	final TextView text_verbindungsstatus = (TextView)verbindungs_dialog.findViewById(R.id.verbindungsstatus_wert);
    	final EditText textfeld_ipeingabe = (EditText)verbindungs_dialog.findViewById(R.id.verbindungsip_eingabe);
    	
    	if(NetworkModule.IsConnected()){
    		text_verbindungsstatus.setText("verbunden");
        	text_verbindungsstatus.setTextColor(Color.GREEN);
    	}
    	else{
    		text_verbindungsstatus.setText("nicht verbunden");
        	text_verbindungsstatus.setTextColor(Color.RED);
    	}
    	textfeld_ipeingabe.setText(NetworkModule.GetIPAddress());
    	
    	
    	/* ----------- Funktionen der Button ------------- */
    	Button dial_button_close = (Button) verbindungs_dialog.findViewById(R.id.verbindungsbutton_close);
    	Button dial_button_verbinden = (Button) verbindungs_dialog.findViewById(R.id.verbindungsbutton);
    	/* Verbinden */
    	dial_button_verbinden.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				/*Text aus dem Texteingabefeld*/
		    	String ip_word = textfeld_ipeingabe.getText().toString();
		    	NetworkModule.SetIPAddress(ip_word);
		    	if(NetworkModule.IsConnected()){
		    		NetworkModule.CloseConnection();
		    	}
	        	if(NetworkModule.OpenConnection(MainActivity.this)){
	        		text_verbindungsstatus.setText("verbunden");
	            	text_verbindungsstatus.setTextColor(Color.GREEN);
	        	}
	        	else{
	        		text_verbindungsstatus.setText("nicht verbunden");
	            	text_verbindungsstatus.setTextColor(Color.RED);
	        	}
			}
		});
    	/* schließen */
    	dial_button_close.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				verbindungs_dialog.dismiss();
			}
		});
    	
    	
    	/*Dialogbox anzeigen*/
    	verbindungs_dialog.show();
    }
    
    public void menu_button7_event(View view) {
    	NetworkModule.CloseConnection();
    	MainActivity.this.finish();
    }
    
    
}
