

<ul>**Overview**</ul><br>
An Ardiuno sketch for a Lilygo T5 4.7 epaper display. This unit has a builtin ESP32 and 18650 battery holder. I wanted something that could sit on my table on rotate through some Film/Tv artwork. 
<br><br>

<ul>The Highlevel Logic</ul> <br>
<li>A @reboot cron job loads the APIserver at server bootup.</li>
<li>The device boots up, joins WiFi, calls the /load API which starts the imageserver App.</li>
<li>imageserver APP converts a randomly picked jpg from the folder to B64, adjusts the brightness and resolution for the Epaper screen and serves it on /image</li>
<li>Device delays for 20 seconds waiting for /image API to load, then streams the image from /image API, displays it</li>
<li>Device calls /shutdown API which kills the imageserver App. Then goes into sleep mode for the set time (see sleep_duration variable)</li>
<li>loop</li>



<br><br><br>
*Some Notes*<br>

The API Server will need the path  changing to wherever you are running the script from

Imageserver needs the location of the jpgs changing to wherever you store yours. I have symlinked mine to a folder which is scraped with all my Plex movies and TV covers.

The Arduio Sketch will need the IP address of the server running both Apps

The arduino_screts.h file will need to be renamed (delete the SAMPLE_) and updated with your own WiFi creds.
<br><br>
Future Jobs;
<li>convert paths to variables</li>
<li>setup a function to check for night hours and increase sleep length</li>
<li>maybe look at the brightness adj calcs again. Possibly still a little contrasty</li>


<br><br><br>
<b><i>Dan</i></b>

  <br><br>
Also,<br>
Thanks to;<br>
https://github.com/srslynow
For providing the basis for this.
