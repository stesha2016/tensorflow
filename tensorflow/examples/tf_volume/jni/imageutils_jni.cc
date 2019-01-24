/* Copyright 2015 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

// This file binds the native image utility code to the Java class
// which exposes them.

#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <fcntl.h>
#include "math.h"
#include <pthread.h>

#include "./rgb2yuv.h"
#include "./yuv2rgb.h"

#define IMAGEUTILS_METHOD(METHOD_NAME) \
  Java_org_tensorflow_demo_env_ImageUtils_##METHOD_NAME  // NOLINT

#define LOG_TAG "GridCam-JNI"
#define TOF_PIPE_FILE "/data/tof/fifo"
#define PIXEL_DIMENSIONS            0.000310f
#define FOCAL_LENGTHS               0.357f
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
IMAGEUTILS_METHOD(convertYUV420SPToARGB8888)(
    JNIEnv* env, jclass clazz, jbyteArray input, jintArray output,
    jint width, jint height, jboolean halfSize);

JNIEXPORT void JNICALL IMAGEUTILS_METHOD(convertYUV420ToARGB8888)(
    JNIEnv* env, jclass clazz, jbyteArray y, jbyteArray u, jbyteArray v,
    jintArray output, jint width, jint height, jint y_row_stride,
    jint uv_row_stride, jint uv_pixel_stride, jboolean halfSize);

JNIEXPORT void JNICALL IMAGEUTILS_METHOD(convertYUV420SPToRGB565)(
    JNIEnv* env, jclass clazz, jbyteArray input, jbyteArray output, jint width,
    jint height);

JNIEXPORT void JNICALL
IMAGEUTILS_METHOD(convertARGB8888ToYUV420SP)(
    JNIEnv* env, jclass clazz, jintArray input, jbyteArray output,
    jint width, jint height);

JNIEXPORT void JNICALL
IMAGEUTILS_METHOD(convertRGB565ToYUV420SP)(
    JNIEnv* env, jclass clazz, jbyteArray input, jbyteArray output,
    jint width, jint height);

//Add by stesha start
JNIEXPORT jint JNICALL IMAGEUTILS_METHOD(startGridCamReader)(
    JNIEnv* env, jclass clazz);
JNIEXPORT jint JNICALL IMAGEUTILS_METHOD(getGridCamPixel)(
    JNIEnv* env, jclass clazz);
JNIEXPORT void JNICALL IMAGEUTILS_METHOD(stopGridCamReader)(
    JNIEnv* env, jclass clazz);
//Add by stesha end

#ifdef __cplusplus
}
#endif

JNIEXPORT void JNICALL
IMAGEUTILS_METHOD(convertYUV420SPToARGB8888)(
    JNIEnv* env, jclass clazz, jbyteArray input, jintArray output,
    jint width, jint height, jboolean halfSize) {
  jboolean inputCopy = JNI_FALSE;
  jbyte* const i = env->GetByteArrayElements(input, &inputCopy);

  jboolean outputCopy = JNI_FALSE;
  jint* const o = env->GetIntArrayElements(output, &outputCopy);

  if (halfSize) {
    ConvertYUV420SPToARGB8888HalfSize(reinterpret_cast<uint8_t*>(i),
                                      reinterpret_cast<uint32_t*>(o), width,
                                      height);
  } else {
    ConvertYUV420SPToARGB8888(reinterpret_cast<uint8_t*>(i),
                              reinterpret_cast<uint8_t*>(i) + width * height,
                              reinterpret_cast<uint32_t*>(o), width, height);
  }

  env->ReleaseByteArrayElements(input, i, JNI_ABORT);
  env->ReleaseIntArrayElements(output, o, 0);
}

JNIEXPORT void JNICALL IMAGEUTILS_METHOD(convertYUV420ToARGB8888)(
    JNIEnv* env, jclass clazz, jbyteArray y, jbyteArray u, jbyteArray v,
    jintArray output, jint width, jint height, jint y_row_stride,
    jint uv_row_stride, jint uv_pixel_stride, jboolean halfSize) {
  jboolean inputCopy = JNI_FALSE;
  jbyte* const y_buff = env->GetByteArrayElements(y, &inputCopy);
  jboolean outputCopy = JNI_FALSE;
  jint* const o = env->GetIntArrayElements(output, &outputCopy);

  if (halfSize) {
    ConvertYUV420SPToARGB8888HalfSize(reinterpret_cast<uint8_t*>(y_buff),
                                      reinterpret_cast<uint32_t*>(o), width,
                                      height);
  } else {
    jbyte* const u_buff = env->GetByteArrayElements(u, &inputCopy);
    jbyte* const v_buff = env->GetByteArrayElements(v, &inputCopy);

    ConvertYUV420ToARGB8888(
        reinterpret_cast<uint8_t*>(y_buff), reinterpret_cast<uint8_t*>(u_buff),
        reinterpret_cast<uint8_t*>(v_buff), reinterpret_cast<uint32_t*>(o),
        width, height, y_row_stride, uv_row_stride, uv_pixel_stride);

    env->ReleaseByteArrayElements(u, u_buff, JNI_ABORT);
    env->ReleaseByteArrayElements(v, v_buff, JNI_ABORT);
  }

  env->ReleaseByteArrayElements(y, y_buff, JNI_ABORT);
  env->ReleaseIntArrayElements(output, o, 0);
}

JNIEXPORT void JNICALL IMAGEUTILS_METHOD(convertYUV420SPToRGB565)(
    JNIEnv* env, jclass clazz, jbyteArray input, jbyteArray output, jint width,
    jint height) {
  jboolean inputCopy = JNI_FALSE;
  jbyte* const i = env->GetByteArrayElements(input, &inputCopy);

  jboolean outputCopy = JNI_FALSE;
  jbyte* const o = env->GetByteArrayElements(output, &outputCopy);

  ConvertYUV420SPToRGB565(reinterpret_cast<uint8_t*>(i),
                          reinterpret_cast<uint16_t*>(o), width, height);

  env->ReleaseByteArrayElements(input, i, JNI_ABORT);
  env->ReleaseByteArrayElements(output, o, 0);
}

JNIEXPORT void JNICALL
IMAGEUTILS_METHOD(convertARGB8888ToYUV420SP)(
    JNIEnv* env, jclass clazz, jintArray input, jbyteArray output,
    jint width, jint height) {
  jboolean inputCopy = JNI_FALSE;
  jint* const i = env->GetIntArrayElements(input, &inputCopy);

  jboolean outputCopy = JNI_FALSE;
  jbyte* const o = env->GetByteArrayElements(output, &outputCopy);

  ConvertARGB8888ToYUV420SP(reinterpret_cast<uint32_t*>(i),
                            reinterpret_cast<uint8_t*>(o), width, height);

  env->ReleaseIntArrayElements(input, i, JNI_ABORT);
  env->ReleaseByteArrayElements(output, o, 0);
}

JNIEXPORT void JNICALL
IMAGEUTILS_METHOD(convertRGB565ToYUV420SP)(
    JNIEnv* env, jclass clazz, jbyteArray input, jbyteArray output,
    jint width, jint height) {
  jboolean inputCopy = JNI_FALSE;
  jbyte* const i = env->GetByteArrayElements(input, &inputCopy);

  jboolean outputCopy = JNI_FALSE;
  jbyte* const o = env->GetByteArrayElements(output, &outputCopy);

  ConvertRGB565ToYUV420SP(reinterpret_cast<uint16_t*>(i),
                          reinterpret_cast<uint8_t*>(o), width, height);

  env->ReleaseByteArrayElements(input, i, JNI_ABORT);
  env->ReleaseByteArrayElements(output, o, 0);
}

//Add by stesha start
bool mIsRunning = false;
int mDistance = 0;
pthread_t mReaderThread;
void* readDistanceThread(void* data) {
  int pipefd = 0;
  char cache[10];
  int err = 0;

  pipefd = open(TOF_PIPE_FILE, O_RDONLY | O_NONBLOCK);
  LOGI("gridcam==>open %s return: %s\n", TOF_PIPE_FILE, strerror(errno));
  if(pipefd < 0) {
    LOGI("gridcam==>open file failed");
    return NULL;
  }

  while(mIsRunning) {
    memset(cache, 0x0, sizeof(cache));
    err = read(pipefd, cache, 10);
    if(err <= 0) {
      continue;
    }
    mDistance = atoi(cache);
    LOGI("gridcam==>mDistance: %d", mDistance);
  }

  close(pipefd);
  return (void* )NULL;
}

JNIEXPORT jint JNICALL IMAGEUTILS_METHOD(startGridCamReader)(
    JNIEnv* env, jclass clazz){
  LOGI("gridcam==>startGridCamReader");

  int rc = 0;
  rc= pthread_create(&mReaderThread, NULL, readDistanceThread, NULL);
  if (!rc) {
    LOGI("gridcam==>startGridCamReader success");
  }
  mIsRunning = true;

  return rc;
}

JNIEXPORT void JNICALL IMAGEUTILS_METHOD(stopGridCamReader)(
    JNIEnv* env, jclass clazz){
  LOGI("gridcam==>stopGridCamReader");
  mIsRunning = false;
  if (mReaderThread != 0) {
    if (pthread_join(mReaderThread, NULL) != 0) {
      LOGI("gridcam==>%s pthread dead already", __func__);
    }
    mReaderThread = 0;
  }
}

JNIEXPORT jint JNICALL IMAGEUTILS_METHOD(getGridCamPixel)(
    JNIEnv* env, jclass clazz){
  float value = 0;
  int width = 640;

  if (mDistance <= 0) {
    return 0;
  } 
  value = mDistance * 1.0f / 10;

  //1. Get cropFactor from width and height
  int maxWidth=4160;
  double cropFactor = 1.f;
  cropFactor = maxWidth * 1.0f / width;
  LOGI("gridcam==>read file cropFactor = %f", cropFactor);

  //2. calculate space
  int spacing = 0;
  spacing = round(FOCAL_LENGTHS / (value * PIXEL_DIMENSIONS * cropFactor));
  LOGI("gridcam==>spacing = %d", spacing);

  return spacing;
}

/*JNIEXPORT jint JNICALL IMAGEUTILS_METHOD(startGridCamReader)(
    JNIEnv* env, jclass clazz){
  LOGI("gridcam==>startGridCamReader");
  int pipefd = 0;

  pipefd = open(TOF_PIPE_FILE, O_RDONLY | O_NONBLOCK);
  LOGI("gridcam==>open file pipefd = %d", pipefd);
  LOGI("gridcam==>open %s return: %s\n", TOF_PIPE_FILE, strerror(errno));
  if(pipefd < 0) {
    LOGI("gridcam==>open file failed");
    return 0;
  }
  return pipefd;
}

JNIEXPORT jint JNICALL IMAGEUTILS_METHOD(getGridCamPixel)(
    JNIEnv* env, jclass clazz, jint pipefd){
  LOGI("gridcam==>getGridCamPixel pipefd = %d", pipefd);
  char cache[10];
  int err = 0;
  float value = 0;
  int width = 640;
  int i = 0;

  //1. Get distance info from tof
  memset(cache, 0x0, sizeof(cache));
  err = read(pipefd, cache, 10);
  LOGI("gridcam==>read file return = %d", err);
  LOGI("gridcam==>read %s return: %s\n", TOF_PIPE_FILE, strerror(errno));
  if(err <= 0) {
    for(i = 0; i < 100; i++) {
      err = read(pipefd, cache, 10);
      LOGI("gridcam==>try again read return: %d", err);
      if (err > 0) {
        break;
      }
      LOGI("gridcam==>try again read %s return: %s\n", TOF_PIPE_FILE, strerror(errno));
    }
    LOGI("gridcam==>loop end: %d", err);
    if(err <= 0) {
      return 0;
    }
  }
  value = atoi(cache) * 1.0f / 10;
  LOGI("gridcam==>read file value = %f", value);

  //2. Get cropFactor from width and height
  int maxWidth=4160;
  double cropFactor = 1.f;
  cropFactor = maxWidth * 1.0f / width;
  LOGI("gridcam==>read file cropFactor = %f", cropFactor);

  //3. calculate space
  int spacing = 0;
  spacing = round(FOCAL_LENGTHS / (value * PIXEL_DIMENSIONS * cropFactor));
  LOGI("gridcam==>spacing = %d", spacing);

  return spacing;
}

JNIEXPORT void JNICALL IMAGEUTILS_METHOD(stopGridCamReader)(
    JNIEnv* env, jclass clazz, jint pipefd){
  LOGI("gridcam==>stopGridCamReader");
  close(pipefd);
}*/
//Add by stesha end
