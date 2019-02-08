// Load the weather data api key from .env file
[@bs.val]
external weatherdataApiKey: string =
  "process.env.REACT_APP_WEATHERDATA_API_KEY";

// Type for single weather item
type weatherItem = {
  id: int,
  main: string,
  description: string,
  icon: string,
};

// Type for array of weather items
type weatherItems = {weather: array(weatherItem)};

// Get the first weather item in the list
let getFirstWeatherItem = (items: weatherItems) => {
  ArrayLabels.get(items.weather, 0);
};

// Decode a single weather item from JSON
let decodeWeatherItem = json =>
  Json.Decode.{
    id: json |> field("id", int),
    main: json |> field("main", string),
    description: json |> field("description", string),
    icon: json |> field("icon", string),
  };

// Decode array of weather items from JSON
let decodeWeatherItems = json =>
  Json.Decode.{weather: json |> field("weather", array(decodeWeatherItem))}
  |> getFirstWeatherItem;

// API url to query
let url =
  "http://api.openweathermap.org/data/2.5/weather?q=London,uk&APPID="
  ++ weatherdataApiKey;

// Parse out a single weather item from JSON
// Returns a single weather item
let parseWeatherResultsJson = json =>
  Json.parseOrRaise(json) |> decodeWeatherItems;

// Get the weather from API endpoint and process a single weather item
let getWeather = () => {
  Js.Promise.(
    Bs_fetch.fetch(url)
    |> then_(Bs_fetch.Response.text)
    |> then_(jsonText =>
         switch (parseWeatherResultsJson(jsonText)) {
         | exception e => reject(e)
         | weather => resolve(weather)
         }
       )
  );
};