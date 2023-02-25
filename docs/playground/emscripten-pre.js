// -*- js -*-
var printCallback;
Module['print'] = function(text) { if (printCallback) { printCallback(text + "\n"); } };
