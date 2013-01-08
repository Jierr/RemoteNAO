package de.tuchemnitz.remoteclient;

import java.io.*;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Queue;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

class EventCallback
{
	Handler EvtHandler;
	int EventType;
	int InfoType;
};

class RobotInformation
{
	int BattLoad;
	String State;
};

public class NetworkModule {

	private static String IP_Addr = "134.109.151.142";
	private static final int Port = 0x8000;
	public static final char MOVE_UP 	= 'F';
	public static final char MOVE_DOWN	= 'B';
	public static final char MOVE_LEFT 	= 'L';
	public static final char MOVE_RIGHT	= 'R';
	private static NetworkThread NetTData = null;
	public static final int INFO_BATT = 0;
	public static final int INFO_STATE = 1;
	public static final int INFO_CONN = 2;
	public static final int CONN_CLOSED = 0;
	public static final int CONN_CONNECTING = 1;
	public static final int CONN_OPEN = 2;
	public static final String STATE_CROUCH		= "CROUCHING";
	public static final String STATE_STAND		= "STANDING";
	public static final String STATE_SIT		= "SITTING";
	public static final String STATE_WALK		= "WALKING";
	public static final String STATE_STOP		= "STOPPING";
	public static final String STATE_MOVE		= "MOVING";
	public static final String STATE_UNKNOWN	= "UNKNOWN";
	
	private static ArrayList<EventCallback> EventList = new ArrayList<EventCallback>();
	private static RobotInformation RoboInfo = new RobotInformation();
	
	public static void RegisterCallback(Handler EvtHandler, int EventType, int InfoType)
	{
		if (EventType == -1)
		{
			EventList.clear();
			return;
		}
		
		EventCallback NewCB = new EventCallback();
		
		NewCB.EvtHandler = EvtHandler;
		NewCB.EventType = EventType;
		NewCB.InfoType = InfoType;
		EventList.add(NewCB);
		
		return;
	}
	
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
		if (NetTData == null || NetTData.GetConnectionState() != CONN_OPEN)
			return;
		
		NetworkThread.CMDTYPE MoveType;
		String CommandStr;
		
		switch(Type)
		{
		case 0:
			MoveType = NetworkThread.CMDTYPE.MOVE;		// "MOV";
			CommandStr = String.valueOf(Direction);
			break;
		case 1:
		case 2:
			MoveType = NetworkThread.CMDTYPE.MOVEARM;	// "ARM"
			if (Type == 1)
				CommandStr = "L";
			else //if (Type == 2)
				CommandStr = "R";
			CommandStr += "_" + String.valueOf(Direction);
			break;
		case 3:
			MoveType = NetworkThread.CMDTYPE.MOVEHEAD;	// "HAD"
			CommandStr = String.valueOf(Direction);
			break;
		default:
			return;
		}
		
		NetTData.QueueCommand(MoveType, CommandStr);
		
		return;
	}
	
	public static void Stop()
	{
		if (NetTData == null || NetTData.GetConnectionState() != CONN_OPEN)
			return;
		
		NetTData.QueueCommand(NetworkThread.CMDTYPE.STOP, null);
		
		return;
	}
	
	public static void SitToggle()
	{
		if (NetTData == null || NetTData.GetConnectionState() != CONN_OPEN)
			return;
		
		NetTData.QueueCommand(NetworkThread.CMDTYPE.SIT, null);
		
		return;
	}
	
	public static void StandUp()
	{
		if (NetTData == null || NetTData.GetConnectionState() != CONN_OPEN)
			return;
		
		NetTData.QueueCommand(NetworkThread.CMDTYPE.STANDUP, null);
		
		return;
	}
	
	public static void Rest()
	{
		if (NetTData == null || NetTData.GetConnectionState() != CONN_OPEN)
			return;
		
		NetTData.QueueCommand(NetworkThread.CMDTYPE.REST, null);
		
		return;
	}
	
	public static void Speak(String Text)
	{
		if (NetTData == null || NetTData.GetConnectionState() != CONN_OPEN)
			return;
		
		NetTData.QueueCommand(NetworkThread.CMDTYPE.SPEAK, Text + "_");
		
		return;
	}
	
	public static void Dance(String Text)
	{
		if (NetTData == null || NetTData.GetConnectionState() != CONN_OPEN)
			return;
		
		NetTData.QueueCommand(NetworkThread.CMDTYPE.DANCE, Text);
		
		return;
	}
	
	public static void Wink()
	{
		if (NetTData == null || NetTData.GetConnectionState() != CONN_OPEN)
			return;
		
		NetTData.QueueCommand(NetworkThread.CMDTYPE.WINK, null);
		
		return;
	}
	
	public static void Wipe()
	{
		if (NetTData == null || NetTData.GetConnectionState() != CONN_OPEN)
			return;
		
		NetTData.QueueCommand(NetworkThread.CMDTYPE.WIPE, null);
		
		return;
	}
	
	public static void RequestBatteryState()
	{
		if (NetTData == null || NetTData.GetConnectionState() != CONN_OPEN)
			return;
		
		NetTData.QueueCommand(NetworkThread.CMDTYPE.GETBATT, null);
		
		return;
	}
	
	public static Object GetInfoData(int Type)
	{
		if (NetTData == null || NetTData.GetConnectionState() != CONN_OPEN)
			return null;
				
		switch(Type)
		{
		case INFO_BATT:
			//return (int)(Math.random() * 100);
			return RoboInfo.BattLoad;
		case INFO_STATE:
			return RoboInfo.State;
		}
		
		return 0;
	}
	
	public static void OpenConnection()
	{
		if (NetTData != null && NetTData.GetConnectionState() != CONN_CLOSED)
			return;
		
		NetTData = new NetworkThread(IP_Addr, Port, EventList, RoboInfo);
		NetTData.start();
		
		return;
	}
	
	public static void CloseConnection()
	{
		if (NetTData == null || NetTData.GetConnectionState() != CONN_OPEN)
			return;
		
		NetTData.StopThread();
		
		return;
	}
	
	public static int IsConnected()
	{
		if (NetTData == null)
			return 0;
		
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


class NetworkThread extends Thread
{
	public enum CMDTYPE
	{
		NETTEST, MOVE, MOVEARM, MOVEHEAD, STOP, SIT, STANDUP, REST, SPEAK, DANCE, WINK, WIPE, GETBATT
	};
	private final String CMDTYPE_MOVE	= "MOV";
	private final String CMDTYPE_MVARM	= "ARM";
	private final String CMDTYPE_MVHEAD	= "HAD";
	private final String CMDTYPE_STOP	= "STP";
	private final String CMDTYPE_SIT	= "SIT";
	private final String CMDTYPE_STANDUP= "AUF";
	private final String CMDTYPE_REST	= "RST";
	private final String CMDTYPE_SPEAK 	= "SPK";
	private final String CMDTYPE_DANCE 	= "DNC";
	private final String CMDTYPE_WINK 	= "WNK";
	private final String CMDTYPE_WIPE 	= "WIP";
	private final String CMDTYPE_BATT	= "BAT";
	
	private final String RETCMD_BATT	= "BAT";
	private final String RETCMD_STATE	= "ZST";
	
	private final String TERMINATE_CHR = "";
	private final String IP_Addr;
	private final int Port;
	private Socket Client = null;
	private OutputStream outToServer;
	private InputStream inFromServer;
	private volatile boolean CloseConn = false;
	private volatile int ConnectionState = NetworkModule.CONN_CLOSED;
	
	private static ArrayList<EventCallback> EventList;
	private static RobotInformation RoboInfo;
	
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
	
	public NetworkThread(String DestIP, int DestPort, ArrayList<EventCallback> CBEvtList,
						RobotInformation RbInfo)
	{
		IP_Addr = DestIP;
		Port = DestPort;
		EventList = CBEvtList;
		RoboInfo = RbInfo;
		RoboInfo.BattLoad = 0;
		RoboInfo.State = null;
		
		return;
	}
	
	@Override public void run()
	{
		boolean RetVal;
		CommandQueue CurCmd;
		String CmdStr;
		
		ConnectionState = NetworkModule.CONN_CONNECTING;
		ConnectionCallback(ConnectionState);
		RetVal = OpenNet();
		if (! RetVal)
		{
			ConnectionState = NetworkModule.CONN_CLOSED;
			ConnectionCallback(ConnectionState);
			return;
		}
		
		ConnectionState = NetworkModule.CONN_OPEN;
		ConnectionCallback(ConnectionState);
		while(! CloseConn)
		{
			synchronized(this)
			{
				try
				{
					this.wait(400);
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
				case STOP:
					CmdStr = CMDTYPE_STOP;
					break;
				case SIT:
					CmdStr = CMDTYPE_SIT;
					break;
				case STANDUP:
					CmdStr = CMDTYPE_STANDUP;
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
				case WINK:
					CmdStr = CMDTYPE_WINK;
					break;
				case WIPE:
					CmdStr = CMDTYPE_WIPE;
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
			}
			
			HandleAnswer();
		}
		
		CloseNet();
		ConnectionState = NetworkModule.CONN_CLOSED;
		ConnectionCallback(ConnectionState);
		
		return;
	}
	
	private void ConnectionCallback(int RetState)
	{
		EventCallback DestCB;
		
		DestCB = GetCallback(NetworkModule.INFO_CONN);
		if (DestCB == null)
			return;
		DestCB.EvtHandler.sendMessage(Message.obtain(null, DestCB.EventType, RetState, 0));
		
		return;
	}
	
	private EventCallback GetCallback(int InfoType)
	{
		int CurEvt;
		EventCallback TempEvt;
		
		for (CurEvt = 0; CurEvt < EventList.size(); CurEvt ++)
		{
			TempEvt = EventList.get(CurEvt);
			if (TempEvt.InfoType == InfoType)
				return TempEvt;
		}
		
		return null;
	}
	
	private void HandleAnswer()
	{
		try
		{
			if (inFromServer.available() < 4)
				return;
		}
		catch (IOException e)
		{
			e.printStackTrace();
			return;
		}
		
		String ReceiveStr;
		String CmdStr;
		EventCallback DestCB;
		int ArgInt = 0;
		Object ArgObj = null;
		
		ReceiveStr = GetAnswer();
		if (ReceiveStr == null)
			return;
		if (ReceiveStr.charAt(3) != '_')
		{
			Log.v("NetMod.HandleRecv", "Invalid Answer: " + ReceiveStr);
			return;
		}
		Log.v("NetMod.HandleRecv", "Received: " + ReceiveStr);
		CmdStr = ReceiveStr.substring(0, 3);
		ReceiveStr = ReceiveStr.substring(4);
		if (ReceiveStr.length() == 0)
			return;
		
		/*if (CmdStr.equals(CMDTYPE_SIT))
		{
			RoboInfo.SitState = ReceiveStr;
			
			//Log.v("NetMod.SIT", "returned string: " + ReceiveStr);
			DestCB = GetCallback(NetworkModule.INFO_STATE);
			ArgObj = RoboInfo.SitState;
		}*/
		if (CmdStr.equals(RETCMD_STATE))
		{
			/*	STATE_CROUCH
				STATE_STAND
				STATE_SIT
				STATE_WALK
				STATE_STOP
				STATE_MOVE
				STATE_UNKNOWN	*/
			RoboInfo.State = ReceiveStr;
			
			Log.v("NetMod.SIT", "returned string: " + ReceiveStr);
			DestCB = GetCallback(NetworkModule.INFO_STATE);
			ArgObj = RoboInfo.State;
		}
		else if (CmdStr.equals(RETCMD_BATT))
		{
			RoboInfo.BattLoad = (int)ReceiveStr.charAt(0);
			
			DestCB = GetCallback(NetworkModule.INFO_BATT);
			ArgInt = RoboInfo.BattLoad;
			Log.v("NetMod.BAT", Integer.toHexString(ArgInt));
		}
		else
		{
			return;
		}
		
		if (DestCB == null)
			return;
		DestCB.EvtHandler.sendMessage(Message.obtain(null, DestCB.EventType, ArgInt, 0, ArgObj));
		
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
		return ConnectionState == NetworkModule.CONN_OPEN && ! Client.isClosed();
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
			Client = new Socket();
			Client.connect(new InetSocketAddress(IP_Addr, Port), 3000);
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
	
	private boolean CloseNet()
	{
		try
		{
			DataOutputStream out = new DataOutputStream(outToServer);
			out.writeBytes("DIS" + TERMINATE_CHR);
			out.close();
			
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
//		Log.v("NetMod.Send", "isConnected: " + (Client.isConnected() ? "true" : "false"));
//		Log.v("NetMod.Send", "isClosed: " + (Client.isClosed() ? "true" : "false"));
//		Log.v("NetMod.Send", "isBound: " + (Client.isBound() ? "true" : "false"));
		int TryResend;
		
		TryResend = 2;
		while(TryResend > 0)
		{
			try
			{
				DataOutputStream out = new DataOutputStream(outToServer);
				
				out.writeBytes(CommandStr + TERMINATE_CHR);
				Log.v("NetMod.SendCmd", "Sent: " + CommandStr);
				//outToServer.write(byte_array);
				
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
					ConnectionCallback(-1);
					CloseConn = true;
					break;
				}
				TryResend --;
			}
		}
		
		return;
	}
	
	private String GetAnswer()
	{
		String InStr;
		byte Data[];
		int DataLen;
		
		InStr = null;
		try
		{
			Log.v("NetMod.Receive", "Waiting for data...");
			Data = new byte[16];
			DataLen = inFromServer.read(Data);
			Log.v("NetMod.Receive", "Received " + Integer.toString(DataLen) + " bytes");
//			String DebugStr = "";
//			for (int i=0; i < DataLen; i ++)
//				DebugStr += Integer.toHexString(Data[i]) + " ";
//			DebugStr += "[" + Integer.toHexString(Data[DataLen]) + "]";
//			Log.v("NetMod.Receive", DebugStr);
			if (DataLen > 0)
			{
				//InStr = new String(Data).substring(0, DataLen);
				char ChrArr[] = new char[DataLen]; 
				for (int i = 0; i < DataLen; i ++)
					ChrArr[i] = (char)Data[i];
				InStr = new String(ChrArr);
			//	String DebugStr = "";
			//	for (int i=0; i < InStr.length(); i ++)
			//		DebugStr += Integer.toHexString(InStr.charAt(i)) + " ";
			//	Log.v("NetMod.Receive", DebugStr);
			}
			//BufferedReader in = new BufferedReader(new InputStreamReader(inFromServer));
			//inMessage = in.readLine();
			
			// Clear buffer
			//Log.v("NetMod.Receive", "Remaining bytes: " + Integer.toString(inFromServer.available()));
			while(inFromServer.available() > 0)
				DataLen = inFromServer.read(Data);
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
		
		return InStr;
	}
}
