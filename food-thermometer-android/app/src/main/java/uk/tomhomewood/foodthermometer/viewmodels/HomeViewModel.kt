package uk.tomhomewood.foodthermometer.viewmodels

import androidx.lifecycle.ViewModel
import uk.tomhomewood.foodthermometer.model.Thermometer
import uk.tomhomewood.foodthermometer.model.ThermometerBindable
import uk.tomhomewood.foodthermometer.model.ThermometerChannel
import uk.tomhomewood.foodthermometer.viewmodels.delegates.ThermometerChannelViewModelDelegate

class HomeViewModel: ViewModel(), ThermometerBindable {

    val channel1Delegate by lazy { ThermometerChannelViewModelDelegate(ThermometerChannel.CHANNEL_ONE) }
    val channel2Delegate by lazy { ThermometerChannelViewModelDelegate(ThermometerChannel.CHANNEL_TWO) }

    override fun bindThermometer(thermometer: Thermometer) {
        channel1Delegate.bindThermometer(thermometer)
        channel2Delegate.bindThermometer(thermometer)
    }
}