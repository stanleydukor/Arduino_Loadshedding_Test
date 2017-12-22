# Arduino Loadshedding Test

A device that calculates the power consumed by different loads by measuring the current through the load using a current sensor, and then allows users to pay their bills based on consumption via their mobile phones.

# How it works

First of all, 3 current sensors measure the current drawn from 3 different loads respectively, then calculations are made to get the current in RMS (root-mean-squared).

After getting the current consumed for 3 iterations, the current is summed up for each load respectively and using some constants, the cost per usage is calculated and stored in Naira (Nigerian Official Currency).

Next, the three users are asked to pay for the usage (which could be done using an android smart phone with bluetooth or the Arduino Serial Com screen) after which their payment is graded in percentage from the highest to the lowest.

Finally, we vary the Arduino's 5v using a potentiometer (frequency was actually meant to be varied) so that at a particular frequency, all three loaded are connected. If the freuency is reduced to a specific level, the load with lowest pay gets cut-off to stabilize the frequency (with the help of a relay), if reduced again, the next load gets cut-off, and again, the last load gets cut-off.

Thankyou! and feel free to make neccessary corrections to this code.
