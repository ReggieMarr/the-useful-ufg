# the-useful-ufg
The Useful UFG (universal function generator) was initially conceived as a tool that would be developed as part of a self-learning exercise to gain a deeper understanding of machine learning and its relevant applications to process and manufacturing. This was done under Dr. Tom Wanyama as part of the masters level course ProcTech6AS3.

## Summary
This program is meant to serve as an application which allows for easy integration of machine learning algorithms into a variety of engineering systems (manufacturing, chemical process, embedded, ect). 

It contains a neural network classes which allows for deep learning and can communicate to control systems via modbus standard protocol. (I2C and UART capability soon to come).

The user is then able to set up control methods using the dynamic method creator, save these methods (and associated machine learning models) and connect to systems for real time supervision and control.

Additionally the application allows the user to set up data logging either via a direct connection to a MySQL database or by creating local data logging files.

## Open source resources used

### Qt Creator
### libmodbus
### MySQL C API

***Additionally while the Neural Network functionality was developed by myself it did draw inspiration from [David Miller's Tutorial Video](https://vimeo.com/19569529).


## Default Training Set Format
The training set must be in the format of the following:

  topology: inputlayer hiddenlayer1 outputlayer
  
  in: data1 data2 data3 ect..
  
  out: expectedoutput1 expectedoutput2 expectedoutput3
