const opts = {
    angle: -0.2, // The span of the gauge arc
    lineWidth: 0.2, // The line thickness
    radiusScale: 1, // Relative radius
    pointer: {
      length: 0.6, // // Relative to gauge radius
      strokeWidth: 0.035, // The thickness
      color: '#000000' // Fill color
    },
    limitMax: false,     // If false, max value increases automatically if value > maxValue
    limitMin: false,     // If true, the min value of the gauge will be fixed
    colorStart: '#F75F45',   // Colors
    colorStop: '#59C5DB',    // just experiment with them
    strokeColor: '#E0E0E0',  // to see which ones work best for you
    generateGradient: true,
    highDpiSupport: true,     // High resolution support
    
  };
const target = document.getElementById('temperatura'); // your canvas element
let gaugeTemperatura = new Gauge(target).setOptions(opts); // create sexy gauge!
gaugeTemperatura.maxValue = 3000; // set max gauge value
gaugeTemperatura.setMinValue(0);  // Prefer setter over gauge.minValue = 0
gaugeTemperatura.animationSpeed = 32; // set animation speed (32 is default value)
gaugeTemperatura.set(875); // set actual value