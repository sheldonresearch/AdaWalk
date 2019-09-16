MRC Psycholinguistic Database Java Interface v0.9
*************************************************

Francois Mairesse, 25/01/2007
http://www.dcs.shef.ac.uk/~francois

This library is covered by the GNU GENERAL PUBLIC LICENSE, 
see the file license.txt distributed with this program.



INSTALLATION (Linux/Solaris/Windows):

You need to have the 

- 	Java Runtime Environment v1.5 or later installed (available at http://www.sun.com). 

- 	the original MRC Psycholinguistic Database installed (available at 
    http://ota.ahds.ac.uk/texts/1054.html, or ask author)
	
	
USAGE:

The src/Example.java source file contains a simple example of how to use the interface, you can
run it with the command:

java -classpath INSTALL_DIR/jmrc.jar jmrc.Example MRC_DATABASE_DIRECTORY

and you should get query results for the word 'happiness' and 'car'. 

To use is in your Java programs, you just need to make sure than the jmrc.jar file in the
root directory is in your classpath. Additional information about the interface can be 
found in the Javadoc (doc/index.html). Feel free to email the author at F.Mairesse@sheffield.ac.uk
for enquiries.
