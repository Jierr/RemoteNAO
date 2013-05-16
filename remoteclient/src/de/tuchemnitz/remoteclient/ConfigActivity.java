package de.tuchemnitz.remoteclient;

import android.content.Intent;
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

/**
 * @file   ConfigActivity.java
 * @author Riko Streller
 * 
 * Creates a surface, to insert the IP-Adress for the connection to the robot
 *
 */
public class ConfigActivity extends SherlockActivity {

	private TextView text_verbindungsstatus; ///< Text that tells the current connection sate
	private EditText textfeld_ipeingabe; ///< String for holding the input of the ip-input-editfield
	/*Buttons*/
	private Button button_verbindenbutton;
	
	private MenuItem BatteryIcon;
	private MenuItem ConnectIcon;
	
	
	/**
	 * Loads/sets up the Activity layout and registers the callback.
	 */
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
    	case NetworkModule.CONN_CLOSED:
    		text_verbindungsstatus.setText("nicht verbunden");
        	text_verbindungsstatus.setTextColor(Color.RED);
        	break;
    	case NetworkModule.CONN_CONNECTING:
    		text_verbindungsstatus.setText("verbinden ...");
        	text_verbindungsstatus.setTextColor(Color.YELLOW);
        	break;
    	case NetworkModule.CONN_OPEN:
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
	
	/**
	 * Loads ActionBar, enables Back button and
	 * gets and refreshes Battery and Connection Icons.
	 */
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
	
    /**
     * Handles option menu clicks.
     * Shows other activities or closes the application.
     */
	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		super.onOptionsItemSelected(item);
		Intent intent;
		switch(item.getItemId()){
		case android.R.id.home:
			finish();
			break;
		case R.id.acb_m_1:
			intent = new Intent(Callbacksplit.getMainActivity(), BewegungActivity.class);
			finish();
			startActivity(intent);
			break;
		case R.id.acb_m_2:
			intent = new Intent(Callbacksplit.getMainActivity(), SprachausgabeActivity.class);
			finish();
			startActivity(intent);
			break;
		case R.id.acb_m_3:
			intent = new Intent(Callbacksplit.getMainActivity(), SpecialsActivity.class);
			finish();
			startActivity(intent);
			break;
		case R.id.acb_m_4:
			break;
		case R.id.acb_m_5:
			intent = new Intent(Callbacksplit.getMainActivity(), SettingActivity.class);
			finish();
			startActivity(intent);
			break;
		case R.id.acb_video:
			VideoModule.create_dialog(ConfigActivity.this, true);
			break;
		}
		
		
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
	
	/**
	 * Activates when the Connection Button is pressed.
	 * 
	 * @param view	ignored, ID of the button, which triggers this function
	 * 
	 * Opens or closes the network connection.
	 */
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
	
	/**
	 * Activates when the Close button is pressed.
	 * 
	 * @param view	ignored, ID of the button, which trigger this function
	 * 
	 * Closes the application.
	 */
	public void cfgbutton_close_event(View view)
	{
		finish();
	}	
	
	/************ Dynamisches Aenderugszeug *****************
     ********************************************************/
    
	/**
	 * Refreshes the text of the connection state and button.
	 * 
	 * @param state	Connection state (see NetworkModule.CONN_ constants)
	 */
    public void changeConnectionView(int state){
    	switch(state)
    	{
    	case NetworkModule.CONN_CLOSED:
    		text_verbindungsstatus.setText("nicht verbunden");
        	text_verbindungsstatus.setTextColor(Color.RED);
        	button_verbindenbutton.setText("verbinden");
        	break;
    	case NetworkModule.CONN_CONNECTING:
    		text_verbindungsstatus.setText("verbinden ...");
        	text_verbindungsstatus.setTextColor(Color.YELLOW);
        	break;
    	case NetworkModule.CONN_OPEN:
    		text_verbindungsstatus.setText("verbunden");
        	text_verbindungsstatus.setTextColor(Color.GREEN);
        	button_verbindenbutton.setText("trennen");
        	break;
    	}
    }
    
    /**
     * Sets the Battery Icon in the ActionBar.
     * 
     * @param pic	Drawable of the Battery Icon
     */
    public void setActBarBatteryIcon(Drawable pic){
    	if(pic!=null)
    		BatteryIcon.setIcon(pic);
    }
	
    /**
     * Sets the Connection Icon depending on the current connection state.
     */
    public void setActBarConnectIcon(){
    	if(ConnectIcon == null && BatteryIcon != null)
    		return;
    	
    	if(NetworkModule.IsConnected()==NetworkModule.CONN_CLOSED)
    	{
    		ConnectIcon.setIcon(R.drawable.network_disconnected);
    	}
    	else
    	{
    		ConnectIcon.setIcon(R.drawable.network_connected);
    	}
    }
	
}
