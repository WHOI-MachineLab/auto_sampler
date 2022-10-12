## Open Source Engineered Autonomous Sampler (OpenSEAS)
#WHOI Machine Lab
Ryan C. Galusha: Summer Student Fellow, 2022
Solomon T. Chen: MIT/WHOI JP Student
Matthew H. Long: Department of Marine Chemistry and Geochemistry, WHOI

----
#File Descriptions
<OpenSEAS_interval.ino>
This code triggers the first sampling event at a user specified time. All subsequent sampling events occur at a regular time interval.

A note regarding the Servo.h Arduino IDE library:
This library is written to tell a servo motor to write to a specific position from 0 to 180 degrees ONLY. Some servo motors have the ability to rotate well beyond 180 degrees, including the goBILDA Stingray-4 being used on this project. For these servos, programming with the Aruduino library can be unintuitive because telling the servo to write to position 180 actually results in the servo rotating to its maximum point of rotation. For this reason, a global constant integer, "convertDegrees" is included to convert from "Aduino IDE degrees" to actual degrees of rotation. For example, the maximum rotation allowed by the Stingray-4 is 450 degrees, so "Arduino IDE degrees" are multiplied by the ratio 450/180 to convert to actual degrees.

<<<<<<< HEAD
###### <OpenSEAS_bench-test.ino>
Used for ...

Sample port 4 is home position because...

=======
>>>>>>> parent of c265c0a (Imported CAD files and component datasheets)
More options for sample scheduling to come...