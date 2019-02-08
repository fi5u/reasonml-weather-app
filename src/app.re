// Create an option or error type for weather item
type optionOrError('a) =
  | Some('a)
  | None
  | Error;

// State weather can be optional or error
type state = {weather: optionOrError(WeatherData.weatherItem)};

// Actions loaded (with weather item) or error
type action =
  | WeatherLoaded(WeatherData.weatherItem)
  | WeatherError;

// React REDUCER component
let component = ReasonReact.reducerComponent("App");

// Define the component
let make = _children => {
  // Overwrite `component`
  ...component,

  // Set the initial state of the weather to None
  initialState: () => {weather: None},

  // React lifecycle event
  didMount: self => {
    // Send an action
    let handleWeatherLoaded = weather => self.send(WeatherLoaded(weather));
    let handleWeatherError = () => self.send(WeatherError);

    // Fetch the weather data
    WeatherData.getWeather()
    |> Js.Promise.then_(weather => {
         handleWeatherLoaded(weather);
         Js.Promise.resolve();
       })
    |> Js.Promise.catch(_err => {
         handleWeatherError();
         Js.Promise.resolve();
       })
    |> ignore;
  },

  // Reducer called when actions sent
  reducer: (action, _prevState) => {
    switch (action) {
    | WeatherLoaded(newWeather) =>
      // Update the state with new weather
      // Since type state.weather is optional, we need to use `Some` when there is a value
      ReasonReact.Update({weather: Some(newWeather)})
    // Update the state with error
    | WeatherError => ReasonReact.Update({weather: Error})
    };
  },

  // Render app
  // `self` allows for accessing, for example, state
  render: self =>
    <div className="App">
      <p>
        {switch (self.state.weather) {
         | Error =>
           ReasonReact.string("We could not fetch the weather at this time")
         | None => ReasonReact.string("Loading weather...")
         | Some(weather) =>
           ReasonReact.string(
             weather.description ++ " " /*++ string_of_float(weather.temp)*/,
           )
         }}
      </p>
    </div>,
};