GO TO jm_final_project


//FINAL IN GAZEBO
Run the Final_sim. launch to see our final structure in Gazebo.
You should open in other terminal keyboardControl in order to have manual control.

1. click on take off.
2. click automatic if u want to use the manual mode.
3. otherway, click on inicio.

Nodes in:

-Brain : The State Machine
- RQT_Commander: dynamic control
- watchmarker_server, yaw_server, z_server are the nodes working as service server for the Brain
-Final_control: A piecewise  controller depending on Toggle/Giro

// FINAL IN REAL

We consider adding the sonar_control node in order to begin in a higher altitude.


// EXPERIMENTATION 

Run the sim_experimentation.launch or Real_experiumentation.launch to see our  structure in Gazebo for experimentation.
You should open in other terminal keyboardControl in order to have manual control.


MORE USER BASED CONTROL

Nodes in:

- Take_land : dynamic control with option to change the toogle and give instructions to do the giro.

- arsuscriber_marker1 : the Controller.


/
