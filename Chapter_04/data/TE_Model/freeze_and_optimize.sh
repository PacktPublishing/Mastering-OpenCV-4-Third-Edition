#!/bin/bash

freeze_graph --input_graph=graph.pbtxt --input_checkpoint=model.ckpt-20000 --output_graph frozen_graph.pb --output_node_names=softmax_tensor

~/HDD_data_linux/sdks/tensorflow/bazel-bin/tensorflow/tools/graph_transforms/transform_graph --in_graph="frozen_graph.pb" --out_graph="frozen_cut_graph.pb" --inputs="IteratorGetNext" --outputs="softmax_tensor" --transforms='strip_unused_nodes(type=half, shape="1,20,20,1") fold_constants(ignore_errors=true) fold_batch_norms fold_old_batch_norms sort_by_execution_order'

python ~/HDD_data_linux/sdks/tensorflow/tensorflow/python/tools/optimize_for_inference.py --input frozen_cut_graph.pb --output frozen_cut_graph_opt.pb --frozen_graph True --input_names IteratorGetNext --output_names softmax_tensor