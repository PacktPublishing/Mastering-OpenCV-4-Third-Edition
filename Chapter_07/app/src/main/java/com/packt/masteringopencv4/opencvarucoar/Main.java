package com.packt.masteringopencv4.opencvarucoar;

import android.util.Log;

import com.jme3.app.SimpleApplication;
import com.jme3.light.DirectionalLight;
import com.jme3.material.Material;
import com.jme3.math.ColorRGBA;
import com.jme3.math.Matrix3f;
import com.jme3.math.Matrix4f;
import com.jme3.math.Vector3f;
import com.jme3.renderer.Camera;
import com.jme3.renderer.ViewPort;
import com.jme3.scene.Geometry;
import com.jme3.scene.Node;
import com.jme3.scene.debug.Arrow;
import com.jme3.scene.shape.Box;
import com.jme3.scene.shape.Quad;
import com.jme3.texture.Image;
import com.jme3.texture.Texture2D;

public class Main extends SimpleApplication {
    private static final String LOGTAG = "Main";
    private Geometry mBGQuad;
    private Texture2D mCameraTexture;
    private Image mCameraImage;
    private boolean mNewCameraFrameAvailable = false;

    final int mHeightPx = 720;
    final int mWidthPx = 1280;
    final float fgCamNear = 1f;
    final float fgCamFar = 1000f;
    private Node mNode;

    private Matrix4f mPose = Matrix4f.IDENTITY;
    private boolean mMarkerFound = false;

    static final float boxSize = 52.85f / 2.0f;

    @Override
    public void simpleInitApp() {
        viewPort.detachScene(rootNode);

        initVideoBackground(mWidthPx, mHeightPx);
        initForegroundCamera();

        Box b = new Box(boxSize, boxSize, boxSize);
        Geometry mBoxGeom = new Geometry("Box", b);
        mBoxGeom.setLocalTranslation(0, 0, -boxSize);

        Material mat = new Material(assetManager, "Common/MatDefs/Light/Lighting.j3md");
        mat.setBoolean("UseMaterialColors",true);
        mat.setColor("Diffuse",ColorRGBA.Blue);  // minimum material color
        mBoxGeom.setMaterial(mat);

        mNode = createCoordinationNode((int) boxSize * 2);
        mNode.attachChild(mBoxGeom);
        mNode.setLocalScale(1,1,-1);

        rootNode.attachChild(mNode);

        DirectionalLight sun = new DirectionalLight();
        sun.setDirection(new Vector3f(0,0,-1).normalizeLocal());
        sun.setColor(ColorRGBA.White);
        rootNode.addLight(sun);
    }

    public Node createCoordinationNode(float size) {
        Node coordNode = new Node("Axis helper node");
        Material mat;

        for (Vector3f v : new Vector3f[]{Vector3f.UNIT_X, Vector3f.UNIT_Y, Vector3f.UNIT_Z}) {
            Arrow arrow = new Arrow(v.mult(size));
            Geometry axis = new Geometry("Axis " + v.toString(), arrow);
            mat = new Material(assetManager, "Common/MatDefs/Misc/Unshaded.j3md");
            mat.setColor("Color", new ColorRGBA(v.x,v.y,v.z,1.0f));
            axis.setMaterial(mat);
            coordNode.attachChild(axis);
        }

        return coordNode;
    }

    public void initVideoBackground(int screenWidth, int screenHeight) {
        Log.d(LOGTAG, "initVideoBackground: " + screenWidth + " x " + screenHeight);
        Log.d(LOGTAG, "settings: " + settings.getWidth() + " x " + settings.getHeight());
        Log.d(LOGTAG, "fixed: " + mWidthPx + " x " + mHeightPx);

        Quad videoBGQuad = new Quad(1, 1, true);
        mBGQuad = new Geometry("quad", videoBGQuad);
        final float newWidth = (float)screenWidth / (float)screenHeight;
        final float sizeFactor = 0.825f;
        // Center the Geometry in the middle of the screen.
        mBGQuad.setLocalTranslation(-sizeFactor / 2.0f * newWidth, -sizeFactor / 2.0f, 0.f);//
        // Scale (stretch) the width of the Geometry to cover the whole screen width.
        mBGQuad.setLocalScale(sizeFactor * newWidth, sizeFactor, 1);
        Material BGMat = new Material(assetManager, "Common/MatDefs/Misc/Unshaded.j3md");
        mBGQuad.setMaterial(BGMat);

        // Create a new texture which will hold the Android camera preview frame pixels.
        mCameraTexture = new Texture2D();
        BGMat.setTexture("ColorMap", mCameraTexture);

        // Create a custom virtual camera with orthographic projection
        Camera videoBGCam = cam.clone();
        videoBGCam.setParallelProjection(true);
        // Also create a custom viewport.
        ViewPort videoBGVP = renderManager.createMainView("VideoBGView", videoBGCam);
        // Attach the geometry representing the video background to the viewport.
        videoBGVP.attachScene(mBGQuad);
    }

    public void setTexture(final Image image) {
        mCameraImage = image;
        mNewCameraFrameAvailable = true;
    }

    public void setMarkerFound(boolean found, float[] pose4x4) {
        mMarkerFound = found;
        if (found) {
            mPose.set(new Matrix4f(pose4x4).transpose());
            Log.v(LOGTAG, mPose.toString());
        } else {
            mPose.loadIdentity();
        }
    }

    public void initForegroundCamera() {
        Camera fgCam = new Camera(settings.getWidth(), settings.getHeight());
        fgCam.setLocation(new Vector3f(0f, 0f, 0f));
        fgCam.lookAtDirection(Vector3f.UNIT_Z.negateLocal(), Vector3f.UNIT_Y);

        // mock intrinsic parameters
        final float f = 0.625f * mWidthPx;

        // from: http://ksimek.github.io/2013/06/18/calibrated-cameras-and-gluperspective/
        final float fovy = (float)Math.toDegrees(2.0f * (float)Math.atan2(mHeightPx, 2.0f * f));
        final float aspect = (float) mWidthPx / (float) mHeightPx;
        fgCam.setFrustumPerspective(fovy, aspect, fgCamNear, fgCamFar);
        
        Log.d(LOGTAG, "jme proj: " + fgCam.getProjectionMatrix().toString());

        ViewPort fgVP = renderManager.createPostView("ForegroundView", fgCam);
        fgVP.attachScene(rootNode);
        fgVP.setClearFlags(false, true, false);
        fgVP.setBackgroundColor(ColorRGBA.Blue);
    }

    @Override
    public void simpleUpdate(float tpf) {
        if (mNewCameraFrameAvailable && mCameraImage != null && mCameraTexture != null) {
            mCameraTexture.setImage(mCameraImage);
            mNewCameraFrameAvailable = false;
        }

        mNode.setLocalRotation(mPose.toRotationMatrix());
        mNode.setLocalTranslation(mPose.m03,mPose.m13,mPose.m23);

        mBGQuad.updateLogicalState(tpf);
        mBGQuad.updateGeometricState();
    }
}