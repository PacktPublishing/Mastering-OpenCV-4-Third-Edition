package com.packt.masteringopencv4.opencvarucoar;

import android.media.ImageReader;
import android.util.Size;

/**
 * Created by hilab on 12/1/17.
 */

interface CameraHandler extends ImageReader.OnImageAvailableListener {
    void onCameraSetup(final Size previewSize);
}
