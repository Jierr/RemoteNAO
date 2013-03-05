package de.tuchemnitz.remoteclient;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.app.Dialog;
import android.content.Context;
import android.content.Intent;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;

import com.actionbarsherlock.app.SherlockActivity;
import com.actionbarsherlock.view.Menu;
import com.actionbarsherlock.view.MenuItem;

/**
 * @file   SprachausgabeActivity.java
 * 
 * @author Riko Streller
 * 
 * Creates a surface with a editable field for text, which the robot should speak
 *
 */
public class SprachausgabeActivity extends SherlockActivity {

	private MenuItem BatteryIcon;
	private MenuItem ConnectIcon;
	
	/**
	 * Loads the Activity layout and registers the callback.
	 */
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
        
        ((MenuItem)menu.findItem(R.id.acb_m_3)).setVisible(false);
        
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
	
	
	/**
	 * Makes the Networkmodule send the text from the editfield as speakmessage to the robot
	 * 
	 * @param view		ignored, ID of the button, which trigger this function
	 *  
	 */
	public void okbutton_event(View view){
		EditText textfeld = (EditText)findViewById(R.id.spk_editText1);
    	String words = textfeld.getText().toString();
    	NetworkModule.Speak(words);
    	textfeld.setText(null);
	}
	
	
    /************ Dynamisches Aenderugszeug *****************
     ********************************************************/
    
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
	 * Refreshes the ActionBar's network state icon.
	 */
	public void setActBarConnectIcon(){
    	if(NetworkModule.IsConnected()==NetworkModule.CONN_CLOSED)
    	{
    		ConnectIcon.setIcon(R.drawable.network_disconnected);
    	}
    	else
    	{
    		ConnectIcon.setIcon(R.drawable.network_connected);
    	}
    }
	
	/**
	 * Funktion for opening a save/load text dialog
	 * 
	 * @param view		ignored, ID of the button, which trigger this function
	 *  
	 * Opens a dialog, where text from the editfield in the activity can be saved or loaded
	 * to a specified place declared by number
	 */
	public void saveload(View view){
		/* Dialogbox erstellen*/
		final Dialog spk_dialog = new Dialog(SprachausgabeActivity.this);
		spk_dialog.setContentView(R.layout.dialog_spk_saveloadtext);
		spk_dialog.setTitle("Textspeicher");
		
		final TextView textanzeige = (TextView) spk_dialog.findViewById(R.id.spkdial_textvorschau);
		textanzeige.setText(new FileSave().loadshort(1));
		
		final EditText numberfield = (EditText) spk_dialog.findViewById(R.id.spkdial_editText_num);
		
		/**
		 * Setup the Listener for input in the numberfield
		 * 
		 */
		numberfield.setOnEditorActionListener(new OnEditorActionListener() {
			@Override
			public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
				final EditText numberfield = (EditText) spk_dialog.findViewById(R.id.spkdial_editText_num);
				int i = Integer.valueOf((String)numberfield.getText().toString());
				if(i>99) i = 99;
				else if(i<1) i=1;
				numberfield.setText(String.valueOf(i) );
				final TextView textanzeige = (TextView) spk_dialog.findViewById(R.id.spkdial_textvorschau);
				textanzeige.setText(new FileSave().loadshort(i));
				return false;
			}
		});
		

		/* ------------ Funktion bei - Button ------------ */
		Button Button_Minus = (Button) spk_dialog.findViewById(R.id.spkdial_minus);
		/**
		 * Setup the Listener for the minusbutton
		 * 
		 * On click decreases the number of the storage space
		 * 
		 */
		Button_Minus.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				final EditText numberfield = (EditText) spk_dialog.findViewById(R.id.spkdial_editText_num);
				int i = Integer.valueOf((String)numberfield.getText().toString()) - 1;
				if(i>99) i = 1;
				else if(i<1) i=99;
				numberfield.setText(String.valueOf(i) );
				final TextView textanzeige = (TextView) spk_dialog.findViewById(R.id.spkdial_textvorschau);
				textanzeige.setText(new FileSave().loadshort(i));
				
			}
		});
		/* ------------ Funktion bei + Button ------------ */
		Button Button_Plus = (Button) spk_dialog.findViewById(R.id.spkdial_plus);
		/**
		 * Setup the Listener for the plusbutton
		 * 
		 * On click increases the number of the storage space
		 * 
		 */
		Button_Plus.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				final EditText numberfield = (EditText) spk_dialog.findViewById(R.id.spkdial_editText_num);
				int i = Integer.valueOf((String)numberfield.getText().toString()) + 1;
				if(i>99) i = 1;
				else if(i<1) i=99;
				numberfield.setText(String.valueOf(i) );
				final TextView textanzeige = (TextView) spk_dialog.findViewById(R.id.spkdial_textvorschau);
				textanzeige.setText(new FileSave().loadshort(i));
				
			}
		});
		Button Button_Save = (Button) spk_dialog.findViewById(R.id.spkdial_save);
		/**
		 * By pressing the related buttton this function saves the text in 
		 * the editfield of the activity, by writing it to the file on the line declared by storage space
		 * 
		 */
		Button_Save.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				final EditText textfeld = (EditText)findViewById(R.id.spk_editText1);
		    	String words = textfeld.getText().toString();
		    	final EditText numberfield = (EditText) spk_dialog.findViewById(R.id.spkdial_editText_num);
				int i = Integer.valueOf((String)numberfield.getText().toString());
		    	new FileSave().save(words, i);
		    	
		    	spk_dialog.dismiss();
			}
		});
		Button Button_load = (Button) spk_dialog.findViewById(R.id.spkdial_load);
		/**
		 * By pressing the related buttton this function loads the text from the line, 
		 * declared by storage space, in savefile to the editfield of the activity
		 * 
		 */
		Button_load.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				final EditText textfeld = (EditText) findViewById(R.id.spk_editText1);
		    	final EditText numberfield = (EditText) spk_dialog.findViewById(R.id.spkdial_editText_num);
				int i = Integer.valueOf((String)numberfield.getText().toString());
		    	String sprachtext = new FileSave().load(i);
		    	textfeld.setText(sprachtext);
		    	
		    	spk_dialog.dismiss();
			}
		});	
		Button Button_close = (Button) spk_dialog.findViewById(R.id.spkdial_close);
		/**
		 * By pressing the related buttton the save / load dialog will be closed
		 * 
		 */
		Button_close.setOnClickListener(new OnClickListener() {			
			@Override
			public void onClick(View v) {		    	
		    	spk_dialog.dismiss();
			}
		});
		
		
		/*Dialogbox anzeigen*/
		spk_dialog.show();		
	}
	
	/**
	* @class FileSave
	*
	* Provides access to a file for saving data
	*/ 
	class FileSave {
		
		FileOutputStream fos = null;
		FileInputStream fis = null;
		
		private String[] readfile(){
			
			String collected = null;
			
			try {
				fis = openFileInput("spk_textfile");
				byte[] readData = new byte [fis.available()];
				while(fis.read(readData) != -1)
				{
					collected = new String(readData);
				}
			}
			catch (FileNotFoundException e)
			{
				e.printStackTrace();
				collected = null;
				fis = null;
			}
			catch (IOException e)
			{
				e.printStackTrace();
				collected = null;
			}
			finally
			{
				if(fis != null)
				{
					try{
						fis.close();
					}
					catch (IOException e)
					{
						e.printStackTrace();
					}
					fis = null;
				}
			}
			
			if(collected==null)
				return null;
			else
				return collected.split("\r#\n");
		}
		
		
		public String load( int pos)
		{
			String[] text = readfile();
			if(text == null || text.length<pos )
				return "nichts gespeichert";
			else
				return text[pos-1];
		}
		
		public String loadshort(int pos)
		{
			String text = load(pos);
			if(text.length() < 23 )
				return (text.substring(0, text.length())).replace('\n', ' ');
			else 
				return ((text.substring(0, 22).replace('\n', ' '))+"...");
		}
		
		public void save(String neutext, int pos)
		{
			String[] texte = readfile();
			
			if( texte==null)
			{
				texte = new String[100];
				for( int i=0; i<100; i++)
				{
					texte[i]="frei";
				}
			}
			
			if(texte.length < pos)
			{
				String[] replacetexte = new String[100];
				int i = 0;
				for(i=0; i<texte.length; i++)
				{
						replacetexte[i]=texte[i];
				}
				for(i=texte.length; i<100; i++)
				{
					replacetexte[i]="frei";
				}
				texte=replacetexte;
			}
			
			texte[pos-1] = neutext;
			
			try {
				fos = openFileOutput("spk_textfile", Context.MODE_PRIVATE);
				for(int i=0; i<texte.length; i++)
				{
					fos.write( (texte[i]+"\r#\n").getBytes() );
				}
				fos.close();
			}
			catch(FileNotFoundException e){
				e.printStackTrace();
			}
			catch(IOException e)
			{
				e.printStackTrace();
			}
			
		}
		
	}
}


