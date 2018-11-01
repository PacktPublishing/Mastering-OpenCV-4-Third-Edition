#!/usr/bin/env python

import sys
import os.path
import skimage.io as io
import tensorflow as tf
import pandas as pd
from IPython.display import clear_output
import sys, getopt


def _bytes_feature(value):
    return tf.train.Feature(bytes_list=tf.train.BytesList(value=[value]))


def _int64_feature(value):
    return tf.train.Feature(int64_list=tf.train.Int64List(value=[value]))


def create_tfrecords_from_folder(folder, output_file):
    print "Folder:", folder, "Output:", output_file
    tfrecords_filename = output_file
    writer = tf.python_io.TFRecordWriter(tfrecords_filename)

    
    label = 0
    for dirname, dirnames, filenames in os.walk(folder):
        dirnames.sort()
        for subdirname in dirnames:
            subject_path = os.path.join(dirname, subdirname)
            print subject_path, "with label", label
                
            for filename in os.listdir(subject_path):
                abs_path = "%s/%s" % (subject_path, filename)
                print abs_path
                img = io.imread(abs_path)
                img_raw = img.tostring()
                example = tf.train.Example(features=tf.train.Features(feature={
		            'label': _int64_feature(label),
		            'data': _bytes_feature(img_raw)
		        }))
                writer.write(example.SerializeToString())
            label = label + 1

    writer.close()


    

if __name__ == "__main__":
    inputfolder = ''
    outputfile = ''
    try:
        opts, args = getopt.getopt(sys.argv[1:],"hi:o:",["ifolder=","ofile="])
    except getopt.GetoptError:
        print 'test.py -i <inputfolder> -o <outputfile>'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print 'test.py -i <inputfolder> -o <outputfile>'
            sys.exit()
        elif opt in ("-i", "--ifolder"):
            inputfolder = arg
        elif opt in ("-o", "--ofile"):
            outputfile = arg

    create_tfrecords_from_folder(inputfolder, outputfile)