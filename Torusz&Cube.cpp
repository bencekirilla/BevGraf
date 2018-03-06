﻿#include <GL/glut.h>
#include <bevgrafmath2017.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <algorithm>

bool centralProjection = true;
GLfloat torusRad = 3.0f, torusrad = 0.5, rotAlp = 0.0f;
GLfloat camRad = 2.0f, camHeight = 1.0f, camWidth = 0.3f; 
GLfloat winWidth = 1200.0f, winHeight = 800.0f;
GLfloat oX = 500.0f, oY = 400.0f, oW = 50.0f, oH = 50.0f;
GLfloat cX = 500.0f, cY = 400.0f, cW = 100.0f, cH = 100.0f;
GLfloat center = 6.0f;
vec3 centrum= {0.0, 0.0, center}, camEye = {camRad, 0.0, 0.0};
mat4 camMat,transMat,Wo,Wc;

class face{
public:
    vec3 P0, P1, P2, P3, NORMV, SP;
    GLfloat CENTDIST;
    face(vec3 P0, vec3 P1, vec3 P2, vec3 P3){
        this -> P0 = P0;
        this -> P1 = P1;
        this -> P2 = P2;
        this -> P3 = P3;
        this -> NORMV = cross(P1 - P0, P3 - P0);
        this -> SP = (P0 + P1 + P2 + P3) / 4.0;
        this -> CENTDIST = dist(centrum, SP);
    }    
};

void initTransformations(){
	camEye = vec3(camRad * cos(camWidth), camRad * sin(camWidth), camHeight);
        vec3 OEV = -1.0 * (vec3(0.0, 0.0, 0.0) - camEye); 
        vec3 camZ = normalize(OEV);
        vec3 camX = normalize(cross(vec3(0.0, 0.0, 0.1),camZ));
        vec3 camY = cross(camZ,camX);
        camMat = coordinateTransform(camEye ,camX, camY, camZ);
	Wo = windowToViewport3(vec2(-1.0f, -1.0f), vec2(2.0f, 2.0f), vec2(oX, oY), vec2(oW, oH));
	Wc = windowToViewport3(vec2(-1.0f, -1.0f), vec2(2.0f, 2.0f), vec2(cX, cY), vec2(cW, cH));
}

void init(){
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glMatrixMode(GL_PROJECTION);
    glLineWidth(1.0f);
	glOrtho(0.0f, winWidth, 0.0f, winHeight, 0.0f, 1.0f);
    initTransformations();
}
bool compareAsCentral(face face1, face face2){
    return face1.CENTDIST  > face2.CENTDIST;
}

bool compareAsOrthogonal(face face1, face face2){
    return face1.SP.z  < face2.SP.z;
}

GLfloat getColor(face Face){
    vec4 camLight = transpose(inverse(camMat)) * vec3(0.0, 0.0, 4.0);
    vec3 ihLight = vec3(camLight.x, camLight.y, camLight.z);
    return (dot(normalize(Face.NORMV),normalize(ihLight)) + 1) / 2.0;  
}
std::vector<face> getDrawableFaces(){
    std::vector<face> drawableFaces;
    std::vector<face> allFaces;
    /*===================================Összes tórusz és kocka lap megadása=====================================================*/    
    for (double theta = 0; theta <= two_pi() + 0.001; theta += pi() / 6)
        for (double phi = 0; phi <= two_pi() + 0.001; phi += pi() / 6){
            vec3 ihPoint0 = vec3((torusRad + torusrad * cos(phi)) * cos(theta),(torusRad + torusrad * cos(phi)) * sin(theta),torusrad * sin(phi));
            vec4 hPoint0 = ihToH(ihPoint0);
            hPoint0 = camMat * rotateZ(rotAlp) * hPoint0;
            if (hPoint0.w != 0)
                ihPoint0 = hToIh(hPoint0);
                
            vec3 ihPoint1 = vec3((torusRad + torusrad * cos(phi)) * cos(theta + pi() / 6),(torusRad + torusrad * cos(phi)) * sin(theta + pi() / 6),torusrad * sin(phi));
            vec4 hPoint1 = ihToH(ihPoint1);
            hPoint1 = camMat * rotateZ(rotAlp) * hPoint1;
            if (hPoint1.w != 0)
                ihPoint1 = hToIh(hPoint1);
                
            vec3 ihPoint2 = vec3((torusRad + torusrad * cos(phi + pi() / 6)) * cos(theta + pi() / 6),(torusRad + torusrad * cos(phi + pi() / 6)) * sin(theta + pi() / 6),torusrad * sin(phi +pi() / 6));
            vec4 hPoint2 = ihToH(ihPoint2);
            hPoint2 = camMat * rotateZ(rotAlp) * hPoint2;
            if (hPoint2.w != 0)
                ihPoint2 = hToIh(hPoint2);
                
            vec3 ihPoint3 = vec3((torusRad + torusrad * cos(phi + pi() / 6)) * cos(theta),(torusRad + torusrad * cos(phi + pi() / 6)) * sin(theta),torusrad * sin(phi + pi() / 6));
            vec4 hPoint3 = ihToH(ihPoint3);
            hPoint3 = camMat * rotateZ(rotAlp) * hPoint3;
            if (hPoint3.w != 0)
                ihPoint3 = hToIh(hPoint3);
            allFaces.push_back(face(ihPoint0,ihPoint1,ihPoint2,ihPoint3));
            }
    vec3 cubePoints[8] ={
    {-0.5 , -0.5 ,  0.5}, { 0.5 , -0.5 ,  0.5}, 
    {0.5 ,  0.5 ,  0.5},  {-0.5 ,  0.5 ,  0.5},
    {-0.5 , -0.5 , -0.5}, {0.5 , -0.5 , -0.5},
    { 0.5 ,  0.5 , -0.5}, {-0.5 ,  0.5 , -0.5}};
    for (int i = 0; i < 8; i++){
        vec4 hPoint = ihToH(cubePoints[i]);
        hPoint = camMat * hPoint;
        if (hPoint.w != 0)
            cubePoints[i] = hToIh(hPoint);
    }
    allFaces.push_back(face(cubePoints[0],cubePoints[1],cubePoints[2],cubePoints[3]));
    allFaces.push_back(face(cubePoints[0],cubePoints[4],cubePoints[5],cubePoints[1]));
    allFaces.push_back(face(cubePoints[1],cubePoints[5],cubePoints[6],cubePoints[2]));
    allFaces.push_back(face(cubePoints[3],cubePoints[2],cubePoints[6],cubePoints[7]));
    allFaces.push_back(face(cubePoints[3],cubePoints[7],cubePoints[4],cubePoints[0]));
    allFaces.push_back(face(cubePoints[4],cubePoints[7],cubePoints[6],cubePoints[5]));
    /*=======================Lap eldobás & Vektor berendezése a festésre======================================================*/    
    for (int i = 0; i < allFaces.size(); i++){
        if (centralProjection){
            if (dot(allFaces[i].NORMV,centrum - allFaces[i].P0) > 0)
                drawableFaces.push_back(allFaces[i]);
        } else {
            if (allFaces[i].NORMV.z > 0)
                drawableFaces.push_back(allFaces[i]);
        }
    }
    if (centralProjection){
        std::sort(drawableFaces.begin(),drawableFaces.end(),compareAsCentral);
    }else{
        std::sort(drawableFaces.begin(),drawableFaces.end(),compareAsOrthogonal);
    }
    
    return drawableFaces;
}

void draw(){
	glClear(GL_COLOR_BUFFER_BIT);
	std::vector<face> drawableFaces = getDrawableFaces();
        for (int i = 0; i < drawableFaces.size();i++){
            GLfloat color = getColor(drawableFaces[i]);
            vec4 hPoint0 = ihToH(drawableFaces[i].P0);
            vec4 hPoint1 = ihToH(drawableFaces[i].P1);
            vec4 hPoint2 = ihToH(drawableFaces[i].P2);
            vec4 hPoint3 = ihToH(drawableFaces[i].P3);
            if (centralProjection)
                transMat = Wc * perspective(center);
            else
                transMat = Wc * ortho();
            hPoint0 = transMat * hPoint0;
            hPoint1 = transMat * hPoint1;
            hPoint2 = transMat * hPoint2;
            hPoint3 = transMat * hPoint3;
            if (hPoint0.w != 0)
                drawableFaces[i].P0 = hToIh(hPoint0);
            if (hPoint1.w != 0)
                drawableFaces[i].P1 = hToIh(hPoint1);
            if (hPoint2.w != 0)
                drawableFaces[i].P2 = hToIh(hPoint2);
            if (hPoint3.w != 0)
                drawableFaces[i].P3 = hToIh(hPoint3);
            glColor3f(color, color, color);
            glBegin(GL_POLYGON);
            glVertex2f(drawableFaces[i].P0.x,drawableFaces[i].P0.y);
            glVertex2f(drawableFaces[i].P1.x,drawableFaces[i].P1.y);
            glVertex2f(drawableFaces[i].P2.x,drawableFaces[i].P2.y);
            glVertex2f(drawableFaces[i].P3.x,drawableFaces[i].P3.y);
            glEnd();
            glColor3f(0.0, 0.0, 0.0);
            glBegin(GL_LINE_LOOP);
            glVertex2f(drawableFaces[i].P0.x,drawableFaces[i].P0.y);
            glVertex2f(drawableFaces[i].P1.x,drawableFaces[i].P1.y);
            glVertex2f(drawableFaces[i].P2.x,drawableFaces[i].P2.y);
            glVertex2f(drawableFaces[i].P3.x,drawableFaces[i].P3.y);
            glEnd(); 
        }
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y){
	switch (key) {
	case 27:      exit(0);                         break;
	case 'a':     camWidth += 0.1;                 break;
	case 'd':     camWidth -= 0.1;                 break;
	case 'w':     camHeight += 0.2;                break;
	case 's':     camHeight -= 0.2;                break;
	case '+': if (center > 1.2)  center -= 0.1;    break;
	case '-': if (center < 10.0) center += 0.1;    break;
    case 'c':     centralProjection = true;        break;
    case 'x':     centralProjection = false;       break;
    case 'r': if (torusRad < 6.0) torusRad += 0.1; break;
    case 'e': if (torusRad > 2.0) torusRad -= 0.1; break;
    case 't': if (torusrad < 1.2) torusrad += 0.1; break;
    case 'z': if (torusrad > 0.3) torusrad -= 0.1; break;
    case 'f': if (camRad < 10.0) camRad += 0.1;    break;
    case 'g': if (camRad > 0.0) camRad -= 0.1;     break;
    }
	initTransformations();
	glutPostRedisplay();
}
void update(int n){
    if (rotAlp > two_pi())
        rotAlp = 0.0;
    else 
        rotAlp += 0.001;
    initTransformations();
    glutPostRedisplay();
    glutTimerFunc(5,update, 0);
}

int main(int argc, char** argv){
	glutInit(&argc, argv);                         
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);  
	glutInitWindowPosition(0, 0);   
	glutInitWindowSize(winWidth, winHeight);     
	glutCreateWindow("Torus & Cube."); 
	init();             
	glutDisplayFunc(draw); 
    glutTimerFunc(5,update, 0);
	glutKeyboardFunc(keyboard);
	glutMainLoop();                
	return 0;
}