package com.example.project_app

class AccelerometerState {
    var x: Double = 0.0
    var y: Double = 0.0
    var z: Double = 0.0
    var time: Long = 0

    constructor(x: Double, y: Double, z: Double, time: Long) {
        this.x = x
        this.y = y
        this.z = z
        this.time = time
    }
}