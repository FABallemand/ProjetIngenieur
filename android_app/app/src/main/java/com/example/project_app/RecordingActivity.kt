package com.example.project_app

import android.app.Activity
import android.content.Intent
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import android.net.Uri
import android.os.Bundle
import android.os.Environment
import android.os.StrictMode
import android.os.StrictMode.VmPolicy
import androidx.activity.compose.setContent
import androidx.activity.result.contract.ActivityResultContracts
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
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.documentfile.provider.DocumentFile
import com.example.project_app.ui.theme.TestTheme
import com.example.project_app.ui.theme.spanColor
import kotlinx.coroutines.flow.internal.NoOpContinuation.context
import java.io.File
import kotlin.coroutines.jvm.internal.CompletedContinuation.context


class RecordingActivity : AppCompatActivity(), SensorEventListener {

    // Accelerometer
    private lateinit var sensorManager: SensorManager

    // Recording
    private var recordingState = mutableStateOf(false)
    private var recording: Int = 0

    // private val directory: File = applicationContext.getDir("data", MODE_PRIVATE)
    private var fileName: String = ""
    private lateinit var folderUri: Uri
    private var sensorDataString: String = "Sensor Data"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            TestTheme {
                RecordingApp()
            }
        }

        requestStorageAccess()
        setUpSensor()
    }

    private val requestStorageAccess = registerForActivityResult(ActivityResultContracts.OpenDocumentTree()) { uri ->
        uri?.let {
            contentResolver.takePersistableUriPermission(uri, Intent.FLAG_GRANT_READ_URI_PERMISSION or Intent.FLAG_GRANT_WRITE_URI_PERMISSION)

            // Use the URI to access or write to the selected directory
        }
        folderUri = uri as Uri
    }

    private fun requestStorageAccess() {
        requestStorageAccess.launch(null)
    }

    private fun setUpSensor() {
        // Accelerometer
        sensorManager = getSystemService(SENSOR_SERVICE) as SensorManager
        sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER)?.also {
            sensorManager.registerListener(
                this,
                it,
                SensorManager.SENSOR_DELAY_FASTEST,
                SensorManager.SENSOR_DELAY_FASTEST
            )
        }
    }

    override fun onSensorChanged(event: SensorEvent?) {
        if (event?.sensor?.type == Sensor.TYPE_ACCELEROMETER) {
            val x = event.values[0]
            val y = event.values[1]
            val z = event.values[2]

            sensorDataString = "${x.toFloat()};${y.toFloat()};${z.toFloat()}"

            if (recordingState.value) {
                File(
                    Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS),
                    fileName
                ).appendText(sensorDataString + "\n")
            }
        }
    }

    override fun onAccuracyChanged(p0: Sensor?, p1: Int) {
        return
    }

    private fun onButtonClicked() {
        if (!recordingState.value) {
            recording += 1

            var isNewFileCreated = false
            while (!isNewFileCreated) {
                val list = listOf("DATA", recording.toString(), ".txt")
                fileName = list.joinToString("")
                val file = File(
                    DocumentFile.fromTreeUri(this, folderUri),
                    fileName
                )
                isNewFileCreated = file.createNewFile()
                if (isNewFileCreated) {
                    println("New file: $fileName")
                } else {
                    println("$fileName already exists")
                    recording += 1
                }
            }

            recordingState.value = true
            println("Start collecting data")
        } else {
            recordingState.value = false
            println("Stop collecting data")

            /*val emailIntent = Intent(
                Intent.ACTION_SEND
            ).apply {
                type = "application/octet-stream"
                putExtra(Intent.EXTRA_EMAIL, "fabien.allemand@etu.unistra.fr")
                putExtra(Intent.EXTRA_SUBJECT, fileName)
                putExtra(Intent.EXTRA_STREAM, Uri.fromFile(File(fileName)))
            }
            startActivity(Intent.createChooser(emailIntent, fileName))*/
        }
    }

    private fun onCleaningButtonClicked() {
        var fileId = 0
        var fileExists = true
        while (fileExists) {
            fileId += 1
            val list = listOf("DATA", fileId.toString(), ".txt")
            fileName = list.joinToString("")
            val file = File(
                Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS),
                fileName
            )
            fileExists = file.exists()

            if (fileExists) {
                println("$fileName exists.")
                if (file.delete()) {
                    println("File $fileName deleted successfully.")
                } else {
                    println("Error in deleting file $fileName.")
                }
            } else {
                println("$fileName does not exist.")
            }
        }

        recording = 0

        println("All files deleted successfully.")
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

                //Spacer(modifier = Modifier.height(16.dp))

                Button(
                    onClick = { onButtonClicked() },
                    colors = ButtonDefaults.buttonColors(
                        backgroundColor = MaterialTheme.colors.spanColor,
                        contentColor = Color.White
                    )
                ) {
                    Text(
                        text = if (recordingState.value) stringResource(R.string.stop_recording_button) else stringResource(
                            R.string.start_recording_button
                        )
                    )
                }

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

                Button(
                    onClick = { onCleaningButtonClicked() },
                    colors = ButtonDefaults.buttonColors(
                        backgroundColor = MaterialTheme.colors.spanColor,
                        contentColor = Color.White
                    )
                ) {
                    Text(text = stringResource(R.string.cleaning_button))
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

    override fun onDestroy() {
        sensorManager.unregisterListener(this)
        super.onDestroy()
    }
}