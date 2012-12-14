package de.tuchemnitz.remoteclient;

import java.util.*;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.res.Configuration;
import android.graphics.Color;
import android.util.Log;
import android.view.Gravity;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {

	private Timer BattTimer = null;
	private int bewegungsart = R.id.bewa_rbutton_LAUFEN;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
    	//NetworkModule.SetIPAddress("134.109.146.139");
        NetworkModule.SetIPAddress("134.109.151.142");
        menu_button5_event(null);
        
		BattTimer = new Timer();
		BattTimer.schedule(new BattTmrTask(), 1000, 5000);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_main, menu);
        return true;
    }
    
    /** Called when the user clicks the Send button */
    public void sendMessage(View view) {
        // Do something in response to button
    	/*boolean RetVal;
    	
    	RetVal = NetworkModule.OpenConnection(this);
    	if (! RetVal)
    	{
	    	new AlertDialog.Builder(this)
			.setMessage("Connection failed")
			.setNeutralButton("och", null)
			.show();
    	}*/
    	
    	NetworkModule.net_test();
    	//NetworkModule.CloseConnection();
    	/*new AlertDialog.Builder(this)
			.setMessage("Test done.")
			.setNeutralButton("och", null)
			.show();*/
    	
    	return;
    }
    
    /************** BEW_BUTTONS ******************
     *********************************************/
    
    public void bew_button1_event(View view) {
    	if(bewegungsart == R.id.bewa_rbutton_LAUFEN)
    	{
    		NetworkModule.Move(NetworkModule.MOVE_UP);
        	Toast toast = Toast.makeText(MainActivity.this, "vorwärts", Toast.LENGTH_SHORT);
        	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
        	toast.show();
    	}
    	else if(bewegungsart == R.id.bewa_rbutton_ARME)
    	{
    		NetworkModule.MoveArm(NetworkModule.MOVE_UP);
        	Toast toast = Toast.makeText(MainActivity.this, "Arm hoch", Toast.LENGTH_SHORT);
        	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
        	toast.show();
    	}
    	else
    	{
    		NetworkModule.MoveHead(NetworkModule.MOVE_UP);
    		Toast toast = Toast.makeText(MainActivity.this, "Kopf hoch", Toast.LENGTH_SHORT);
        	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
        	toast.show();
    	}
    	
    }
    
    public void bew_button2_event(View view) {
    	
    	if(bewegungsart == R.id.bewa_rbutton_LAUFEN)
    	{
    		NetworkModule.Move(NetworkModule.MOVE_LEFT);
        	Toast toast = Toast.makeText(MainActivity.this, "links", Toast.LENGTH_SHORT);
        	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
        	toast.show();
    	}
    	else if(bewegungsart == R.id.bewa_rbutton_ARME)
    	{
    		NetworkModule.MoveArm(NetworkModule.MOVE_LEFT);
        	Toast toast = Toast.makeText(MainActivity.this, "Arm links", Toast.LENGTH_SHORT);
        	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
        	toast.show();
    	}
    	else
    	{
    		NetworkModule.MoveHead(NetworkModule.MOVE_LEFT);
    		Toast toast = Toast.makeText(MainActivity.this, "Kopf links", Toast.LENGTH_SHORT);
        	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
        	toast.show();
    	}
    }
    
    public void bew_button3_event(View view) {
    	if(bewegungsart == R.id.bewa_rbutton_LAUFEN)
    	{
    		NetworkModule.Move(NetworkModule.MOVE_RIGHT);
        	Toast toast = Toast.makeText(MainActivity.this, "rechts", Toast.LENGTH_SHORT);
        	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
        	toast.show();
    	}
    	else if(bewegungsart == R.id.bewa_rbutton_ARME)
    	{
    		NetworkModule.MoveArm(NetworkModule.MOVE_RIGHT);
        	Toast toast = Toast.makeText(MainActivity.this, "Arm rechts", Toast.LENGTH_SHORT);
        	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
        	toast.show();
    	}
    	else
    	{
    		NetworkModule.MoveHead(NetworkModule.MOVE_RIGHT);
    		Toast toast = Toast.makeText(MainActivity.this, "Kopf rechts", Toast.LENGTH_SHORT);
        	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
        	toast.show();
    	}
    }
    
    public void bew_button4_event(View view) {
    	if(bewegungsart == R.id.bewa_rbutton_LAUFEN)
    	{
    		NetworkModule.Move(NetworkModule.MOVE_DOWN);
        	Toast toast = Toast.makeText(MainActivity.this, "rückwärts", Toast.LENGTH_SHORT);
        	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
        	toast.show();
    	}
    	else if(bewegungsart == R.id.bewa_rbutton_ARME)
    	{
    		NetworkModule.MoveArm(NetworkModule.MOVE_DOWN);
        	Toast toast = Toast.makeText(MainActivity.this, "Arm unten", Toast.LENGTH_SHORT);
        	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
        	toast.show();
    	}
    	else
    	{
    		NetworkModule.MoveHead(NetworkModule.MOVE_DOWN);
    		Toast toast = Toast.makeText(MainActivity.this, "Kinn rann", Toast.LENGTH_SHORT);
        	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
        	toast.show();
    	}
    }
    
    /********************** Menu *************************
     *****************************************************/
    public void menu_button1_event(View view) {
    	final Dialog bewa_dialog = new Dialog(MainActivity.this);
    	bewa_dialog.setContentView(R.layout.bewegungsauswahl);
    	bewa_dialog.setTitle("Bewegungsauswahl");
    	((RadioGroup) bewa_dialog.findViewById(R.id.bew_radioGroup1)).check(bewegungsart);
    	/*bewa_radiogroup.setOnCheckedChangeListener(new OnCheckedChangeListener() {
			
			@Override
			public void onCheckedChanged(RadioGroup group, int checkedId) {
				Toast toast = Toast.makeText(MainActivity.this, "checked: " + Integer.toString(checkedId), Toast.LENGTH_SHORT);
		    	toast.setGravity(Gravity.TOP | Gravity.RIGHT, 0, 0);
		    	toast.show();
			}
		});*/
    	
    	/* ------------ Funktion bei OK Button ------------ */
    	Button dial_button_ok = (Button) bewa_dialog.findViewById(R.id.bewa_OKbutton);
    	dial_button_ok.setOnClickListener(new OnClickListener() {
    		@Override
			public void onClick(View view) {
    			bewa_dialog.dismiss();
    			final RadioGroup bewa_radiogroup = (RadioGroup)bewa_dialog.findViewById(R.id.bew_radioGroup1);
    			int checkedId = bewa_radiogroup.getCheckedRadioButtonId();
    			
    			bewegungsart = checkedId;
    		}
    	});
    	/*Dialogbox anzeigen*/
    	bewa_dialog.show();
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
    
    public void menu_button3_event(View view) {
    	String sit_status = null;
    	sit_status = NetworkModule.SitToggle();
    	if (sit_status == "STAND")
    		((Button)findViewById(R.id.menu_button3)).setText("setzen");
    	else
    		((Button)findViewById(R.id.menu_button3)).setText("aufstehen");
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
		    	ip_word = ip_word.trim();
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
    	BattTimer.cancel();
    	NetworkModule.CloseConnection();
    	MainActivity.this.finish();
    }
    
	class BattTmrTask extends TimerTask
	{
		final ImageView battery_view = (ImageView)findViewById(R.id.img_bat);
		
		private Handler updateUI = new Handler(){
			@Override
			public void dispatchMessage(Message msg) {
			    super.dispatchMessage(msg);
			    if(msg.what > 90)
			    {
			    	battery_view.setImageDrawable(getResources().getDrawable(R.drawable.bat100));
			    }
			    else if (msg.what <= 90 && msg.what > 65) {
			    	battery_view.setImageDrawable(getResources().getDrawable(R.drawable.bat75));
				}
			    else if (msg.what <= 65 && msg.what > 35) {
			    	battery_view.setImageDrawable(getResources().getDrawable(R.drawable.bat50));
				}
			    else if (msg.what <= 35 && msg.what > 10) {
			    	battery_view.setImageDrawable(getResources().getDrawable(R.drawable.bat25));
				}
			    else
			    {
			    	battery_view.setImageDrawable(getResources().getDrawable(R.drawable.bat0));
			    }
			}
			
		};
		
		public void run()  
		{
			if (! NetworkModule.IsConnected())
				return;
			
			int BattState;
			
			//System.out.println("Make my day.");
			//NetworkModule.Speak(MainActivity.NOTIFICATION_SERVICE);
			BattState = NetworkModule.GetBatteryState();
			updateUI.sendEmptyMessage(BattState);
		}
	}
}
