import tensorflow as tf
import argparse
import os

BASE_PATH="./chars_seg/DNN_data/"
project_name="ANPR_v2"
train_csv_file=BASE_PATH+"train.tfrecords"
test_csv_file=BASE_PATH+"test.tfrecords"
image_resize=[20,20]

def model_fn(features, labels, mode, params):

    convolutional_2d_1537261701724 = tf.layers.conv2d(
            name="convolutional_2d_1537261701724",
            inputs=features,
            filters=32,
            kernel_size=[5,5],
            strides=(1,1),
            padding="same",
            data_format="channels_last",
            dilation_rate=(1,1),
            activation=tf.nn.relu,
            use_bias=True)

    max_pool_2d_1537261722515 = tf.layers.max_pooling2d(
        name='max_pool_2d_1537261722515',
        inputs=convolutional_2d_1537261701724,
        pool_size=[2,2],
        strides=[2,2],
        padding='same',
        data_format='channels_last')

    convolutional_2d_1537261728442 = tf.layers.conv2d(
            name="convolutional_2d_1537261728442",
            inputs=max_pool_2d_1537261722515,
            filters=64,
            kernel_size=[5,5],
            strides=(1,1),
            padding="same",
            data_format="channels_last",
            dilation_rate=(1,1),
            activation=tf.nn.relu,
            use_bias=True)

    max_pool_2d_1537261754562 = tf.layers.max_pooling2d(
        name='max_pool_2d_1537261754562',
        inputs=convolutional_2d_1537261728442,
        pool_size=[2,2],
        strides=[2,2],
        padding='same',
        data_format='channels_last')

    flatten_1537261781778 = tf.reshape(max_pool_2d_1537261754562, [-1, 1600])

    dense_1537261790190 = tf.layers.dense(inputs=flatten_1537261781778, units=1024, activation=tf.nn.relu)

    dropout_1537261796854= tf.layers.dropout(inputs=dense_1537261790190, rate=0.4, training=mode == tf.estimator.ModeKeys.TRAIN)

    dense_1537261807397 = tf.layers.dense(inputs=dropout_1537261796854, units=30, activation=tf.nn.relu)

    logits=dense_1537261807397

    predictions = {
        "classes": tf.argmax(input=logits, axis=1),
        "probabilities": tf.nn.softmax(logits, name="softmax_tensor")
    }
    #Prediction and training
    if mode == tf.estimator.ModeKeys.PREDICT:
        return tf.estimator.EstimatorSpec(mode=mode, predictions=predictions)

    # Calculate Loss (for both TRAIN and EVAL modes)
    onehot_labels = tf.one_hot(indices=tf.cast(labels, tf.int32), depth=30)
    loss = tf.losses.softmax_cross_entropy(
        onehot_labels=onehot_labels, logits=logits)
    
    # Compute evaluation metrics.
    accuracy = tf.metrics.accuracy(labels=labels,
                                   predictions=predictions["classes"],
                                   name='acc_op')
    metrics = {'accuracy': accuracy}
    tf.summary.scalar('accuracy', accuracy[1])

    # Configure the Training Op (for TRAIN mode)
    if mode == tf.estimator.ModeKeys.TRAIN:
        optimizer = tf.train.GradientDescentOptimizer(learning_rate=0.001)
        train_op = optimizer.minimize(
            loss=loss,
            global_step=tf.train.get_global_step())
        return tf.estimator.EstimatorSpec(mode=mode, loss=loss, train_op=train_op)

    # Add evaluation metrics (for EVAL mode)
    eval_metric_ops = {
        "accuracy": tf.metrics.accuracy(
            labels=labels, predictions=predictions["classes"])}
    return tf.estimator.EstimatorSpec(
        mode=mode, loss=loss, eval_metric_ops=eval_metric_ops)


def _parser_function(example_proto):
    features = {"label": tf.FixedLenFeature((), tf.int64, default_value=0),
                "data": tf.FixedLenFeature((), tf.string, default_value="")
                }
    parsed_features = tf.parse_single_example(example_proto, features)
    image = tf.decode_raw(parsed_features['data'], tf.uint8)
    image = tf.cast(image, tf.float16)
    height = 20
    width = 20

    image_shape = tf.stack([height, width, 1])
    image = tf.reshape(image, image_shape)

    return image, parsed_features["label"]

def data_train_estimator():
    tfrecord_filenames = [train_csv_file]
    dataset = tf.data.TFRecordDataset(tfrecord_filenames)
    dataset = dataset.repeat()
    dataset = dataset.map(_parser_function, num_parallel_calls=100)
    dataset = dataset.batch(100)
    dataset = dataset.shuffle(100)
    iterator = dataset.make_one_shot_iterator()  # create one shot iterator
    feature, label = iterator.get_next()
    return feature, label

def data_test_estimator():
    tfrecord_filenames = [test_csv_file]
    dataset = tf.data.TFRecordDataset(tfrecord_filenames)
    dataset = dataset.map(_parser_function, num_parallel_calls=100)
    dataset = dataset.batch(100)
    iterator = dataset.make_one_shot_iterator()  # create one shot iterator
    feature, label = iterator.get_next()
    return feature, label
        

def build_estimator(model_dir):
    # Create the Estimator
    return tf.estimator.Estimator(
        model_fn=model_fn,
        model_dir=model_dir,
        params={
            # PARAMS
        }
    )

def run_experiment(args):
    """Run the training and evaluate using the high level API"""

    estimator = build_estimator(args.job_dir)

    train_spec = tf.estimator.TrainSpec(input_fn=data_train_estimator, max_steps=20000)
    eval_spec = tf.estimator.EvalSpec(input_fn=data_test_estimator)

    tf.estimator.train_and_evaluate(estimator, train_spec, eval_spec)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    # Input Arguments
    parser.add_argument(
        '--job-dir',
        help='GCS location to write checkpoints and export models',
        required=True
    )
  
    # Argument to turn on all logging
    parser.add_argument(
        '--verbosity',
        choices=[
            'DEBUG',
            'ERROR',
            'FATAL',
            'INFO',
            'WARN'
        ],
        default='INFO',
    )
  
    args = parser.parse_args()
  
    # Set python level verbosity
    tf.logging.set_verbosity(args.verbosity)
    # Set C++ Graph Execution level verbosity
    os.environ['TF_CPP_MIN_LOG_LEVEL'] = str(
        tf.logging.__dict__[args.verbosity] / 10)
  
    # Run the training job
    run_experiment(args)

