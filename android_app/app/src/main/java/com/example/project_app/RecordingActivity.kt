package com.example.project_app

import android.content.Intent
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.project_app.ui.theme.spanColor
import com.example.project_app.ui.theme.TestTheme


class RecordingActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            TestTheme {
                RecordingApp()
            }
        }
    }
}

@Composable
fun RecordingScreen(modifier: Modifier = Modifier) {
    val context = LocalContext.current
    var result by remember { mutableStateOf(0) }

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

            Button(
                onClick = {  },
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