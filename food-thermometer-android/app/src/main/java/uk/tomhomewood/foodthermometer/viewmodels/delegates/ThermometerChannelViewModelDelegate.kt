package uk.tomhomewood.foodthermometer.viewmodels.delegates

import uk.tomhomewood.foodthermometer.model.Thermometer
import uk.tomhomewood.foodthermometer.model.ThermometerBindable
import uk.tomhomewood.foodthermometer.model.ThermometerChannel

class ThermometerChannelViewModelDelegate(val channel: ThermometerChannel): ThermometerBindable {

//    private var thermometer

    var name = "Thermometer name"

    override fun bindThermometer(thermometer: Thermometer) {

    }
}