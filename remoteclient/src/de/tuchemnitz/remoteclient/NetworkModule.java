package de.tuchemnitz.remoteclient;

import java.io.*;
import java.net.Socket;
import java.util.Random;

import android.app.Activity;
import android.app.AlertDialog;
import android.util.Log;

public class NetworkModule {

	private static final String TERMINATE_CHR = "\0";
	private static String IP_Addr = null;
	private static final int Port = 0x8000;
	private static Socket Client = null;
	private static OutputStream outToServer;
	private static InputStream inFromServer;
	public static final char MOVE_UP = 'U';
	public static final char MOVE_DOWN = 'D';
	public static final char MOVE_LEFT = 'L';
	public static final char MOVE_RIGHT = 'R';
	private static Activity RefActivity;
	
	public static void SetIPAddress(String IP_Str)
	{
		IP_Addr = IP_Str;
		
		return;
	}
	
	public static String GetIPAddress()
	{
			return IP_Addr;
	}
	
	public static void Move(boolean Start, char Direction)
	{
		if (Client == null || ! Client.isConnected())
			return;
		
		String CommandStr;
		
		CommandStr = "MOV";
		CommandStr += "_";
		CommandStr += Start ? "1" : "0";
		CommandStr += "_";
		CommandStr += String.valueOf(Direction);
		SendCommand(CommandStr);
		
		return;
	}
	
	public static void Speak(String Text)
	{
		if (Client == null || ! Client.isConnected())
			return;
		
		String CommandStr;
		
		CommandStr = "SPK";
		CommandStr += "_";
		CommandStr += Text;
		SendCommand(CommandStr);
		
		return;
	}
	
	public static int GetBatteryState()
	{
		if (Client == null || ! Client.isConnected())
			return 0;
		
		return (int)(Math.random()*100);
	}
	
	public static boolean OpenConnection(Activity MainAct)
	{
		if (MainAct != null)
			RefActivity = MainAct;
		
		try
		{
			Log.v("NetMod", "Connecting to " + IP_Addr + " on port " + Port);
			Client = new Socket(IP_Addr, Port);
			Client.setSoTimeout(1000);
			Log.v("NetMod", "Just connected to " + Client.getRemoteSocketAddress());
			
			outToServer = Client.getOutputStream();
			inFromServer = Client.getInputStream();
			
			Log.v("NetMod", "Connection open successful.");
			return true;
		}
		catch(IOException e)
		{
			e.printStackTrace();
			Log.v("NetMod", "Connection open failed.");
			try
			{
				if (Client != null)
					Client.close();
			}
			catch(IOException e1)
			{
				// do nothing
			}
			
			return false;
		}
		catch(Exception e)
		{
			e.printStackTrace();
			Log.v("NetMod", "Unexpected Error.");
			try
			{
				if (Client != null)
					Client.close();
			}
			catch(IOException e1)
			{
				// do nothing
			}
			
			return false;
		}
	}
	
	public static boolean CloseConnection()
	{
		if (Client == null || ! Client.isConnected())
			return true;
		
		try
		{
			DataOutputStream out = new DataOutputStream(outToServer);
			out.writeBytes("#" + TERMINATE_CHR);
			
			Client.close();
			Log.v("NetMod", "Connection closed.");
		}
		catch(IOException e)
		{
			e.printStackTrace();
			Log.v("NetMod", "Connection close failed.");
			return false;
		}
		/*Client = null;
		try
		{
			Client.close();
		}
		catch(IOException e)
		{
		}*/
		
		return true;
	}
	
	private static void SendCommand(String CommandStr)
	{
		if (Client == null || ! Client.isConnected())
			return;
		
		Log.v("NetMod.Send", "isConnected: " + (Client.isConnected() ? "true" : "false"));
		Log.v("NetMod.Send", "isClosed: " + (Client.isClosed() ? "true" : "false"));
		Log.v("NetMod.Send", "isBound: " + (Client.isBound() ? "true" : "false"));
		int TryResend;
		
		TryResend = 2;
		while(TryResend > 0)
		{
			try
			{
				DataOutputStream out = new DataOutputStream(outToServer);
				
				out.writeBytes(CommandStr + TERMINATE_CHR);
				TryResend = 0;
			}
			catch(IOException e)
			{
				e.printStackTrace();
				
				Log.v("NetMod", "Send error. Reconnecting ...");
				CloseConnection();
				if (! OpenConnection(null))
				{
					Log.v("NetMod", "Reconnect failed.");
			    	new AlertDialog.Builder(RefActivity)
						.setMessage("Connection lost.")
						.setNeutralButton("bummer", null)
						.show();
					break;
				}
				TryResend --;
			}
		}
		
		return;
	}
	
	public static boolean IsConnected()
	{
		if (Client == null || Client.isClosed())
			return false;
		
		Log.v("NetMod.IsC", "isConnected: " + (Client.isConnected() ? "true" : "false"));
		Log.v("NetMod.IsC", "isClosed: " + (Client.isClosed() ? "true" : "false"));
		Log.v("NetMod.IsC", "isBound: " + (Client.isBound() ? "true" : "false"));
		
		// Send dummy data
		try
		{
			DataOutputStream out = new DataOutputStream(outToServer);
			
			out.writeBytes(" " + TERMINATE_CHR);
		}
		catch(IOException e)
		{
			e.printStackTrace();
			Log.v("NetMod", "Send error. Reconnecting ...");
			try
			{
				Client.close();
			}
			catch(IOException e2)
			{
			}
		}
		return Client.isConnected();
	}
	
	public static void net_test()
	{
		//DataOutputStream out = new DataOutputStream(outToServer);
		//DataInputStream in = new DataInputStream(inFromServer);
		
		SendCommand("Hello World");
		/*System.out.println("Server says " + in.readLine());
		while(in.available() > 0)
			System.out.println("\t" + in.readLine());*/
		//out.writeBytes("QUIT\r\n\r\n");
		
		return;
	}

}
