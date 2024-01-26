This project utilizes two different algorithms for image processing.
    Nearest Neighbour Algorithm:
    I have chosen to use this algorithm for its efficiency and grid-like structure. 
    Its grid-like structure simplifies the mapping of pixel positions from the original image to the resized one.
    
    Bilinear Interpolation Algorithm:
    This algorithm was chosen because of my previous experience in using lerp functions.


To set up this project, I used "g++ main.cpp tgaHandler.cpp -o ImageScaler.exe".
When ImageScaler.exe is successfully created, then you can input
    ./ImageScaler.exe ImageTestRGb(A).tga test.tga b for Bilinear Interpolation.
    ./ImageScaler.exe ImageTestRGb(A).tga test.tga n for Nearest Neighbour.
If you don't give either n or b but something random as the last input, then you will get out both.

Remember that the tga file that is created will have either "Bilinear_" or "NearestNeighbour_" 
at the front of the file to specify which algorithm was used.