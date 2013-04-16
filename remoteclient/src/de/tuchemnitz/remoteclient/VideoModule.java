package de.tuchemnitz.remoteclient;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

import android.R.drawable;
import android.app.Activity;
import android.app.Dialog;
import android.graphics.drawable.Drawable;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnLongClickListener;
import android.webkit.WebView.FindListener;
import android.widget.ImageView;

public class VideoModule {
	
	// A reference of this must be availible as long as the activity is availibile and
	// a video should showed
	private Activity ref_activity = null;
	private Dialog video_dialog = null;
	private ImageView clickpic = null;
	private VideoThread NetVideo = null;
	
	
	private Handler RecvPicHandler = new Handler()
	{
		/**
		 * Handles callback messages from VideoThread
		 * 
		 */
		@Override public void dispatchMessage(Message msg)
		{
		    //super.dispatchMessage(msg);
		    
			if(video_dialog != null)
			{
				clickpic.setImageResource(R.drawable.stop);
			}
		    
		    return;
		}
	};
	
	public VideoModule(Activity callingActivity)
	{
		ref_activity = callingActivity;
		
		//Creating the dialog
		video_dialog = new Dialog(ref_activity);
		video_dialog.setContentView(R.layout.dialog_video);
		clickpic = (ImageView) video_dialog.findViewById(R.id.videodial_pic);
		/**
		 * By pressing the related Image the about dialog will be closed
		 * 
		 */
    	clickpic.setOnClickListener(new OnClickListener() {			
			@Override
			public void onClick(View v) {
				NetVideo.StopThread();
				video_dialog.dismiss();
			}
		});
    	
		//Register Callback for getting Pic from VideoThread
		//NetworkModule.RegisterCallback(null,		-1,				0);
		//NetworkModule.RegisterCallback(EvtHandler,	EVENT_CONN,		NetworkModule.INFO_CONN);
	}
	
	public void create_dialog()
	{
		NetVideo = new VideoThread(RecvPicHandler);
		
    	video_dialog.show();
    	
    	NetVideo.start(); // start the run-function of VideoThread
	}

}


class VideoThread extends Thread {
	
	private final int Port = 0x8001;
	private volatile boolean CloseVideo = false;
	private Handler RecEvHandler = null;
	
	public VideoThread(Handler EventHandler){
		RecEvHandler = EventHandler;
		return;
	}
	
	@Override
	public void run()
	{
		//CloseVideo = false;
		//byte[] inData = new byte[6144]; // Platz für Pakete
		DatagramSocket videosocket = null;
		
		try {
			videosocket = new DatagramSocket(Port); // Socket binden
		} catch (SocketException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			return;
		}
		
		while(!CloseVideo)
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
					CloseVideo = true;
				}
			}
			//---------------------------------------------
			// UDP-Verbindung
			
//			DatagramPacket packet = new DatagramPacket( new byte[6144], 6144 );
//			try {
//				videosocket.receive( packet );
//			} catch (IOException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			}

			// Empfänger auslesen
			//InetAddress address = packet.getAddress();
			//int         port    = packet.getPort();
//			int         len     = packet.getLength();
//			byte[]      data    = packet.getData();
			
			Log.v("Video.Thread", "run - "+ RecEvHandler.toString());
			if(RecEvHandler != null)
			{
				RecEvHandler.sendMessage(Message.obtain(null, 0, "Hallo"));
			}
			
		}
		videosocket.close();
		Log.v("Video.Thread", "END");
	}
	
	public void StopThread()
	{
		CloseVideo = true;
		Log.v("Video.Thread", "Finish - " + String.valueOf(CloseVideo));
		synchronized (this)
		{
			this.notify();
		}
		
		return;
	}
	
	//http://opencv.org/about.html für bilder
}
