use <path_extrude.scad>;
myPoints = [ for(t = [0:72:359]) [cos(t-18),sin(t-18)] ];
myPath = [[-1,0,0],[1,0,0],[2,1,0.5],[2,3,1.5],
    [1,4,2],[-1,4,3],[-2,3,3.5],[-2,1,4.5],[-1,0,8]];
path_extrude(exShape=myPoints, exPath=myPath);