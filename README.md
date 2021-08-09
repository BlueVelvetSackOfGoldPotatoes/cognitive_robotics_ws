# Open-Ended Learning Approaches for 3D Object Recognition

###### [Hamidreza Kasaei](https://hkasaei.github.io/) | [cognitive robotics course](https://rugcognitiverobotics.github.io/) | [assignment description](https://github.com/SeyedHamidreza/cognitive_robotics_ws/blob/main/first_assignment_cognitive_robotics.pdf)
##


## Assignment overview
Cognitive science revealed that humans learn to recognize object categories ceaselessly over time. This ability allowsthem to adapt to new environments,  by enhancing their knowledge from the accumulation of experiences and theconceptualization of new object categories. Taking this theory as an inspiration, we seek to create an interactive objectrecognition system that can learn 3D object categories in an open-ended fashion. In this project, “open-ended” implies thatthe set of categories to be learned is not known in advance. The training instances are extracted from on-line experiencesof a robot, and thus become gradually available over time, rather than being completely available at the beginning of thelearning process.Your  goal  for  this  assignment  is  to  implement  an  open-ended  learning  approach for 3D object recognition. 

We break this assignment down into two parts:
1. The first part is about implementing/optimizingoffline 3D object recognition systems, which takean object view as input and produces the categorylabel as output (e.g.,apple,mug,fork, etc).

2. The second part of this assignment is dedicated totesting your approach in an open-ended fashion.In this assignment, the number of categories isnot pre-defined in advance and the knowledge ofagent/robot is increasing over time by interacting with a simulated teacher using three actions: teach, ask, and correct:

<p align="center">
  <img src="images/simulated_user.jpg" width="500" title="">
</p>
<p align="center">
  Abstract architecture for interaction between the simulated teacher and the learning agent
</p>
