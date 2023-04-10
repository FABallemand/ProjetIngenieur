package com.example.project_app

import android.content.Context
import android.hardware.Sensor
import android.hardware.SensorManager
import android.os.Bundle
import androidx.activity.compose.setContent
import androidx.appcompat.app.AppCompatActivity
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
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

    //private var recordingState: Boolean = false
    private var recordingState = mutableStateOf(false)
    private var recording: Int = 0
    private var fileName: String = ""

    private lateinit var sensorManager: SensorManager
    private var accelerometerEventListener: AccelerometerHandler? = null

    private lateinit var sensor: Sensor
    private var sensorDataString = "Sensor Data"

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
        sensorDataString = text
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
        if (!recordingState.value) {
            recording += 1

            /*var isNewFileCreated = false
            while(!isNewFileCreated) {
                val file = File(fileName)
                isNewFileCreated = file.createNewFile()
                if (isNewFileCreated) {
                    println("New file: $fileName")
                } else {
                    println("$fileName already exists")
                    recording += 1
                }
            }*/

            /*val directory: File = applicationContext.getDir("/data", MODE_PRIVATE)
            val file = File(directory, fileName)*/

            var isNewFileCreated = false
            val directory: File = applicationContext.getDir("data", MODE_PRIVATE)
            while (!isNewFileCreated) {
                val list = listOf("DATA", recording.toString(), ".txt")
                fileName = list.joinToString("")
                val file = File(directory, fileName)
                isNewFileCreated = file.createNewFile()
                if (isNewFileCreated) {
                    println("New file: $fileName")
                } else {
                    println("$fileName already exists")
                    recording += 1
                }
            }

            startListen()

            recordingState.value = true
            println("Button clicked true")
        } else {
            recordingState.value = false
            println("Button clicked false")
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

                Spacer(modifier = Modifier.height(16.dp))

                Button(
                    onClick = { onButtonClicked() },
                    colors = ButtonDefaults.buttonColors(
                        backgroundColor = MaterialTheme.colors.spanColor,
                        contentColor = Color.White
                    )
                ) {
                    Text(text = if (recordingState.value) stringResource(R.string.stop_recording_button) else stringResource(R.string.start_recording_button))
                }

                /*val interactionSource = remember { MutableInteractionSource() }
                val isPressed by interactionSource.collectIsPressedAsState()

                Button(
                    onClick = { onButtonClicked() },
                    interactionSource = interactionSource,
                    colors = ButtonDefaults.buttonColors(
                        backgroundColor = MaterialTheme.colors.spanColor,
                        contentColor = Color.White
                    )
                ) {
                    Text(if (isPressed) stringResource(R.string.stop_recording_button) else stringResource(R.string.start_recording_button))
                }*/

                //SensorsDisplay()

                Spacer(modifier = Modifier.height(16.dp))

                /*sensorData = TextView(applicationContext)
                sensorData.text = "Sensor Data"*/

                Spacer(modifier = Modifier.height(16.dp))

                Surface(
                    color = MaterialTheme.colors.spanColor, modifier = Modifier
                        .fillMaxWidth()
                        .clip(
                            RoundedCornerShape(12.dp)
                        )
                ) {
                    Text(
                        text = sensorDataString,
                        color = Color.White,
                        modifier = Modifier.padding(24.dp)
                    )
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