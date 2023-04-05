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


class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            TestTheme {
                PIApp()
            }
        }
    }
}

@Composable
fun Background() {
    val viewTPS = painterResource(R.drawable.tps_2)
    Image(
        painter = viewTPS,
        contentDescription = null,
        modifier = Modifier
            .fillMaxHeight()
            .fillMaxWidth(),
        contentScale = ContentScale.Crop
    )
}

@Composable
fun ApplicationName() {
    Column {
        Text(
            text = stringResource(R.string.pi_app_name),
            fontSize = 32.sp,
            fontWeight = FontWeight.Bold,
            color = Color.White,
            modifier = Modifier
                .background(color = MaterialTheme.colors.spanColor)
                .fillMaxWidth()
                .wrapContentWidth(Alignment.CenterHorizontally)
                //.fillMaxHeight()
                .wrapContentHeight(Alignment.Top)
        )
    }
}

@Composable
fun ProjectLogo() {
    Row {
        val logoALE = painterResource(R.drawable.logo_alcatel_1)
        Image(
            painter = logoALE,
            contentDescription = null,
            modifier = Modifier
                .size(190.dp)
                .padding(5.dp)
        )

        val logoTPS = painterResource(R.drawable.logo_tps_1)
        Image(
            painter = logoTPS,
            contentDescription = null,
            modifier = Modifier
                .size(190.dp)
                .padding(5.dp)
        )
    }
}

@Composable
fun EditTextField(
    value: String,
    onValueChange: (String) -> Unit
) {
    TextField(
        label = { Text(text = "Text input") },
        singleLine = true,
        /*keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),*/
        value = value,
        onValueChange = onValueChange,
    )
}


@Composable
fun HomeScreen(modifier: Modifier = Modifier) {
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
                onClick = { context.startActivity(Intent(context, RecordingActivity::class.java)) },
                colors = ButtonDefaults.buttonColors(
                    backgroundColor = MaterialTheme.colors.spanColor,
                    contentColor = Color.White
                )
            ) {
                Text(stringResource(R.string.recording_button))
            }

            Button(
                onClick = { result = 2 },
                colors = ButtonDefaults.buttonColors(
                    backgroundColor = MaterialTheme.colors.spanColor,
                    contentColor = Color.White
                )
            ) {
                Text("Button 2")
            }

            Surface(
                color = MaterialTheme.colors.spanColor, modifier = Modifier.clip(
                    RoundedCornerShape(12.dp)
                )
            ) {
                Text(
                    text = "State: $result",
                    color = Color.White,
                    modifier = Modifier.padding(24.dp)
                )
            }

            var textInput by remember { mutableStateOf("") }
            EditTextField(value = textInput,
                onValueChange = { textInput = it }
            )

            Surface(
                color = MaterialTheme.colors.spanColor, modifier = Modifier.clip(
                    RoundedCornerShape(12.dp)
                )
            ) {
                Text(
                    text = "State: $textInput",
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
fun PIApp() {
    HomeScreen(
        modifier = Modifier
            .fillMaxSize()
            .wrapContentSize(Alignment.Center)
    )
}