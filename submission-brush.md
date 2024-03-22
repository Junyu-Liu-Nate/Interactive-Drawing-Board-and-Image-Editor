## Project 1: Brush

Please fill this out for Brush only. The project handout can be found [here](https://cs1230.graphics/projects/raster/1).

### Output Comparison
This is how you will demonstrate the functionality of your project.

For each of the rows below, follow the instructions to record a video. Please reference the [Screen Recording Guide](https://cs1230.graphics/docs/screen-recording/) for machine-specific guidance on how to do this.

Once you've recorded everything, navigate to this file in Github, click edit, and either select or drag-and-drop each of your videos into the correct location. This will upload them to GitHub (but not include them in the repo) and automatically embed them into this Markdown file by providing a link. Make sure to double-check that they all show up properly in the preview.

> Note: you can ignore the `student_outputs` folder in the repo for now. You will be using it in the next project, Filter.

We're **not** looking for your video to **exactly** match the expected output (i.e. draw the exact same shape). Just make sure to follow the instructions and verify that the recording fully demonstrates the functionality of that part of your project.

#### Constant Brush
**Instructions:** Draw with the constant brush.

##### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/241d99b5-947a-407c-b454-410534520aad

##### Your Output



https://github.com/BrownCSCI1230/projects-raster-Junyu-Liu-Nate/assets/75256586/c5ecc457-b425-47a3-bb3a-4fd3a3e758fc


<!---
Paste your output on top of this comment!
-->



#### Linear Brush
**Instructions:** Draw with the linear brush.

##### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/9706fa04-7710-441f-b292-ab010e04dec6

##### Your Output



https://github.com/BrownCSCI1230/projects-raster-Junyu-Liu-Nate/assets/75256586/2d752441-d97c-4634-a8c7-14c5db67afd0


<!---
Paste your output on top of this comment!
-->



#### Quadratic Brush
**Instructions:** Draw with the quadratic brush.

##### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/c5df5c09-bfe0-4c05-a56e-14609772d675

##### Your Output



https://github.com/BrownCSCI1230/projects-raster-Junyu-Liu-Nate/assets/75256586/869493c6-138f-44d9-8c37-e6db8a2732cc


<!---
Paste your output on top of this comment!
-->



#### Smudge Brush
**Instructions:** Draw some colors on the canvas and use the smudge brush to smear them together.

##### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/26440b63-2d1c-43fd-95f2-55b74ad3bbed

##### Your Output



https://github.com/BrownCSCI1230/projects-raster-Junyu-Liu-Nate/assets/75256586/10b6dac2-c8cd-4f63-ac32-9a20c9ddfa38


<!---
Paste your output on top of this comment!
-->



#### Smudge Brush Change in Alpha
**Instructions:** Draw some colors on the canvas. Use the smudge brush with varying alpha levels (use at least three) and demonstrate that the brush still works the same way each time.

##### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/0b49c7d0-47ca-46d0-af72-48b831dfe7ea

##### Your Output



https://github.com/BrownCSCI1230/projects-raster-Junyu-Liu-Nate/assets/75256586/3adc496f-c808-418b-acec-b1dbfc91dd68


<!---
Paste your output on top of this comment!
-->



#### Radius
**Instructions:** Use any brush with at least 3 different values for the radius.

##### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/6f619df6-80cd-4849-8831-6a5aade2a517

##### Your Output


https://github.com/BrownCSCI1230/projects-raster-Junyu-Liu-Nate/assets/75256586/65c5d0ba-d115-480d-8290-26f861830015


<!---
Paste your output on top of this comment!
-->



#### Color
**Instructions:** Use any brush to draw red (255, 0, 0), green (0, 255, 0), and blue (0, 0, 255).

##### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/fd9578ca-e0af-433e-ac9e-b27db2ceebc9

##### Your Output


https://github.com/BrownCSCI1230/projects-raster-Junyu-Liu-Nate/assets/75256586/3be1bf31-ff41-4088-beaa-23a3be474e9e


<!---
Paste your output on top of this comment!
-->



#### Canvas Edge Behavior
**Instructions:** With any brush, click and draw on the canvas in a place where the mask intersects with the edge. Then, start drawing anywhere on the canvas and drag your mouse off of the edge.

##### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/f5344248-fa5f-4c33-b6df-ff0a45011c7a

##### Your Output


https://github.com/BrownCSCI1230/projects-raster-Junyu-Liu-Nate/assets/75256586/be5e7e99-941a-4ab7-bec3-7173b594d36f


<!---
Paste your output on top of this comment!
-->



#### Alpha
**Instructions:** With the constant brush, draw a single dot of red (255, 0, 0) with an alpha of 255. Then, draw over it with a single dot of blue (0, 0, 255) with an alpha of 100. You should get a purpleish color.

##### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/b13d312d-d6d4-4375-aeaa-96174065443b

##### Your Output


https://github.com/BrownCSCI1230/projects-raster-Junyu-Liu-Nate/assets/75256586/ce3fd5aa-6f67-4b59-b668-6de7ac302a4a


<!---
Paste your output on top of this comment!
-->



#### Alpha of Zero
**Instructions:** Choose any brush and demonstrate that it will not draw if the alpha value is zero.

##### Expected Output

https://github.com/BrownCSCI1230/projects_raster_template/assets/77859770/8e48777e-8196-401e-9af6-871abe712146

##### Your Output


https://github.com/BrownCSCI1230/projects-raster-Junyu-Liu-Nate/assets/75256586/ee7da044-c527-43cc-a258-44bf7d068563

<!---
Paste your output on top of this comment!
-->



### Design Choices

#### Functionality

##### Generals:
- All brushes are stored and updated as masks and updated properly according to the specification.
- All broundary situations are considered and tested.
- All functions are tested according to the demonstration videos and all of the shown operations are functioned normally. Some additional tests are covered including different edge situations or overlapping (e.g., in the output of Fill Brush in the document) to reduce the probability of stability bugs.

##### Explanation for functionalities:
- **Constant/linear/quadratic Brush**: The detailed guidance is covered in the project specifications and in algo1, thus here I don't do additional explanation.

- **Smudge Brush**: The key to implement smudge brush is to store the color where the previous brush the mouse covered, and mix with the brush of the mouse currently covered.

- **Spray Brush**: Spray mask is generated by setting opacity of value 0 or 1  according to the density value. A random seed is generated by calling ```arc4random_uniform(100)``` to generate a random number between 0 and 100, and it is compared to the density value. In such manner, when density is 0, all values are 0 and when density is 100, all values are 1 (constant brush). In other density values, the probability of a value to be 1 is ```arc4random_uniform(100) / density```.
  
- **Speed Brush**: The key to implement the speed brush is to calculate the speed of the mouse dragging, which is implemented by storing the previous location of the mouse dragged and compare the current location to calculate the mouse moving distance in a unit time. Since the event calling/sampling time interval is uniform, such distance can be used to determine the speed of the mouse in a unit time.Then, the radius of brush is calculated by ```(1.0 - scale) * settings.brushRadius + scale * settings.brushRadius / (unitSpeed + 1.0)```, where scale is a parameter to control the changing rate of the radius. Such calculation enables a visually smooth changing of the brush radius and maintains the max radius of ```ettings.brushRadius``` when the speed is 0.

- **Fill**: The main idea for Fill is to implement a recursive filling process where starts at the clicking position of the mouse and iteratively proceed towards the upper left, upper right, lower left, and lower right of the current pixel until all the neighbors are filled with the same color or reach to the boundary of the canvas. To speed up the procdure for filling the whole canvas  when clicking on empty canvas, there is a pre-check for whether the canvas is empty and fill the empty canvas by directly assign every entry of the canvas array to the designated color.

- **Fixed Alpha Blending**: The main idea for Fixed Alpha Blending is to track where the masks are already covered in one stroke. If the current covering of mask contains the previously tracked pixels, update the color until the color reaches the max color of a stroke under the alpha value (an upper bound). When choosing Constant Brush, such implementation equals not updating previously tracked pixels. When choosing Linear or Quadratic Brush, this implementation updates the color to the max color under current alpha value (i.e., the value when using Constant Brush). Thus, there will be no repeated overlapping of colors in one stroke.

#### Software Engineering, Efficiency, Stability
- Operation codes (e.g., updating masks, handing mouse events, etc.), object codes (e.g., masks, storing structures), and functional codes (e.g., helper functions like converting from 2D cood to 1D, checking functions, comparing functions, etc.) are all properly separated and packed into functions. Functions are properly called and reused to reduce unecessarily long and concoluted code.
- The code are properly annotated and organized in blocks for better readability.
- Reduce the times of updating masks: When settings are changed, there is no immediate update of the mask untill mouse is pressed down. When mouse is pressed down, the settings are freezed untile the next time the settings are changed.
- For additional stability testing, multiple tests have been conducted on the canvas by trying different operation orders, speeds, and even randomly clikcing to check whether the program would crash, to reduce the probability of stability bugs.


### Collaboration/References
I clarify that there is no collaboration or reference include when I do this project,

### Known Bugs

For fixed alpha blending, if using Linear and Quadratic Brush and dragging the mouse extremely fast, there may be some darker areas (looking like fish scales) in one stroke. This doesn't happen when drawing at a normal or slow speed and don't happen for Constant brush at all. There are some possible reasons for this and I have attempted to solve them:
- Rounding error when tracking/storing and updating the RGBA values, in which multiple float-int transformations happen. I have tried the standard way to add 0.5f when transforming from float to int, and used static_cast<float> to transform from int to float. The fish scales effect looks much less obvious, but it still occurs when drawing extremely fast.
-  Efficiency of the algorithm may cause this problem. I've tried to optimize the efficiency by removing unnecessary value updates and reduce looping operations, but it still occurs when drawing extremely fast.

### Extra Credit

#### Spray

My output:

https://github.com/BrownCSCI1230/projects-raster-Junyu-Liu-Nate/assets/75256586/4a39b734-6b2b-4c39-912c-39cd4c1dba70


#### Speed

My output:

https://github.com/BrownCSCI1230/projects-raster-Junyu-Liu-Nate/assets/75256586/ed1b7173-d4e8-4552-a27a-f5ccf0aa85a4



#### Fill

My output:

https://github.com/BrownCSCI1230/projects-raster-Junyu-Liu-Nate/assets/75256586/34ab5fbc-52dc-4b8f-b975-7246364bdef5



#### Fixed Alpha Blending
My output for Const Brush:

https://github.com/BrownCSCI1230/projects-raster-Junyu-Liu-Nate/assets/75256586/21900790-b35a-4272-8887-21985ffcbba3

My output for Linear Brush:

https://github.com/BrownCSCI1230/projects-raster-Junyu-Liu-Nate/assets/75256586/e165decf-dde1-4587-b5de-df3c0a406e37

My output for Qudratic Brush:

https://github.com/BrownCSCI1230/projects-raster-Junyu-Liu-Nate/assets/75256586/bed8cf96-78fe-40dd-a5cc-9c646b9279c0