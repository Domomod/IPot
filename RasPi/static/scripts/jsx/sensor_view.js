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
      const {temperature, humidity, water_level, light} = this.props;
      return (
        <div className='Sensor-view' ref='slider'>
          <div className='item'>
            <img src="static/img/water_drop.svg"/>
            Humidity: {humidity}
          </div>
          <div className='item'>
            <img src="static/img/thermostat.svg"/>
            Temperature: {temperature} 
          </div>
          <div className='item'>
            <img src="static/img/water.svg"/>
            Water level: {(0.00263157894 * water_level).toFixed(2)} cm
          </div>
          <div className='item'>
            <img src="static/img/light_mode.svg"/>
            Light/Dark: {light}
          </div>
        </div>
      );
    }
});

export {SensorView}