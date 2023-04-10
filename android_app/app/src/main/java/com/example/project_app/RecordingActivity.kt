package com.example.project_app

import android.content.Context
import android.hardware.Sensor
import android.hardware.SensorManager
import android.os.Bundle
import android.widget.TextView
import androidx.activity.compose.setContent
import androidx.appcompat.app.AppCompatActivity
import androidx.compose.foundation.layout.*
import androidx.compose.material.Button
import androidx.compose.material.ButtonDefaults
import androidx.compose.material.MaterialTheme
import androidx.compose.material.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.project_app.ui.theme.TestTheme
import com.example.project_app.ui.theme.spanColor
import java.io.File

class RecordingActivity : AppCompatActivity(), AccelerometerEventListener {

    private var recordingState: Int = 0
    private var recording: Int = 0
    private var fileName: String = ""

    private lateinit var sensorManager: SensorManager
    private var accelerometerEventListener: AccelerometerHandler? = null

    private lateinit var sensor: Sensor
    private lateinit var outView: TextView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            TestTheme {
                RecordingApp()
            }
        }

        sensorManager = getSystemService(Context.SENSOR_SERVICE) as SensorManager

        if (sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER) != null) {
            sensor = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER)
        }
    }

    private fun startListen() {

        accelerometerEventListener = AccelerometerHandler(this)

        if (accelerometerEventListener != null) {
            sensorManager.unregisterListener(accelerometerEventListener)
        }
        sensorManager.registerListener(
            accelerometerEventListener,
            sensor,
            SensorManager.SENSOR_DELAY_NORMAL
        )
    }

    override fun onPause() {
        super.onPause()
        sensorManager.unregisterListener(accelerometerEventListener)
    }

    override fun onStateReceived(state: AccelerometerState) {
        val text = "x=${state.x.format(3).replaceFirst(",", ".")}\n" +
                "y=${state.y.format(3).replaceFirst(",", ".")}\n" +
                "z=${state.z.format(3).replaceFirst(",", ".")}"
        outView.text = text
    }

    fun Double.format(digits: Int) = java.lang.String.format("%.${digits}f", this)

    @Composable
    fun SensorsDisplay() {
        val file = File(fileName)
        val fileExists = file.exists()

        if (fileExists) {
            val sensorData = File(fileName).readText()
            Text(
                text = sensorData,
                color = Color.Green,
                fontSize = 30.sp,
                textAlign = TextAlign.Left
            )
        } else {
            print("No such file: $fileName")
        }
    }

    private fun onButtonClicked() {
        if (recordingState == 0) {
            recording += 1

            val list = listOf("DATA", recording.toString(), ".txt")
            fileName = list.joinToString("")

            val file = File(fileName)
            val isNewFileCreated: Boolean = file.createNewFile()
            if (isNewFileCreated) {
                println("New file: $fileName")
            } else {
                println("$fileName already exists")
            }

            startListen()

            recordingState = 1
        } else {
            recordingState = 0
        }
    }

    @Composable
    fun RecordingScreen(modifier: Modifier = Modifier) {

        Box {
            // Background
            Background()
            // User Interface
            Column(
                modifier = modifier,
                horizontalAlignment = Alignment.CenterHorizontally,
                verticalArrangement = Arrangement.spacedBy(16.dp)

            ) {
                ApplicationName()

                // Spacer(modifier = Modifier.height(16.dp))

                SensorsDisplay()

                Button(
                    onClick = { onButtonClicked() },
                    colors = ButtonDefaults.buttonColors(
                        backgroundColor = MaterialTheme.colors.spanColor,
                        contentColor = Color.White
                    )
                ) {
                    Text(stringResource(R.string.start_recording_button))
                }

                ProjectLogo()

            }
        }
    }

    @Preview(name = "PI Application", showBackground = true, showSystemUi = true)
    @Composable
    fun RecordingApp() {
        RecordingScreen(
            modifier = Modifier
                .fillMaxSize()
                .wrapContentSize(Alignment.Center)
        )
    }
}