package de.tuchemnitz.remoteclient;

import java.io.*;
import java.net.Socket;
import android.util.Log;

public class NetworkModule {

	private static String IP_Addr = null;
	private static final int Port = 0x8000;
	private static Socket Client = null;
	private static OutputStream outToServer;
	private static InputStream inFromServer;
	public static final char MOVE_UP = 'U';
	public static final char MOVE_DOWN = 'D';
	public static final char MOVE_LEFT = 'L';
	public static final char MOVE_RIGHT = 'R';
	
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
		if (Client == null)
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
		if (Client == null)
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
		if (Client == null)
			return 0;
		
		return 0;
	}
	
	public static boolean OpenConnection()
	{
		try
		{
			Log.v("NetMod", "Connecting to " + IP_Addr + " on port " + Port);
			Client = new Socket(IP_Addr, Port);
			Log.v("NetMod", "Just connected to " + Client.getRemoteSocketAddress());
			
			outToServer = Client.getOutputStream();
			inFromServer = Client.getInputStream();
			
			Log.v("NetMod", "Connection open successful.");
			return true;
		}
		catch(Exception e)
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
			Client = null;
			
			return false;
		}
	}
	
	public static boolean CloseConnection()
	{
		if (Client == null)
			return true;
		
		try
		{
			SendCommand("#");
			Client.close();
			Client = null;
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
		if (Client == null)
			return;
		
		int TryResend;
		
		TryResend = 2;
		while(TryResend > 0)
		{
			try
			{
				DataOutputStream out = new DataOutputStream(outToServer);
				
				out.writeBytes(CommandStr + "+");
				//out.writeBytes(CommandStr + "\n");
				TryResend = 0;
			}
			catch(IOException e)
			{
				e.printStackTrace();
				
				CloseConnection();
				if (! OpenConnection())
					break;
				TryResend --;
			}
		}
		
		return;
	}
	
	public static boolean IsConnected()
	{
		return (Client != null);
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
