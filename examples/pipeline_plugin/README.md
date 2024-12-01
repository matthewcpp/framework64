### Pipeline Plugin

This example demonstrates how to add your own processing code for a particular file extension to the asset pipeline.
A simple plugin is created which transforms a json file containing rectangles to draw into a binary format.
The game loads the binary file and uses a spritebatch to draw the rectangles with the given color.