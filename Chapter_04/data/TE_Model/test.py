# inference using tensorflow
import tensorflow as tf
import numpy as np
import cv2

x_image = cv2.imread('/home/damiles/Projects/Damiles/Mastering-OpenCV-4-Third-Edition/Chapter_05/data/chars_seg/DNN_data/test/5/5_original_0.jpg_7bce2b60-99e1-48b3-b404-095d3d8f221e.jpg', cv2.IMREAD_GRAYSCALE)
x_image = cv2.resize(x_image, dsize=(20, 20))

inputBlob = np.reshape(x_image, [-1, 20, 20, 1])

with open('frozen_cut_graph.pb', 'rb') as f:
    out_graph_def = tf.GraphDef()
    out_graph_def.ParseFromString(f.read())
    tf.import_graph_def(out_graph_def, name="")

    with tf.Session() as sess:
        for n in sess.graph.as_graph_def().node:
            print n.name
        data = sess.graph.get_tensor_by_name("IteratorGetNext:0")
        prediction = sess.graph.get_tensor_by_name("softmax_tensor:0")

        sess.run(tf.global_variables_initializer())
        x_image_out = sess.run(prediction, feed_dict={data: inputBlob})

        print(x_image_out)
        print(np.argmax(x_image_out, 1))



print "==============================="
print "OpenCV DNN"
print "==============================="
# read model and inference
inputBlob=cv2.dnn.blobFromImage(x_image)
net = cv2.dnn.readNetFromTensorflow('frozen_cut_graph.pb')
net.setInput(inputBlob)
result = net.forward()

print(result)
print(np.argmax(result, 1))
