package de.tuchemnitz.remoteclient;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import de.tuchemnitz.remoteclient.NetworkModule.VIDEOSTATE;

import android.app.Activity;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnDismissListener;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.ImageView;

public class VideoModule {
	
	// A reference of this must be availible as long as the activity is availibile and
	// a video should showed
	//private Activity ref_activity = null;
	private static Dialog video_dialog = null;
	private static ImageView video_dialog_picture = null;
	private static ImageView video_picture = null;
	private static VideoThread NetVideo = null;
	private static boolean video_networkcommand_send_onoff = false;
	private static Bitmap bitmap = null;
	private final static BitmapFactory.Options opt = new BitmapFactory.Options();
	
	public static int Videotransparency_bewact = 100;
	
	/**
	* @class RecvPicHandler
	*
	* A class to handle the events from the callbackfunction of the VideoThread
	*/
	private static Handler RecvPicHandler = new Handler()
	{
		/**
		 * Handles callback messages from VideoThread
		 * 
		 */
		@Override public void dispatchMessage(Message msg)
		{
		    //super.dispatchMessage(msg);
		    if(video_dialog_picture != null)
		    {
//		    	BitmapFactory.Options opt = new BitmapFactory.Options();
//				opt.inDither = true;
//				opt.inPreferredConfig = Bitmap.Config.ARGB_8888;
				bitmap = BitmapFactory.decodeByteArray((byte[])msg.obj, 0, msg.arg2, opt);
				
				int h = video_dialog_picture.getHeight();
				int w = video_dialog_picture.getWidth();
				if(w > h) w = (4*h)/3;
				else      h = (3*w)/4;
				if(w<=0)
				{
					return;
				}
//				Matrix matrix = new Matrix();
//				matrix.postScale( ((float) w / bitmap.getWidth() ),
//						          ((float) h / bitmap.getHeight()) );
//				Bitmap resbitmap = Bitmap.createBitmap(bitmap,0,0,bitmap.getWidth(), bitmap.getHeight(),matrix,false);
				bitmap = Bitmap.createScaledBitmap(bitmap, w, h, false);
				
				video_dialog_picture.setImageBitmap(bitmap);
				//Log.v("VideoMod", "Pic changed");
		    }
		    else if(video_picture != null)
			{
				//ImageFactroy msg.obj
				//clickpic.setImageResource(R.drawable.stop);
//				BitmapFactory.Options opt = new BitmapFactory.Options();
//				opt.inDither = true;
//				opt.inPreferredConfig = Bitmap.Config.ARGB_8888;
				bitmap = BitmapFactory.decodeByteArray((byte[])msg.obj, 0, msg.arg2, opt);
				video_picture.setImageBitmap(bitmap);
				
				//Log.v("VideoMod", "Pic changed");
			}
		    
		    //System.gc();
		    
		    return;
		}
	};	
	
	
	/**
	 * Creates a dialog in the Activity, which is given by ref_activity
	 * 
	 * @param ref_activity a reference of the activity in which the dialog will be opened
	 * @param startstop_naovideoserver By closing the dialoge let the Networkmodule send a video-stop-command to the robot
	 */
	public static void create_dialog(Activity ref_activity, boolean startstop_naovideoserver)
	{
		if(NetVideo == null)
		{
			return;
		}
		NetVideo.pause_on(); // stop updating window while dialog is created
		//Creating the dialog
		video_dialog = new Dialog(ref_activity);
		video_dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
		video_dialog.setContentView(R.layout.dialog_video);
		video_dialog_picture = (ImageView) video_dialog.findViewById(R.id.videodial_pic);
		
		video_networkcommand_send_onoff = startstop_naovideoserver;
		
		/**
		 * By pressing the related Image the about dialog will be closed
		 * 
		 */
		video_dialog_picture.setOnClickListener(new OnClickListener() {			
			@Override
			public void onClick(View v) {
				video_dialog.dismiss();
			}
		});
    	
    	video_dialog.setOnDismissListener(new OnDismissListener() {
			
			@Override
			public void onDismiss(DialogInterface dialog) {
				video_dialog = null;
				video_dialog_picture = null;
				if (video_networkcommand_send_onoff)
				{
					NetworkModule.Video(VIDEOSTATE.OFF);
					video_networkcommand_send_onoff = false;
				}
				
			}
		});
		
    	if(video_networkcommand_send_onoff)
    	{
    		NetworkModule.Video(VIDEOSTATE.ON, getVideoServerPort());
    	}
    	video_dialog.show();
    	
    	NetVideo.pause_off();
	}
	
	/**
	 * Let you know if a videodialog exists
	 * 
	 * @return True if a Reference to a videodialog exists
	 */
	public static boolean isVideoDialogOpen()
	{
		return (video_dialog != null);
	}
	
	/**
	 * closes the videodialog
	 * if there is no existing dialog then nothing happens
	 * 
	 */
	public static void closeVideoDialog()
	{
		if(video_dialog != null)
			video_dialog.dismiss();
	}
	
	/**
	 * Resets all the variables of the VideoModule.
	 * 
	 */
	public static void init()
	{
		video_dialog = null;
		video_dialog_picture = null;
		video_picture = null;
		NetVideo = null;
		video_networkcommand_send_onoff = false;
		bitmap = null;
	}
	
	/**
	 * Start a new thread with an udp-server to receive pictures from robot.
	 * 
	 *  @return On success it returns the port. If an error occurs then -1 is returned.
	 */
	public static int startVideoServer()
	{
		
		if(NetVideo != null)
		{
			stopVideoServer();
		}
		
		opt.inDither = true;
		opt.inPreferredConfig = Bitmap.Config.ARGB_8888;
		
		NetVideo = new VideoThread(RecvPicHandler);
		NetVideo.start(); // start the run-function of VideoThread
		
		return NetVideo.getServerPort();
	}
	
	/**
	 * Returns the port of the videoserver
	 * 
	 * @return the port of the udp-socket
	 */
	public static int getVideoServerPort()
	{
		if(NetVideo != null)
		{
			return NetVideo.getServerPort();
		}
		return -1;
	}
	
	/**
	 * Stops the videoserver an closes the thread and the udp-socket in it. 
	 * 
	 */
	public static void stopVideoServer()
	{
		if(NetVideo != null)
		{
			try {
				NetVideo.StopThread();
			} catch (Exception e) {
				e.printStackTrace();
				System.gc();
			}
			NetVideo = null;
			unsetVideoPicture();
		}
	}
	
	/**
	 * This function tells if a thread with an udp-socket exists or not
	 * 
	 * @return True if thread with udp-socket for receiving pictures exists.
	 * 
	 */
	public static boolean isVideoThreadStarted()
	{
		if( NetVideo == null)
		{
			return false;
		}
		
		return true;
	}
	
	/**
	 * Set the active Image, which will later updated with the received pictures
	 * 
	 * @param active_image current active ImageView for video
	 * 
	 */
	public static void setVideoPicture (ImageView active_image)
	{
		video_picture = active_image;
	}
	
	/**
	 * Unset the variable video_picture by setting it to null.
	 * No picture can be updated anymore.
	 */
	public static void unsetVideoPicture()
	{
		video_picture = null;
	}
}


/**
* @class VideoThread
*
* A class which will start a new thread and an udp-server for receiving pictures from the NAO-Robot
*/
class VideoThread extends Thread {
	
	//private int Port = 0x8001;
	private volatile boolean CloseVideo = false;
	private Handler RecEvHandler = null;
	DatagramSocket videosocket = null;
	private volatile boolean receive_pause = false; 
	
	/**
	 * The constructor
	 * @param EventHandler VideoModule RecvPicHandler member
	 */
	public VideoThread(Handler EventHandler){
		RecEvHandler = EventHandler;
		return;
	}
	
	/**
	 * Return the Port from the udp-socket
	 * 
	 * @return port of udp-socket
	 */
	public int getServerPort()
	{
		if(videosocket != null)
			return videosocket.getLocalPort();
		else
			return -1;
	}
	
	/**
	 * Set the member receive_pause to true. Effect is the Handler is not called.
	 * It was necessary while changing the view some elements in the gui where not available but variables in the videomodule were not updated jet.
	 */
	public void pause_on()
	{
		receive_pause = true;
	}
	
	/**
	 * Set the member receive_pause to false. Effect is the Handler will be called from this moment on.
	 * 
	 * It was necessary while changing the view some elements in the gui where not available but variables in the videomodule were not updated jet.
	 */
	public void pause_off()
	{
		receive_pause = false;
	}
	
	/**
	 * Robot Video-Module-Function.
	 * 
	 * Opens a udp-socket and receive pictures from the robot. After a picture was received the Handler is called.
	 * 
	 */
	@Override
	public void run()
	{
		//CloseVideo = false;
		//byte[] inData = new byte[6144]; // Platz für Pakete
		
		try {
			videosocket = new DatagramSocket(0x8001); // Socket binden
		} catch (SocketException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			try {
				videosocket = new DatagramSocket(); // Socket binden
			} catch (SocketException e2) {
				e2.printStackTrace();
				return;
			}
		}
		
		Log.v("Video.Thread", "socket - " + videosocket.toString() + " Port=" + String.valueOf(getServerPort() ));
		
		
		//----------------------------------------------------
//			InetAddress ia = null;
//			try {
//				ia = InetAddress.getByName( NetworkModule.GetIPAddress() );
//			} catch (UnknownHostException e1) {
//				// TODO Auto-generated catch block
//				e1.printStackTrace();
//			}
//
//		      String s = "hallo";
//		      byte[] raw = s.getBytes();
//
//		      DatagramPacket packet1 = new DatagramPacket( raw, raw.length, ia, 21845 );
//
//			try {
//				videosocket.send( packet1 );
//			} catch (Exception e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			}
		//------------------------------------
		
		
		
		while(!CloseVideo)
		{
			//---------------------------------------------
			// UDP-Verbindung
			
			// Space for UDP-Packet
			DatagramPacket packet = new DatagramPacket( new byte[8192], 8192 );
			try {
				videosocket.receive( packet );
			} catch (IOException e) {
				// TODO Auto-generated catch block
				Log.v("Video.Thread", "Receive Exeption END");
				e.printStackTrace();
				return;
			}

		      
			// Empfänger auslesen
			//InetAddress address = packet.getAddress();
			//int         port    = packet.getPort();
			int         len     = packet.getLength();
			byte[]      data    = packet.getData();
			
			//Log.v("Video.Thread", "run - "+ RecEvHandler.toString());
			if(RecEvHandler != null && receive_pause == false)
			{
				RecEvHandler.sendMessage(Message.obtain(null,0,0, len, data));
			}
			
		}
		Log.v("Video.Thread", "END");
	}
	
	/**
	 * Requests the thread to stop and close udp-socket
	 */
	public void StopThread()
	{
		CloseVideo = true;
		Log.v("Video.Thread", "Finish - " + String.valueOf(CloseVideo));
		try{
			videosocket.close();
		}catch(Exception e)
		{
			e.printStackTrace();
		}
		
		return;
	}
	
	//http://opencv.org/about.html für bilder
}
