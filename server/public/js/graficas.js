let minTemp = 26, maxTemp = 28, tempActual = 0;
let minHum = 80, maxHum = 85, humActual = 0;
const rojo = '#F75F45';
const verde = '#87C15A';
const azul = '#59C5DB';

let optsT = {
    angle: -0.25,
    lineWidth: 0.2,
    radiusScale:0.9,
    pointer: {
      length: 0.6,
      strokeWidth: 0.05,
      color: '#000000'
    },staticLabels: {
        font: "1em Montserrat",
        labels: [0, minTemp, maxTemp, 100],
        fractionDigits: 0
    },
    staticZones: [
       {strokeStyle: azul, min: 0, max: minTemp},
       {strokeStyle: verde, min: minTemp, max: maxTemp},
       {strokeStyle: rojo, min: maxTemp, max: 100}
    ],
    limitMax: false,
    limitMin: false,
    highDpiSupport: true
  };
let targetT = document.getElementById('temperatura'); // your canvas element
let gaugeT = new Gauge(targetT).setOptions(optsT); // create sexy gauge!
gaugeT.minValue = 0;
gaugeT.maxValue = 100;
gaugeT.set(maxTemp);

let optsH = {
    angle: -0.25,
    lineWidth: 0.2,
    radiusScale:0.9,
    pointer: {
      length: 0.6,
      strokeWidth: 0.05,
      color: '#000000'
    },staticLabels: {
        font: "1em Montserrat",
        labels: [0, minHum, maxHum, 100],
        fractionDigits: 0
    },
    staticZones: [
       {strokeStyle: rojo, min: 0, max: minHum},
       {strokeStyle: verde, min: minHum, max: maxHum},
       {strokeStyle: azul, min: maxHum, max: 100}
    ],
    limitMax: false,
    limitMin: false,
    highDpiSupport: true
  };
let targetH = document.getElementById('humedad'); // your canvas element
let gaugeH = new Gauge(targetH).setOptions(optsH); // create sexy gauge!
gaugeH.minValue = 0;
gaugeH.maxValue = 100;
gaugeH.set(0);