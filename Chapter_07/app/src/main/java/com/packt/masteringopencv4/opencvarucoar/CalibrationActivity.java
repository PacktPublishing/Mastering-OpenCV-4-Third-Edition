package com.packt.masteringopencv4.opencvarucoar;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.media.Image;
import android.media.ImageReader;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.util.Size;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;

import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.util.concurrent.Semaphore;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReadWriteLock;

public class CalibrationActivity extends Activity implements CameraHandler {

    private static final String LOGTAG = "CalibrationActivity";
    private CameraHelper mCameraHelper;
    private ImageHelper mImageHelper;
    SurfaceView mSurfaceView;
    private ByteBuffer mPreviewByteBufferGray;
    private Size mPreviewSize;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Set window fullscreen and remove title bar
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(
                WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_calibration);

        Log.i(LOGTAG, "getWindowManager().getDefaultDisplay().getRotation(): " + getWindowManager().getDefaultDisplay().getRotation());

        mImageHelper = new ImageHelper(this);
        mCameraHelper = new CameraHelper(this);
        mCameraHelper.setCameraListener(this);
    }

    /**
     * Callbacks invoked upon state changes in our {@code SurfaceView}.
     */
    final SurfaceHolder.Callback mSurfaceHolderCallback = new SurfaceHolder.Callback() {
        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            // This is called every time the surface returns to the foreground
            Log.i(LOGTAG, "Surface created");
        }
        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            Log.i(LOGTAG, "Surface destroyed");
            holder.removeCallback(this);
            // We don't stop receiving callbacks forever because onResume() will reattach us
        }
        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            Log.i(LOGTAG, "Surface changed: " + width + "x" + height);
            SurfaceHolder surfaceHolder = mSurfaceView.getHolder();
            if (width != 1280 || height != 720) {
                surfaceHolder.setFixedSize(1280, 720);
            } else {
                mCameraHelper.openCamera(surfaceHolder.getSurface());
            }
        }
    };

    @Override
    protected void onResume() {
        super.onResume();

        //This will invoke the CameraHelper.openCamera upon successful initialization
        mSurfaceView = findViewById(R.id.surfaceView);
        mSurfaceView.getHolder().addCallback(mSurfaceHolderCallback);
    }

    @Override
    protected void onPause() {
        super.onPause();
        mCameraHelper.closeCamera();
        mSurfaceView.getHolder().setFixedSize(0,0); // to force a re-initialization
    }

    @Override
    public void onCameraSetup(Size previewSize) {
        mPreviewSize = previewSize;
        mPreviewByteBufferGray = ByteBuffer.allocate(previewSize.getWidth() * previewSize.getHeight());
    }

    public void onCaptureClicked(View v) {
        synchronized (mPreviewByteBufferGray) {
            final int numFrames = addCalibration8UImage(mPreviewByteBufferGray.array(), mPreviewSize.getWidth(), mPreviewSize.getHeight());
            ((Button)findViewById(R.id.buttonCapture)).setText("Capture " + numFrames);
        }
    }

    public void onCalibrateClicked(View v) {
        doCalibration();
    }

    public void onDoneClicked(View v) {
        Intent intent = new Intent(this, MainActivity.class);
        startActivity(intent);
    }

    @Override
    public void onImageAvailable(ImageReader reader) {
        Image image = reader.acquireLatestImage();
        if (image == null) {
            return;
        }
        ByteBuffer buffer = image.getPlanes()[0].getBuffer();
        buffer.rewind();
        synchronized (mPreviewByteBufferGray) {
            mPreviewByteBufferGray.rewind();
            buffer.get(mPreviewByteBufferGray.array());
        }
        image.close();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        if (requestCode != CameraHelper.REQUEST_PERMISSION_CODE) {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }

        if (grantResults.length <= 0) {
            return;
        }
        if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            mCameraHelper.openCamera(null);
        }
    }

    public static native int addCalibration8UImage(byte[] data, int width, int height);
    public static native void doCalibration();

    static {
        System.loadLibrary("native-lib");
    }
}
