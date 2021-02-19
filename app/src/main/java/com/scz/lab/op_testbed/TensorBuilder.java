package com.scz.lab.op_testbed;

import android.content.Context;

import java.io.InputStream;

import ai.onnxruntime.OnnxTensor;
import ai.onnxruntime.OrtEnvironment;
import ai.onnxruntime.OrtException;

public class TensorBuilder {

    private OrtEnvironment env;

    public void setEnv(OrtEnvironment inputEnv) { env = inputEnv; }

    public OnnxTensor buildTensor1() throws OrtException {
        float[][] buffer = new float[1][2];
        return OnnxTensor.createTensor(env, buffer);
    }
    public OnnxTensor buildTensor2() throws OrtException {

        float[][] buffer = new float[1][8];
        return OnnxTensor.createTensor(env, buffer);
    }
    public OnnxTensor buildTensor3() throws OrtException {

        float[][] buffer = new float[1][512];
        return OnnxTensor.createTensor(env, buffer);
    }
    public OnnxTensor buildTensor4() throws OrtException {

        float[][][][] buffer = new float[1][12][128][256];
        return OnnxTensor.createTensor(env, buffer);
    }
}
