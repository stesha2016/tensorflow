# TensorFlow Android 司机行为识别

主要的代码是参考//tensorflow/examples/android中的分类应用

## Description

### 1.环境配置

更加详细的信息配置信息可以参考[TensorFlow Android example](../android/README.md)

### 2.应用描述

#### 2.1训练数据
来自kaggle[训练数据](https://www.kaggle.com/c/state-farm-distracted-driver-detection/data)
#### 2.2类别
##### c0: safe driving
##### c1: texting - right
##### c2: talking on the phone - right
##### c3: texting - left
##### c4: talking on the phone - left
##### c5: operating the radio
##### c6: drinking
##### c7: reaching behind
##### c8: hair and makeup
##### c9: talking to passenger
#### 2.3使用网络
考虑到运行速度使用的是mobilenetv2网络
#### 2.4检测速度
在没有任何优化的情况下速度大约是330ms一张


### 3.编译

当SDK和NDK配置好后可以使用下面的命令进行bazel编译:

```bash
bazel build --cxxopt='--std=c++11' -c opt //tensorflow/examples/tf_driver:t2mobile_driver
```

##### Install


```bash
adb install -r bazel-bin/tensorflow/examples/tf_driver/t2mobile_driver.apk
```

## TODO
### 算法加速
在APK中使用mobilenetv2识别一张图大概耗时563ms

1.使用transform_graph进行加速，可以加速到大概439ms，加速了22%
```
bazel-bin/tensorflow/tools/graph_transforms/transform_graph \
--in_graph=freeze_mobilenetv2.pb \
--out_graph=optimized_mobilenetv2.pb \
--inputs='MobilenetV2/input' \
--outputs='MobilenetV2/Predictions/Softmax' \
--transforms='strip_unused_nodes(type=float, shape="1,224,224,3")
        strip_unused_nodes(type=float, shape="1,224,224,3")
        remove_nodes(op=Identity, op=CheckNumerics)
        fold_constants(ignore_errors=true)
        flatten_atrous_conv
        fold_batch_norms
        fold_old_batch_norms
        strip_unused_nodes
        sort_by_execution_order'
```
