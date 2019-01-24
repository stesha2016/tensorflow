package org.tensorflow.demo;

public class DetectionInfo {
    public DetectionInfo(float vHeight, float hLength) {
        this.vHeight = vHeight;
        this.hLength = hLength;
    }

    public float getvHeight() {
        return vHeight;
    }

    public float gethLength() {
        return hLength;
    }

    public void setvHeight(float vHeight) {
        this.vHeight = vHeight;
    }

    public void sethLength(float hLength) {
        this.hLength = hLength;
    }

    public float vHeight;
    public float hLength;

}
