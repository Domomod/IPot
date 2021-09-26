var SensorView = React.createClass({
  getDefaultProps() {
    return {
      temperature: 0.0,
      humidity: 0.0,
      water_level: 0.0,
      light: 0.0
    };
  },

  render() {
    const {
      temperature,
      humidity,
      water_level,
      light
    } = this.props;
    return React.createElement("div", {
      className: 'Sensor-view',
      ref: 'slider'
    }, React.createElement("div", {
      className: 'item'
    }, React.createElement("img", {
      src: "static/img/water_drop.svg"
    }), "Humidity: ", humidity), React.createElement("div", {
      className: 'item'
    }, React.createElement("img", {
      src: "static/img/thermostat.svg"
    }), "Temperature: ", temperature), React.createElement("div", {
      className: 'item'
    }, React.createElement("img", {
      src: "static/img/water.svg"
    }), "Water level: ", (0.00263157894 * water_level).toFixed(2), " cm"), React.createElement("div", {
      className: 'item'
    }, React.createElement("img", {
      src: "static/img/light_mode.svg"
    }), "Light/Dark: ", light));
  }

});
export { SensorView };