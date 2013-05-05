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
	private static boolean close_videothread_mark = false;
	private static Bitmap bitmap = null;
	private final static BitmapFactory.Options opt = new BitmapFactory.Options();
	
	
	
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
//				Matrix matrix = new Matrix();
//				matrix.postScale( ((float) w / bitmap.getWidth() ),
//						          ((float) h / bitmap.getHeight()) );
//				Bitmap resbitmap = Bitmap.createBitmap(bitmap,0,0,bitmap.getWidth(), bitmap.getHeight(),matrix,false);
				bitmap = Bitmap.createScaledBitmap(bitmap, w, h, false);
				
				video_dialog_picture.setImageBitmap(bitmap);
				Log.v("VideoMod", "Pic changed");
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
				
				Log.v("VideoMod", "Pic changed");
			}
		    
		    //System.gc();
		    
		    return;
		}
	};
	
//	public VideoModule(Activity callingActivity)
//	{
//		ref_activity = callingActivity;
//		
//		//Creating the dialog
//		video_dialog = new Dialog(ref_activity);
//		video_dialog.setContentView(R.layout.dialog_video);
//		clickpic = (ImageView) video_dialog.findViewById(R.id.videodial_pic);
//		/**
//		 * By pressing the related Image the about dialog will be closed
//		 * 
//		 */
//    	clickpic.setOnClickListener(new OnClickListener() {			
//			@Override
//			public void onClick(View v) {
//				video_dialog.dismiss();
//			}
//		});
//    	
//    	video_dialog.setOnDismissListener(new OnDismissListener() {
//			
//			@Override
//			public void onDismiss(DialogInterface dialog) {
//				NetVideo.StopThread();
//			}
//		});
//    	
//    	
//		//Register Callback for getting Pic from VideoThread
//		//NetworkModule.RegisterCallback(null,		-1,				0);
//		//NetworkModule.RegisterCallback(EvtHandler,	EVENT_CONN,		NetworkModule.INFO_CONN);
//	}
	
	
	
	public static void create_dialog(Activity ref_activity, boolean closeVideoServerOnDialogDismiss)
	{
		//Creating the dialog
		video_dialog = new Dialog(ref_activity);
		video_dialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
		video_dialog.setContentView(R.layout.dialog_video);
		video_dialog_picture = (ImageView) video_dialog.findViewById(R.id.videodial_pic);
		
		close_videothread_mark = closeVideoServerOnDialogDismiss;
		
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
				if (close_videothread_mark)
				{
					stopVideoServer();
					close_videothread_mark = false;
				}
				NetworkModule.Video(VIDEOSTATE.OFF);
			}
		});
		
    	NetworkModule.Video(VIDEOSTATE.ON, getVideoServerPort());
    	video_dialog.show();
	}
	
	
	// returns the port on success if error then -1 is returned
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
	
	public static int getVideoServerPort()
	{
		if(NetVideo != null)
		{
			return NetVideo.getServerPort();
		}
		return -1;
	}
	
	public static void stopVideoServer()
	{
		if(NetVideo != null)
		{
			NetVideo.StopThread();
			NetVideo = null;
			unsetVideoPicture();
		}
	}
	
	public static boolean isVideoThreadStarted()
	{
		if( NetVideo == null)
		{
			return false;
		}
		
		return true;
	}
	
	public static void setVideoPicture (ImageView active_image)
	{
		video_picture = active_image;
	}
	
	public static void unsetVideoPicture()
	{
		video_picture = null;
	}
}


class VideoThread extends Thread {
	
	//private int Port = 0x8001;
	private volatile boolean CloseVideo = false;
	private Handler RecEvHandler = null;
	DatagramSocket videosocket = null;
	
	public VideoThread(Handler EventHandler){
		RecEvHandler = EventHandler;
		return;
	}
	
	public int getServerPort()
	{
		if(videosocket != null)
			return videosocket.getLocalPort();
		else
			return -1;
	}
	
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
			if(RecEvHandler != null)
			{
				RecEvHandler.sendMessage(Message.obtain(null,0,0, len, data));
			}
			
		}
		Log.v("Video.Thread", "END");
	}
	
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
