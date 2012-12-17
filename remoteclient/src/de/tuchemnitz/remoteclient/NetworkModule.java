package de.tuchemnitz.remoteclient;

import java.io.*;
import java.net.Socket;
import java.util.LinkedList;
import java.util.Queue;

import android.app.Activity;
import android.app.AlertDialog;
import android.util.Log;

public class NetworkModule {

	private static String IP_Addr = null;
	private static final int Port = 0x8000;
	public static final char MOVE_UP 	= 'F';
	public static final char MOVE_DOWN	= 'B';
	public static final char MOVE_LEFT 	= 'L';
	public static final char MOVE_RIGHT	= 'R';
	private static Activity RefActivity;
	//private static Thread NetTData = null;
	private static NetworkThread NetTData = null;
	
	public static void SetIPAddress(String IP_Str)
	{
		IP_Addr = IP_Str;
		
		return;
	}
	
	public static String GetIPAddress()
	{
		return IP_Addr;
	}
	
	public static void Move(int Type, char Direction)
	{
		if (NetTData == null || NetTData.GetConnectionState() != 1)
			return;
		
		NetworkThread.CMDTYPE MoveType;
		String CommandStr;
		
		switch(Type)
		{
		case 0:
			MoveType = NetworkThread.CMDTYPE.MOVE;		// "MOV";
			break;
		case 1:
			MoveType = NetworkThread.CMDTYPE.MOVEARM;	// "ARM"
			break;
		case 2:
			MoveType = NetworkThread.CMDTYPE.MOVEHEAD;	// "HAD"
			break;
		default:
			return;
		}
		
		CommandStr = String.valueOf(Direction);
		NetTData.QueueCommand(MoveType, CommandStr);
		
		return;
	}
	
	public static String SitToggle()
	{
		if (NetTData == null || NetTData.GetConnectionState() != 1)
			return "";
		
		NetTData.QueueCommand(NetworkThread.CMDTYPE.SIT, null);
		
		return "";
	}
	
	public static void Rest()
	{
		if (NetTData == null || NetTData.GetConnectionState() != 1)
			return;
		
		NetTData.QueueCommand(NetworkThread.CMDTYPE.REST, null);
		
		return;
	}
	
	public static void Speak(String Text)
	{
		if (NetTData == null || NetTData.GetConnectionState() != 1)
			return;
		
		NetTData.QueueCommand(NetworkThread.CMDTYPE.SPEAK, Text + "_");
		
		return;
	}
	
	public static void Dance(String Text)
	{
		if (NetTData == null || NetTData.GetConnectionState() != 1)
			return;
		
		NetTData.QueueCommand(NetworkThread.CMDTYPE.DANCE, Text);
		
		return;
	}
	
	public static int GetBatteryState()
	{
		if (NetTData == null || NetTData.GetConnectionState() != 1)
			return 0;
		
		return (int)(Math.random() * 0x100);
	}
	
	public static void OpenConnection(Activity MainAct)
	{
		if (NetTData != null && NetTData.GetConnectionState() != -1)
			return;
		
		if (MainAct != null)
			RefActivity = MainAct;
		
		NetTData = new NetworkThread(IP_Addr, Port);
		//NetTData = new Thread(NetTData);
		NetTData.start();
		
		return;
	}
	
	public static void CloseConnection()
	{
		if (NetTData == null)
			return;
		
		if (NetTData.GetConnectionState() == 1)
			NetTData.StopThread();
		
		return;
	}
	
	public static int IsConnected()
	{
		if (NetTData == null)
			return -1;
		
		return NetTData.GetConnectionState();
	}
	
	public static void net_test()
	{
		//DataOutputStream out = new DataOutputStream(outToServer);
		//DataInputStream in = new DataInputStream(inFromServer);
		
		//SendCommand("Hello World");
		/*System.out.println("Server says " + in.readLine());
		while(in.available() > 0)
			System.out.println("\t" + in.readLine());*/
		//out.writeBytes("QUIT\r\n\r\n");
		
		return;
	}
}


//class NetworkThread implements Runnable
class NetworkThread extends Thread
{
	public enum CMDTYPE
	{
		NETTEST, MOVE, MOVEARM, MOVEHEAD, SIT, REST, SPEAK, DANCE, GETBATT
	};
	private final String CMDTYPE_MOVE	= "MOV";
	private final String CMDTYPE_MVARM	= "ARM";
	private final String CMDTYPE_MVHEAD	= "HAD";
	private final String CMDTYPE_SIT	= "SIT";
	private final String CMDTYPE_REST	= "RST";
	private final String CMDTYPE_SPEAK 	= "SPK";
	private final String CMDTYPE_DANCE 	= "DNC";
	private final String CMDTYPE_BATT	= "BAT";
	
	private final String TERMINATE_CHR = "";	//"\0";
	private final String IP_Addr;
	private final int Port;
	private Socket Client = null;
	private OutputStream outToServer;
	private InputStream inFromServer;
	private volatile boolean CloseConn = false;
	private volatile int ConnectionState = -1;
	
	class CommandQueue
	{
		CMDTYPE Type;
		String Data;
		
		CommandQueue(CMDTYPE CmdType, String CmdData)
		{
			Type = CmdType;
			Data = CmdData;
			
			return;
		}
	};
	Queue<CommandQueue> CmdQueue = new LinkedList<CommandQueue>();
	
	public NetworkThread(String DestIP, int DestPort)
	{
		IP_Addr = DestIP;
		Port = DestPort;
		
		return;
	}
	
	@Override public void run()
	{
		boolean RetVal;
		CommandQueue CurCmd;
		String CmdStr;
		
		ConnectionState = 0;
		RetVal = OpenNet();
		if (! RetVal)
		{
			ConnectionState = -1;
			return;
		}
		
		ConnectionState = 1;
		while(! CloseConn)
		{
			synchronized(this)
			{
				try
				{
					this.wait();
				}
				catch (InterruptedException e)
				{
					e.printStackTrace();
					CloseConn = true;
				}
			}
			
			CurCmd = CmdQueue.poll();
			if (CurCmd != null)
			{
				if (CurCmd.Type == CMDTYPE.NETTEST)
				{
					SendCommand(" ");
					continue;
				}
				
				switch(CurCmd.Type)
				{
				case MOVE:
					CmdStr = CMDTYPE_MOVE;
					break;
				case MOVEARM:
					CmdStr = CMDTYPE_MVARM;
					break;
				case MOVEHEAD:
					CmdStr = CMDTYPE_MVHEAD;
					break;
				case SIT:
					CmdStr = CMDTYPE_SIT;
					break;
				case REST:
					CmdStr = CMDTYPE_REST;
					break;
				case SPEAK:
					CmdStr = CMDTYPE_SPEAK;
					break;
				case DANCE:
					CmdStr = CMDTYPE_DANCE;
					break;
				case GETBATT:
					CmdStr = CMDTYPE_BATT;
					break;
				default:
					CmdStr = "";
					break;
				}
				if (CurCmd.Data != null)
					CmdStr += "_" + CurCmd.Data;
				SendCommand(CmdStr);
				
				HandleAnswer(CurCmd);
			}
		}
		
		CloseNet();
		ConnectionState = -1;
		
		return;
	}
	
	private void HandleAnswer(CommandQueue Command)
	{
		String ReceiveStr;
		
		switch(Command.Type)
		{
		case SIT:
			ReceiveStr = GetAnswer();
			Log.v("NetMod.SIT", "received string: " + ReceiveStr);
			if(ReceiveStr != null)
			{
				ReceiveStr = ReceiveStr.trim();
				if( ReceiveStr.compareToIgnoreCase("SIT_STAND")==0)
					ReceiveStr =  "STAND";
				else if( ReceiveStr.compareToIgnoreCase("SIT_SIT")==0)
					ReceiveStr =  "SIT";
				else ReceiveStr =  "MIST";
				//ReceiveStr = (ReceiveStr.split("_"))[1];
			}
			Log.v("NetMod.SIT", "returned string: " + ReceiveStr);
			break;
		case GETBATT:
			ReceiveStr = GetAnswer();
			if(ReceiveStr != null)
			{
				if(ReceiveStr.length()>=5)
				{
					char batv = ReceiveStr.charAt(4);
					Log.v("NetMod.BAT", String.valueOf((int)batv));
					//return (int) batv;
					return;
				}
			}
			break;
		default:
			return;
		}
		
		// TODO: Send event to main
		return;
	}
	
	public void StopThread()
	{
		CloseConn = true;
		synchronized (this)
		{
			this.notify();
		}
		
		return;
	}
	
	public int GetConnectionState()
	{
		return ConnectionState;
	}
	
	public boolean HasConnection()
	{
		return ConnectionState == 1 && ! Client.isClosed();
	}
	
	public boolean TestConnection()
	{
		CommandQueue NewCmd;
		
		NewCmd = new CommandQueue(CMDTYPE.NETTEST, null);
		CmdQueue.offer(NewCmd);
		synchronized (this)
		{
			this.notify();
		}
		// TO DO: Wait for thread
		
		return Client.isConnected();
	}
	
	public void QueueCommand(CMDTYPE Type, String Data)
	{
		CommandQueue NewCmd;
		
		NewCmd = new CommandQueue(Type, Data);
		CmdQueue.offer(NewCmd);
		synchronized (this)
		{
			this.notify();
		}
		
		return;
	}
	
	private boolean OpenNet()
	{
		try
		{
			Log.v("NetMod", "Connecting to " + IP_Addr + " on port " + Port);
			Client = new Socket(IP_Addr, Port);
			Client.setSoTimeout(3000);
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
	
	private boolean CloseNet()
	{
		try
		{
			DataOutputStream out = new DataOutputStream(outToServer);
			out.writeBytes("D" + TERMINATE_CHR);
			
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

	private void SendCommand(String CommandStr)
	{
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
				CloseNet();
				if (! OpenNet())
				{
					Log.v("NetMod", "Reconnect failed.");
/*					new AlertDialog.Builder(RefActivity)
						.setMessage("Connection lost.")
						.setNeutralButton("bummer", null)
						.show();*/
					break;
				}
				TryResend --;
			}
		}
		
		return;
	}
	
	private String GetAnswer()
	{
		DataInputStream in = new DataInputStream(inFromServer);
		byte Data[] = null;
		int DataLen;
		
		try
		{
			DataLen = in.read(Data);
			if (DataLen > 0)
				return new String(Data);
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
		
		/*while(in.available() > 0)
			System.out.println("\t" + in.readLine());*/
		
		return "";
	}
	
/*	private static String Receive()
	{
		if (Client == null || ! Client.isConnected())
			return null;
		
		String inMessage = null;
		try
		{
			BufferedReader in = new BufferedReader(new InputStreamReader(inFromServer));
			inMessage = in.readLine();
		}
		catch(Exception e)
		{
			e.printStackTrace();
			Log.v("NetMod.Recv", "Nothing received");
		}
		return inMessage;
	}
	*/
}

/*class Log
{
	public static void v(String name, String text)
	{
		System.out.println(name + "\t" + text);
		return;
	}
}*/
