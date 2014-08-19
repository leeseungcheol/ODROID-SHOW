/*
 * Copyright 2009 Cedric Priscal
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. 
 */

package android_serialport_api.sample;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.security.InvalidParameterException;
import java.util.Calendar;
import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android_serialport_api.SerialPort;

public class MainMenu extends Activity {
	
	Timer timer;
	MyTimerTask myTimerTask;
	protected Application mApplication;
	protected SerialPort mSerialPort;
	protected OutputStream mOutputStream;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        mApplication = (Application) getApplication();
        
		try {
			mSerialPort = mApplication.getSerialPort();
			mOutputStream = mSerialPort.getOutputStream();
		} catch (InvalidParameterException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (SecurityException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

        final Button BtnStart = (Button)findViewById(R.id.BtnStart);
        BtnStart.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				
				if (timer != null) {
					timer.cancel();
				}
				timer = new Timer();
				myTimerTask = new MyTimerTask();
				
				timer.schedule(myTimerTask, 1000, 1000);
			}
		});
        
        final Button BtnStop = (Button)findViewById(R.id.BtnStop);
        BtnStop.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				if (timer != null) {
					timer.cancel();
					timer = null;
				}
				
			}
		});
    }
    
    class MyTimerTask extends TimerTask {
    	public void run() {
    		
    		String currentDate = java.text.DateFormat.getDateTimeInstance().format(Calendar.getInstance().getTime());
    		ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
			byte[] mBuffer = new byte[] {0x1b, 0x5b, '0', ';', '0', 'f'};
    		try {
				outputStream.write(mBuffer);
				outputStream.write(currentDate.getBytes());
				mBuffer = outputStream.toByteArray();
				mOutputStream.write(mBuffer);
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
    			
    	}
    }

}
