import { SensorView } from './sensor_view.js';
import { Slider } from './slider.js';
import { socket } from './app.js';
var ExampleSlider = React.createClass({
  getInitialState() {
    const setSensorData = data => {
      this.setState({
        slides: data
      });
    };

    socket.on('sensor-update', setSensorData);
    socket.emit('sensor-request', {
      'sensor-id': 0
    });
    window.addEventListener('resize', this.setHeight);
    window.addEventListener('touchmove', this.setHeight);
    return {
      slides: [],
      height: 0.7 * window.innerHeight,
      temperature: 1.0,
      humidity: 1.0,
      water_level: 1.0,
      light: 1.0
    };
  },

  emitSensorRequest() {
    socket.emit('sensor-request', null);
  },

  componentDidMount() {
    this.interval = setInterval(this.emitSensorRequest, 1000);
  },

  componentWillUnmount() {
    clearInterval(this.interval);
  },

  addNew() {
    const {
      slides
    } = this.state;
    var newColor = "#" + ((1 << 24) * Math.random() | 0).toString(16);
    console.log(newColor);
    this.setState({
      slides: [...slides, newColor]
    });
  },

  setHeight() {
    const {
      height
    } = this.state;
    var newH = 0.7 * window.innerHeight;

    if (height != newH) {
      console.log("resize");
      this.setState({
        height: newH
      });
    }
  },

  render() {
    console.log("Rendering dynamic slider");
    const {
      slides,
      height
    } = this.state;
    return React.createElement("div", {
      className: 'ExampleSliders'
    }, React.createElement(Slider, null, slides.map(function (object, i) {
      const {
        color,
        humidity,
        temp,
        water,
        light
      } = slides[i];
      return React.createElement("div", {
        className: 'item-view',
        style: {
          height: height,
          background: color
        }
      }, " ", React.createElement(SensorView, {
        humidity: humidity,
        temperature: temp,
        water_level: water,
        light: light
      }), " ");
    }, this)));
  }

});
ReactDOM.render(React.createElement(ExampleSlider, null), document.getElementById('slider1'));