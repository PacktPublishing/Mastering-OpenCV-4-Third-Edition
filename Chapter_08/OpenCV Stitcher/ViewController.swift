//
//  ViewController.swift
//  OpenCV Stitcher
//
//  Created by roy_shilkrot on 10/23/18.
//  Copyright © 2018 roy_shilkrot. All rights reserved.
//

import UIKit
import AVFoundation
import Photos

class ViewController: UIViewController, AVCapturePhotoCaptureDelegate {

    private lazy var captureSession: AVCaptureSession = {
        let s = AVCaptureSession()
        s.sessionPreset = .photo
        return s
    }()
    private let backCamera: AVCaptureDevice? = AVCaptureDevice.default(.builtInWideAngleCamera, for: .video, position: .back)

    private lazy var photoOutput: AVCapturePhotoOutput = {
        let o = AVCapturePhotoOutput()
        o.setPreparedPhotoSettingsArray([AVCapturePhotoSettings(format: [AVVideoCodecKey: AVVideoCodecType.jpeg])], completionHandler: nil)
        return o
    }()
    var capturePreviewLayer: AVCaptureVideoPreviewLayer?
    var deviceOrientationOnCapture: UIDeviceOrientation?
    var capturedImages = [UIImage]()
    var capturedImagesViews = [UIImageView]()
    
    @IBOutlet weak var cameraPreviewView: UIView!
    @IBOutlet weak var prevImageView: UIImageView!

    override func viewDidLoad() {
        super.viewDidLoad()
        
        capturedImagesViews += [self.view.viewWithTag(2)! as! UIImageView,
                                self.view.viewWithTag(3)! as! UIImageView,
                                self.view.viewWithTag(4)! as! UIImageView,
                                self.view.viewWithTag(5)! as! UIImageView]
        
        setupInputOutput()
        setupPreviewLayer()
        startRunningCaptureSession()
        
        print("Opencv version \(StitchingWrapper.openCVVersionString())")
    }
    
    @IBAction func captureButton_TouchUpInside(_ sender: UIButton) {
        photoOutput.capturePhoto(with: AVCapturePhotoSettings(), delegate: self)
    }

    @IBAction func stitch_TouchUpInside(_ sender: Any) {
        let image = StitchingWrapper.stitch(NSMutableArray(array: capturedImages, copyItems: true))
        if image != nil {
            print("result image size: \(image!.size)");
            
            PHPhotoLibrary.shared().performChanges({
                PHAssetChangeRequest.creationRequestForAsset(from: image!)
            }, completionHandler: nil)
            
            let storyboard = UIStoryboard(name: "Main", bundle: nil)
            let viewController = storyboard.instantiateViewController(withIdentifier: "panoViewController") as! PanoViewController
            self.present(viewController, animated: true, completion: nil)
        }
    }
    
    func setupInputOutput() {
        do {
            let captureDeviceInput = try AVCaptureDeviceInput(device: backCamera!)
            captureSession.addInput(captureDeviceInput)
            captureSession.addOutput(photoOutput)
        } catch {
            print(error)
        }
    }
    
    func setupPreviewLayer() {
        capturePreviewLayer = AVCaptureVideoPreviewLayer(session: captureSession)
        capturePreviewLayer?.videoGravity = AVLayerVideoGravity.resizeAspect
        capturePreviewLayer?.connection?.videoOrientation = AVCaptureVideoOrientation.portrait
        
        let previewViewLayer = self.view.viewWithTag(1)!.layer
        capturePreviewLayer?.frame = previewViewLayer.bounds
        previewViewLayer.insertSublayer(capturePreviewLayer!, at: 0)
        previewViewLayer.masksToBounds = true
    }
    
    func startRunningCaptureSession() {
        captureSession.startRunning()
    }
    
    func updateImagesPreview() {
        for (index, img) in capturedImages.enumerated() {
            capturedImagesViews[index].image = img
        }
    }
    
    func photoOutput(_ output: AVCapturePhotoOutput, didFinishProcessingPhoto photo: AVCapturePhoto, error: Error?) {
        let cgImage = photo.cgImageRepresentation()!.takeRetainedValue()
        let image = UIImage(cgImage: cgImage, scale: 1, orientation:  deviceOrientationOnCapture!.imageOrientation)
        prevImageView.image = image
        capturedImages += [image]
        updateImagesPreview()
        
        PHPhotoLibrary.shared().performChanges({
            PHAssetChangeRequest.creationRequestForAsset(from: image)
        }, completionHandler: nil)
    }

    func photoOutput(_ output: AVCapturePhotoOutput,
                     willBeginCaptureFor resolvedSettings: AVCaptureResolvedPhotoSettings)
    {
        // get device orientation on capture
        self.deviceOrientationOnCapture = UIDevice.current.orientation
    }
}

extension UIDeviceOrientation {
    var imageOrientation: UIImage.Orientation {
        switch self {
        case .portrait, .faceUp:                return .right
        case .portraitUpsideDown, .faceDown:    return .left
        case .landscapeLeft:                    return .up
        case .landscapeRight:                   return .down
        case .unknown:                          return .up
        }
    }
}
