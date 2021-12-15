WHAT:

A software that gets earthquake data (location, magnitude, date) from an external API and display it visualy
    
HOW:
    
By using [this API](https://earthquake.usgs.gov/fdsnws/event/1/) we have access to earthquake data for a given time period
We will then use OpenGL to display the data retrieve from the API
    
EXPECTED RESULTS:

To be easy to visualize where an earthquake happened and how strong it was

HOW TO RUN:

Open terminal and navigate to /main folder
Run commands:
    
    make
    ./main 

RESULTS:

![Alt Text](https://raw.githubusercontent.com/mathiasVoelcker/csci596-final-project/master/earthquake.gif)

The litthe squares on earth represents location that a earthquake did happened, the more red, the higher the magnitude.
