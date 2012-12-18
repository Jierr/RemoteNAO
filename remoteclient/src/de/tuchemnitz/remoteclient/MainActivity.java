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
	private Dialog verbindungs_dialog = null;
	private final int EVENT_BATT = 0;
	private final int EVENT_SIT = 1;
	private final int EVENT_CONN = 2;
	
	private Handler EvtHandler = new Handler()
	{
		@Override public void dispatchMessage(Message msg)
		{
		    //super.dispatchMessage(msg);
		    
		    switch(msg.what)
		    {
		    case EVENT_BATT:
				final ImageView battery_view = (ImageView)findViewById(R.id.img_bat);
				final int batt_pic;
				
			    if (msg.arg1 > 90)
			    	batt_pic = R.drawable.bat100;
			    else if (msg.arg1 <= 90 && msg.arg1 > 65)
			    	batt_pic = R.drawable.bat75;
			    else if (msg.arg1 <= 65 && msg.arg1 > 35)
			    	batt_pic = R.drawable.bat50;
			    else if (msg.arg1 <= 35 && msg.arg1 > 10)
			    	batt_pic = R.drawable.bat25;
			    else
			    	batt_pic = R.drawable.bat0;
			    battery_view.setImageDrawable(getResources().getDrawable(batt_pic));
			    break;
		    case EVENT_SIT:
				final Button sit_text = (Button)findViewById(R.id.menu_button3);
		    	final String SitStatus = (String)msg.obj;
		    	
		    	if (SitStatus.equals("STAND"))
		    		sit_text.setText("Setzen");
		    	else
		    		sit_text.setText("Aufstehen");
		    	break;
		    case EVENT_CONN:
		    	Log.v("MainAct.Event", "Connection Status: " + String.valueOf(msg.arg1));
		    	if (msg.arg1 < 0)
		    	{
		    		switch(msg.arg1)
		    		{
		    		case -1:
			    		new AlertDialog.Builder(MainActivity.this)
							.setMessage("Verbindung verloren.")
							.setNeutralButton("Mist", null)
							.show();
			    		break;
		    		}
		    	}
		    	else
		    	{
			    	if (verbindungs_dialog == null)
			    		return;
			    	
			    	final TextView text_verbindungsstatus = (TextView)verbindungs_dialog.findViewById(R.id.verbindungsstatus_wert);
			    	
			    	switch(msg.arg1)
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
			        	break;
			    	}
		    	}
		    	break;
		    }
		    
		    return;
		}
	};
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
    	//NetworkModule.SetIPAddress("134.109.97.52");
        NetworkModule.SetIPAddress("134.109.151.142");
        //NetworkModule.SetIPAddress("192.168.5.20");
        menu_button6_event(null);
        
        NetworkModule.RegisterCallback(EvtHandler,	EVENT_BATT,	NetworkModule.INFO_BATT);
        NetworkModule.RegisterCallback(EvtHandler,	EVENT_SIT,	NetworkModule.INFO_SIT);
        NetworkModule.RegisterCallback(EvtHandler,	EVENT_CONN,	NetworkModule.INFO_CONN);
        
        Log.v("MainAct", "Activity started.");
		BattTimer = new Timer();
		BattTimer.schedule(new BattTmrTask(EvtHandler), 1000, 10000);
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
    	
    	RetVal = NetworkModule.OpenConnection();
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
    		ToastStr = "Kopf hoch";
    		break;
    	}
		NetworkModule.Move(MoveType, NetworkModule.MOVE_UP);
    	Toast toast = Toast.makeText(MainActivity.this, ToastStr, Toast.LENGTH_SHORT);
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
    	Toast toast = Toast.makeText(MainActivity.this, ToastStr, Toast.LENGTH_SHORT);
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
    	Toast toast = Toast.makeText(MainActivity.this, ToastStr, Toast.LENGTH_SHORT);
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
    		ToastStr = "Kopf runter";
    		break;
    	}
		NetworkModule.Move(MoveType, NetworkModule.MOVE_DOWN);
    	Toast toast = Toast.makeText(MainActivity.this, ToastStr, Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    }
    
    public void bew_button5_event(View view) {
    	
    	NetworkModule.Stop();
    	Toast toast = Toast.makeText(MainActivity.this, "STOP", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
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
    	NetworkModule.SitToggle();
    }
    
    public void menu_button4_event(View view) {
    	Toast toast = Toast.makeText(MainActivity.this, "Motoren entspannen", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    	NetworkModule.Rest();    	
    }
    
    public void menu_button5_event(View view) {
    	Toast toast = Toast.makeText(MainActivity.this, "Tanz", Toast.LENGTH_SHORT);
    	toast.setGravity(Gravity.BOTTOM|Gravity.RIGHT, 0, 0);
    	toast.show();
    	NetworkModule.Dance("Rumba");
    }
    
    
    public void menu_button6_event(View view) {
    	/* Dialogbox erstellen*/
    	verbindungs_dialog = new Dialog(MainActivity.this);
    	verbindungs_dialog.setContentView(R.layout.netzwerkverbindung);
    	verbindungs_dialog.setTitle("Netzwerkverbindung");
    	
    	final TextView text_verbindungsstatus = (TextView)verbindungs_dialog.findViewById(R.id.verbindungsstatus_wert);
    	final EditText textfeld_ipeingabe = (EditText)verbindungs_dialog.findViewById(R.id.verbindungsip_eingabe);
    	
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
        	break;
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
		});
    	/* schließen */
    	dial_button_close.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				verbindungs_dialog.dismiss();
				verbindungs_dialog = null;
			}
		});
    	
    	
    	/*Dialogbox anzeigen*/
    	verbindungs_dialog.show();
    }
    
    public void menu_button7_event(View view) {
    	new AlertDialog.Builder(this)
		.setMessage("Das ist eine App um den NAO zu steuern")
		.setNeutralButton("Wer weiss?", null)
		.show();
    }
    
    public void menu_button8_event(View view) {
    	BattTimer.cancel();
    	NetworkModule.CloseConnection();
    	MainActivity.this.finish();
    }
    
	class BattTmrTask extends TimerTask
	{
		private Handler updateUI/* = new Handler(){
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
		}*/;
		
		BattTmrTask(Handler EvtHandler)
		{
			updateUI = EvtHandler;
			
			return;
		}
		
		public void run()  
		{
			/*if (NetworkModule.IsConnected() != 1)
				return;
			
			int BattState;
			
			//System.out.println("Make my day.");
			//NetworkModule.Speak(MainActivity.NOTIFICATION_SERVICE);
			BattState = NetworkModule.GetBatteryState();
			updateUI.sendEmptyMessage(BattState);*/
			
			NetworkModule.RequestBatteryState();
		}
	}
}
