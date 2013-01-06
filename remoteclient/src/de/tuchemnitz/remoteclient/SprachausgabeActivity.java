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
	
	
	public void saveload(View view){
		/* Dialogbox erstellen*/
		final Dialog spk_dialog = new Dialog(SprachausgabeActivity.this);
		spk_dialog.setContentView(R.layout.dialog_spk_saveloadtext);
		spk_dialog.setTitle("Textspeicher");
		
		final TextView textanzeige = (TextView) spk_dialog.findViewById(R.id.spkdial_textvorschau);
		textanzeige.setText(new FileSave().loadshort(1));
		
		final EditText numberfield = (EditText) spk_dialog.findViewById(R.id.spkdial_editText_num);
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
		Button_close.setOnClickListener(new OnClickListener() {			
			@Override
			public void onClick(View v) {		    	
		    	spk_dialog.dismiss();
			}
		});
		
		
		/*Dialogbox anzeigen*/
		spk_dialog.show();		
	}
	
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
				return collected.split("\n");
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
				return text.substring(0, text.length());
			else 
			 return (text.substring(0, 22)+"...");
		}
		
		public void save(String neutext, int pos)
		{
			String[] texte = readfile();
			
			if( texte==null )
			{
				texte = new String[100];
				for( int i=0; i<100; i++)
				{
					texte[i]="frei";
				}
			}
			
			texte[pos-1] = neutext;
			
			try {
				fos = openFileOutput("spk_textfile", Context.MODE_PRIVATE);
				for(int i=0; i<texte.length; i++)
				{
					fos.write( (texte[i]+"\n").getBytes() );
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


