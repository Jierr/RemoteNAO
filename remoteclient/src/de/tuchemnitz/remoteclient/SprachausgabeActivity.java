package de.tuchemnitz.remoteclient;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.widget.EditText;

public class SprachausgabeActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_sprachausgabe);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_sprachausgabe, menu);
		return true;
	}
	
	
	public void okbutton_event(View view){
		EditText textfeld = (EditText)findViewById(R.id.spk_editText1);
    	String words = textfeld.getText().toString();
    	NetworkModule.Speak(words);
    	textfeld.setText(null);
    	finish();
    	
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
