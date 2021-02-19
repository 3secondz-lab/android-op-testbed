package com.scz.lab.op_testbed

import ai.onnxruntime.OnnxJavaType
import ai.onnxruntime.OnnxTensor
import ai.onnxruntime.OrtEnvironment
import ai.onnxruntime.OrtSession
import android.content.Context
import android.os.SystemClock
import android.util.Log
import java.nio.ByteBuffer

/**
 *  Traffic convention
 *  (1, 0) : left handle
 *  (0, 1) : right handle
 */

class OnnxWrapper(context: Context) {
    val env = OrtEnvironment.getEnvironment()
    var session: OrtSession
    val tensorBuilder = TensorBuilder()

    val trafficBuffer = ByteBuffer.allocateDirect(2).apply {
//        putFloat(1.0f)
//        putFloat(0.0f)
        position(0)
    }
//    val traffic = OnnxTensor.createTensor(env, trafficBuffer, longArrayOf(1, 2), OnnxJavaType.FLOAT)
//    val traffic = OnnxTensor(env, )

    val desireBuffer = ByteBuffer.allocateDirect(8).apply {
//        position(0)
    }
//    val desire = OnnxTensor.createTensor(env, desireBuffer, longArrayOf(1, 8), OnnxJavaType.FLOAT)


    val initStateBuffer = ByteBuffer.allocateDirect(512).apply {
//        position(0)
    }
//    val initState = OnnxTensor.createTensor(env, initStateBuffer, longArrayOf(1, 512), OnnxJavaType.FLOAT)


    init {
//        val modelStream = context.assets.open("supercombo.onnx")
//        val modelStream = context.assets.open("supercombo-old.onnx")
//        val modelStream = context.assets.open("supercombo-keras.onnx")
        val modelStream = context.assets.open("supercombo-nnapi.ort")
        val size = modelStream.available()
        val buffer = ByteArray(size)
        modelStream.read(buffer)
        modelStream.close()
        val option = OrtSession.SessionOptions()
        option.addNnapi()
        option.addConfigEntry("session.load_model_format", "ORT")
        session = env.createSession(buffer, option)
//        session = env.createSession(buffer)


        tensorBuilder.setEnv(env)

        val inputs = session.inputInfo
        for (input in inputs) {
            Log.d("MODEL_INPUT", "${input.key} - ${input.value}")
        }

        val outputs = session.outputInfo
        for (output in outputs) {
            Log.d("MODEL_OUTPUT", "${output.key} - ${output.value}")
        }
    }

    public fun inference(data: ByteBuffer) {

//        val image = OnnxTensor.createTensor(env, data, longArrayOf(1, 12, 128, 256), OnnxJavaType.FLOAT)
        val inputMap = HashMap<String, OnnxTensor>()
        inputMap.put("desire", tensorBuilder.buildTensor2())
        inputMap.put("traffic_convention", tensorBuilder.buildTensor1())
        inputMap.put("input_imgs", tensorBuilder.buildTensor4())
        inputMap.put("initial_state", tensorBuilder.buildTensor3())


        while(true) {

            val initTime = SystemClock.currentThreadTimeMillis()
            session.run(inputMap)

            val finTime = SystemClock.currentThreadTimeMillis()
            Log.d("TIME_SPENT", "${finTime - initTime}")
        }

    }
}