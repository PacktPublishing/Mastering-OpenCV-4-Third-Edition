package com.packt.masteringopencv4.opencvarucoar;

import android.media.ImageReader;
import android.util.Size;

interface CameraHandler extends ImageReader.OnImageAvailableListener {
    void onCameraSetup(final Size previewSize);
}
