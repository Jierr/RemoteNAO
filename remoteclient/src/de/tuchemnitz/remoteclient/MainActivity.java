package de.tuchemnitz.remoteclient;

import android.os.Bundle;
import android.app.Activity;
import android.app.AlertDialog;
import android.view.Menu;
import android.view.View;

public class MainActivity extends Activity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_main, menu);
        return true;
    }
    
    /** Called when the user clicks the Send button */
    public void sendMessage(View view) {
        // Do something in response to button
    	new AlertDialog.Builder(this)
		.setMessage("Not Implemented yet")
		.setNeutralButton("och", null)
		.show();
    	return;
    }
}
