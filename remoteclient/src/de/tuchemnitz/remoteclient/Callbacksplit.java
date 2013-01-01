package de.tuchemnitz.remoteclient;

import android.app.Activity;
import android.app.AlertDialog;
import android.graphics.drawable.Drawable;

public class Callbacksplit {
	
	private static MainActivity MainAct = null;
	private static ConfigActivity ConfAct = null;
	private static BewegungActivity BewgAct = null;
	private static SpecialsActivity SpecAct = null;
	private static SprachausgabeActivity SpakAct = null;
	private static Activity active_Activity = null;
	private static Drawable BatteryState = null;
	
	public static void registerMainActivity(MainActivity act){
		MainAct = act;
	}
	
	public static MainActivity getMainActivity(){
		return MainAct;
	}
	
	public static void registerConfigActivity(ConfigActivity act){
		ConfAct = act;
	}
	
	public static ConfigActivity getConfigActivity(){
		return ConfAct;
	}
	
	public static void registerBewegungActivity(BewegungActivity act){
		BewgAct = act;
	}
	
	public static BewegungActivity getBewegungActivity(){
		return BewgAct;
	}
	
	public static void registerSpecialsActivity(SpecialsActivity act){
		SpecAct = act;
	}
	
	public static SpecialsActivity getSpecialsActivity(){
		return SpecAct;
	}
	
	public static void registerSprachausgabeActivity(SprachausgabeActivity act){
		SpakAct = act;
	}
	
	public static SprachausgabeActivity getSprachausgabeActivity(){
		return SpakAct;
	}
	
	public static void setActiveActivity(Activity aact)
	{
		active_Activity = aact;
	}
	
	public static void unsetActiveActivity()
	{
		active_Activity = null;
	}
	
	public static void showConnectionLostDialog(){
		if(active_Activity != null)
		{
			new AlertDialog.Builder(active_Activity)
			.setMessage("Verbindung verloren.")
			.setNeutralButton("Mist", null)
			.show();
		}
	}
	
	public static void saveBatteryStateIcon(Drawable bat){
		BatteryState = bat;
	}
	
	public static Drawable getsavedBatteryStateIcon(){
		return BatteryState;
	}
	
	
	public static void setActBarBatteryIcon(Drawable batt_icon_r){
		if(Callbacksplit.getBewegungActivity()!= null)
		    Callbacksplit.getBewegungActivity().setActBarBatteryIcon(batt_icon_r);
	    if(Callbacksplit.getConfigActivity()!= null)
		    Callbacksplit.getConfigActivity().setActBarBatteryIcon(batt_icon_r);
	    if(Callbacksplit.getSpecialsActivity()!= null)
		    Callbacksplit.getSpecialsActivity().setActBarBatteryIcon(batt_icon_r);
	    if(Callbacksplit.getSprachausgabeActivity()!= null)
		    Callbacksplit.getSprachausgabeActivity().setActBarBatteryIcon(batt_icon_r);
	}
	
	public static void setActBarConnectIcon(){
		
		Callbacksplit.getMainActivity().setActBarConnectIcon();
		    
		if(Callbacksplit.getBewegungActivity()!= null)
		    Callbacksplit.getBewegungActivity().setActBarConnectIcon();
	    if(Callbacksplit.getConfigActivity()!= null)
		    Callbacksplit.getConfigActivity().setActBarConnectIcon();
	    if(Callbacksplit.getSpecialsActivity()!= null)
		    Callbacksplit.getSpecialsActivity().setActBarConnectIcon();
	    if(Callbacksplit.getSprachausgabeActivity()!= null)
		    Callbacksplit.getSprachausgabeActivity().setActBarConnectIcon();
	}
	
	
	
	
}