# FPGA-AttendanceChecker

Visit (Here)[https://cpulator.01xz.net/?sys=arm-de1soc] to demo the code through an online DE10-SOC simulator

Emulates the process of validating user attendance upon time of request based on given UserID input.  
References a Database of Valid Student Numbers to check for legibility and formulates output accordingly using 7SD (7 Segment Display)  
--> Output formulated in hex code with respective hexadecimal values to emulate letters and numbers (as done on a calculator display)  
Uses potentiometer of the board on ADC Port 0 to allotted attendance window size per administrator discretion before being validated as late.  

