package com.packt.masteringopencv4.opencvarucoar;

import android.app.Activity;
import android.app.FragmentManager;
import android.content.pm.PackageManager;
import android.media.ImageReader;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.view.Window;
import android.view.WindowManager;

import com.jme3.app.AndroidHarnessFragment;
import com.jme3.texture.Image;
import com.jme3.texture.image.ColorSpace;

import java.nio.ByteBuffer;
import java.util.Arrays;

public class MainActivity extends Activity implements CameraHandler {


    private static final String LOGTAG = "MainActivity";
//    public static final Dictionary DICTIONARY = Dictionary.get(Aruco.DICT_ARUCO_ORIGINAL);
    private ImageHelper mImageHelper;
    private CameraHelper mCameraHelper;
    private ByteBuffer mPreviewByteBufferRGBA;
    private Image mJMEImageRGB;
    private Main jmeApplication;
    private ByteBuffer mPreviewByteBufferGray;
    private JmeFragment jmeFragment;
    HandlerThread mHandlerThread;
    Handler mHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Set window fullscreen and remove title bar
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(
                WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_main);

        Log.i(LOGTAG, "getWindowManager().getDefaultDisplay().getRotation(): " + getWindowManager().getDefaultDisplay().getRotation());

        mImageHelper = new ImageHelper(this);
        mCameraHelper = new CameraHelper(this);
        mCameraHelper.setCameraListener(this);

        // find the fragment
        FragmentManager fm = getFragmentManager();
        jmeFragment = (JmeFragment) fm.findFragmentById(R.id.jmeFragment);
        jmeApplication = (Main) jmeFragment.getJmeApplication();

        mHandlerThread = new HandlerThread("FindMarkerHandlerThread");
        mHandlerThread.start();
        mHandler = new Handler(mHandlerThread.getLooper());
    }

    @Override
    protected void onResume() {
        super.onResume();
        mCameraHelper.openCamera(null);
    }

    @Override
    protected void onPause() {
        super.onPause();
        mCameraHelper.closeCamera();
        mHandlerThread.quit();
    }


    @Override
    public void onCameraSetup(final Size previewSize) {
        Log.d(LOGTAG, "Camera setup: w " + previewSize.getWidth() + " h " + previewSize.getHeight());
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                jmeFragment.getGLView().getHolder().setFixedSize(previewSize.getWidth(), previewSize.getHeight());
            }
        });

        mPreviewByteBufferRGBA = ByteBuffer.allocate(previewSize.getWidth() * previewSize.getHeight() * 4);
        mJMEImageRGB = new Image(Image.Format.RGBA8, previewSize.getWidth(), previewSize.getHeight(), mPreviewByteBufferRGBA, ColorSpace.Linear);
        mPreviewByteBufferGray = ByteBuffer.allocate(previewSize.getWidth() * previewSize.getHeight());
    }

    @Override
    public void onImageAvailable(ImageReader imageReader) {
        android.media.Image image = imageReader.acquireLatestImage();
        if (image == null) {
            return;
        }

        final int w = image.getWidth();
        final int h = image.getHeight();

        //RGB image
        mImageHelper.YUV_420_888_toRGBAIntrinsics(image, mPreviewByteBufferRGBA);
        mJMEImageRGB.setData(mPreviewByteBufferRGBA);
        jmeApplication.setTexture(mJMEImageRGB);
//        rotate180(mPreviewByteBufferRGBA.array(), h, w, CvType.CV_8UC4);
//        timingLogger.addSplit("rotate rgb");

        //Gray image
        mPreviewByteBufferGray.rewind();
        ByteBuffer buffer = image.getPlanes()[0].getBuffer();
        buffer.rewind();
        buffer.get(mPreviewByteBufferGray.array());
        image.close();

        class FindMarkerTask implements Runnable {
            @Override
            public void run() {
//                rotate180(mPreviewByteBufferGray.array(), h, w, CvType.CV_8UC1);
                float[] poseOutput = new float[16];
                boolean markerFound = findMarker(mPreviewByteBufferGray.array(), h, w, poseOutput);
                Log.v(LOGTAG, "marker found? " + markerFound + " pose " + Arrays.toString(poseOutput));
                jmeApplication.setMarkerFound(markerFound, poseOutput);
            }
        }
        if (!mHandler.hasMessages(0)) {
            mHandler.post(new FindMarkerTask());
        }
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

    public static class JmeFragment extends AndroidHarnessFragment {
        public JmeFragment() {
            // Set main project class (fully qualified path)
            appClass = "com.packt.masteringopencv4.opencvarucoar.Main";

            // Set the desired EGL configuration
            eglBitsPerPixel = 24;
            eglAlphaBits = 0;
            eglDepthBits = 16;
            eglSamples = 0;
            eglStencilBits = 0;

            // Set the maximum framerate
            frameRate = -1;

            // Set the maximum resolution dimension
            maxResolutionDimension = -1;

            // Set input configuration settings
            joystickEventsEnabled = false;
            keyEventsEnabled = true;
            mouseEventsEnabled = true;

            // Set application exit settings
            finishOnAppStop = true;
            handleExitHook = true;
            exitDialogTitle = "Do you want to exit?";
            exitDialogMessage = "Use your home key to bring this app into the background or exit to terminate it.";

            // no splash screen
            splashPicID = 0;
        }

        GLSurfaceView getGLView() {
            return this.view;
        }
    }

    static {
        System.loadLibrary("native-lib");
    }

    public static native boolean findMarker(byte[] grayImageAddr, int h, int w, float[] poseOutput);
    public static native void rotate180(byte[] data, int h, int w, int cvtype);
}
