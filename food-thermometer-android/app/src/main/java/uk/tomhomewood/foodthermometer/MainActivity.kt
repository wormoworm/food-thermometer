package uk.tomhomewood.foodthermometer

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material.MaterialTheme
import androidx.compose.material.Surface
import androidx.compose.material.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import androidx.lifecycle.viewmodel.compose.viewModel
import uk.tomhomewood.foodthermometer.model.Thermometer
import uk.tomhomewood.foodthermometer.ui.theme.FoodThermometerTheme
import uk.tomhomewood.foodthermometer.viewmodels.HomeViewModel

class MainActivity : ComponentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            FoodThermometerTheme {
                // A surface container using the 'background' color from the theme
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colors.background
                ) {
                    TemperatureView(Thermometer("foodthermometer1"))
                }
            }
        }
    }
}

@Composable
fun TemperatureView(thermometer: Thermometer) {
    val viewModel: HomeViewModel = viewModel()
    viewModel.bindThermometer(thermometer)
    Text(text = viewModel.channel1Delegate.name)
}

@Preview(showBackground = true)
@Composable
fun DefaultPreview() {
    FoodThermometerTheme {
        TemperatureView(Thermometer("foodthermometer1"))
    }
}