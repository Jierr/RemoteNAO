package de.tuchemnitz.remoteclient;

import android.app.Activity;
import android.app.AlertDialog;
import android.graphics.drawable.Drawable;

/**
 * @file	Callbacksplit.java
 * @author Riko Streller
 *
 * Does the communication between the Activities.
 * 
 */
public class Callbacksplit {
	
	private static MainActivity MainAct = null;
	private static ConfigActivity ConfAct = null;
	private static BewegungActivity BewgAct = null;
	private static SpecialsActivity SpecAct = null;
	private static SprachausgabeActivity SpakAct = null;
	private static Activity active_Activity = null;
	private static Drawable BatteryState = null;
	
	/**
	 * Registers the current MainActivity instance.
	 * 
	 * @param act	current MainActivity instance
	 */
	public static void registerMainActivity(MainActivity act){
		MainAct = act;
	}
	
	/**
	 * Returns the current MainActivity instance.
	 * 
	 * @return	current registered MainActivity instance
	 */
	public static MainActivity getMainActivity(){
		return MainAct;
	}
	
	/**
	 * Registers the current ConfigActivity instance.
	 * 
	 * @param act	current ConfigActivity instance
	 */
	public static void registerConfigActivity(ConfigActivity act){
		ConfAct = act;
	}
	
	/**
	 * Returns the current ConfigActivity instance.
	 * 
	 * @return	current registered ConfigActivity instance
	 */
	public static ConfigActivity getConfigActivity(){
		return ConfAct;
	}
	
	/**
	 * Registers the current BewegungActivity instance.
	 * 
	 * @param act	current BewegungActivity instance
	 */
	public static void registerBewegungActivity(BewegungActivity act){
		BewgAct = act;
	}
	
	/**
	 * Returns the current BewegungActivity instance.
	 * 
	 * @return	current registered BewegungActivity instance
	 */
	public static BewegungActivity getBewegungActivity(){
		return BewgAct;
	}
	
	/**
	 * Registers the current SpecialsActivity instance.
	 * 
	 * @param act	current SpecialsActivity instance
	 */
	public static void registerSpecialsActivity(SpecialsActivity act){
		SpecAct = act;
	}
	
	/**
	 * Returns the current SpecialsActivity instance.
	 * 
	 * @return	current registered SpecialsActivity instance
	 */
	public static SpecialsActivity getSpecialsActivity(){
		return SpecAct;
	}
	
	/**
	 * Registers the current SprachausgabeActivity act instance.
	 * 
	 * @param act	current SprachausgabeActivity act instance
	 */
	public static void registerSprachausgabeActivity(SprachausgabeActivity act){
		SpakAct = act;
	}
	
	/**
	 * Returns the current SprachausgabeActivity act instance.
	 * 
	 * @return	current registered SprachausgabeActivity act instance
	 */
	public static SprachausgabeActivity getSprachausgabeActivity(){
		return SpakAct;
	}
	
	/**
	 * Sets the currently active Activity.
	 * 
	 * @param aact	Activity
	 */
	public static void setActiveActivity(Activity aact){
		active_Activity = aact;
	}
	
	/**
	 * Unsets the active Activity.
	 */
	public static void unsetActiveActivity(){
		active_Activity = null;
	}
	
	/**
	 * Shows the "Connection Lost" dialog on the active Activity.
	 */
	public static void showConnectionLostDialog(){
		if(active_Activity != null)
		{
			new AlertDialog.Builder(active_Activity)
			.setMessage("Verbindung verloren.")
			.setNeutralButton("Mist", null)
			.show();
		}
	}
	
	/**
	 * Sets the Battery State Icon.
	 * 
	 * @param bat	Drawable of the icon image.
	 */
	public static void saveBatteryStateIcon(Drawable bat){
		BatteryState = bat;
	}
	
	/**
	 * Returns the saved Battery State Icon.
	 * 
	 * @return	Drawable of the saved Battery State Icon
	 */
	public static Drawable getsavedBatteryStateIcon(){
		return BatteryState;
	}
	
	/**
	 * Sets the Battery Icon in the ActionBar in the currently active Activity.
	 * 
	 * @param batt_icon_r	Drawable of the Battery Icon
	 */
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
	
	/**
	 * Sets the saved Battery Icon in the ActionBar in the currently active Activity.
	 */
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
