//Rocket and Solar System Simulator

/*
This program demonstrates a 3D scaled model of our solar system based on real data and the Universal Law of Gravitation.
Users could move, zoom in, zoom out, and rotate the map as well as toggle the time, space, and size scale of the solar
system. Furthermore, users could launch rockets from their preferred planets, and the simulator will monitor and visualize
the path and destinies of these rockets. In the end, users could modify the mass of the planets to see the effect of mass
in the Universal Law of Gravitation.
*/


#include <windows.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

//solar system scale factors
//these factors determines how the solar system is scaled so that they fit in the screen
vector <float> SpaceRatios = {9, 9.5, 9.7, 10, 10.25};//(10 ^ ratio) m , space factors determines the distance
int spaceRatioIndex;//the space factor which is used
vector <float> SizeRatios = {7500000, 10000000, 20000000, 40000000,80000000};//radius size factors determines the size of each planet
int sizeRatioIndex;//the size factor which is used
vector <float> TimeRatios = {1, 60, 3600, 86400, 604800, 2629843.2, 31558118.4, 0.00000001};//time factors determines how fast the program runs
int timeRatioIndex;//the time factor which is used

//map control

//x, y, and z coordinate of the whole map
float mapx;
float mapy;
float mapz;

//angle of the map rotation
float angle;

//factor which the map rotate upon
float anglex;
float angley;
float anglez;

bool showAxis;//if show the assisting axis
int showName;//if show name of planet (1 = show name, 2 = show name and mass, 0 = don't show)
int nF3;//number of times f3 is pressed
int nF4;//number of times f4 is pressed
int nTab;//number of times tab is pressed

//this vector of text stores the 5 latest events
//index at 0 is the oldest, 4 is the latest event
vector<string> Events;

//mass control
int selectedPlanet;//which planet is selected

//rocket control
bool inputRocket;//if the user is inputing an rocket

//velocity
struct velocity
{
    float x;
    float y;
    float z;
};

//displacement
struct displacement
{
    float x;
    float y;
    float z;
};

//universal constants (according to Newtonian mechanics)
float time = 0; //stores the time in real life at the current frame
float lastTime = 0; //stores the time in real life at the last frame
float realTime; // the 'real' time in the simulation world
const float G = 6.67408 * pow(10, -11); // G in Universal Gravitation
float massSun; //mass of the sun
float rSun = 695700000; // radius of the sun
const float PI = 3.1415926535897932384626433832795028841971693; // Pi
displacement dSun; // where the sun is located

void printbitmap(string s) //print the given string on the screen in BITMAP 9 by 15 font
{
    for(int i = 0; i < s.length(); i++) //for each character within the string
    {
        //print the character at BITMAP 9 by 15
        glutBitmapCharacter( GLUT_BITMAP_9_BY_15, s.at(i));
    }
}

void printbitmap2(string s) //print the given string on the screen in Times New Roman font size 10
{
    for(int i = 0; i < s.length(); i++) //for each character within the string
    {
        //print the character at Times New Roman font size 10
        glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_10, s.at(i));
    }
}

void universalTime()//updates the time
{
    if(time != 0) // update the lastTime
        lastTime = time;
    time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;//gives you time in seconds
    //in this simulation, one second = one second, one day, one week, one month, or one year in real life
}

void displayTime()//display the current time
{
    //set colour
    glColor3f(1,1,1);

    //draw the arrow to the time
    glBegin(GL_LINE_STRIP);
    glVertex3f(dSun.x, dSun.y, dSun.z);
    glVertex3f(dSun.x, dSun.y, dSun.z+90);
    glVertex3f(dSun.x+25, dSun.y, dSun.z+100.6);
    glEnd();
    glPointSize(4);
    glBegin(GL_POINTS);
    glVertex3f(dSun.x+25, dSun.y, dSun.z+100.6);
    glEnd();

    //print the time
    glColor3f(1,1,1);//set colour to white
    glRasterPos3f(dSun.x+30,dSun.y,dSun.z+100); //sets the position of the string
    printbitmap("Time: "+to_string(realTime/TimeRatios[6]) + " years");//print the text

    //if F3 is pressed twice
    if(angle<=335 and nF3 == 2)//for viewing angles less or equal to 335 degree
    {
        glRasterPos3f(dSun.x+30,dSun.y,dSun.z+90); //sets the position of the string
        printbitmap2("    = "+to_string(realTime/TimeRatios[5]) + " months");//print the text
        glRasterPos3f(dSun.x+30,dSun.y,dSun.z+80); //sets the position of the string
        printbitmap2("    = "+to_string(realTime/TimeRatios[4]) + " weeks");//print the text
        glRasterPos3f(dSun.x+30,dSun.y,dSun.z+70); //sets the position of the string
        printbitmap2("    = "+to_string(realTime/TimeRatios[3]) + " days");//print the text
        glRasterPos3f(dSun.x+30,dSun.y,dSun.z+60); //sets the position of the string
        printbitmap2("    = "+to_string(realTime/TimeRatios[2]) + " hours");//print the text
        glRasterPos3f(dSun.x+30,dSun.y,dSun.z+50); //sets the position of the string
        printbitmap2("    = "+to_string(realTime/TimeRatios[1]) + " minutes");//print the text
        glRasterPos3f(dSun.x+30,dSun.y,dSun.z+40); //sets the position of the string
        printbitmap2("    = "+to_string(realTime/TimeRatios[0]) + " seconds");//print the text
    }
    else if(nF3 == 2)//for angle greater than 335 degree
    {
        glRasterPos3f(dSun.x+30,dSun.y+10,dSun.z+90); //sets the position of the string
        printbitmap2("    = "+to_string(realTime/TimeRatios[5]) + " months");//print the text
        glRasterPos3f(dSun.x+30,dSun.y+20,dSun.z+90); //sets the position of the string
        printbitmap2("    = "+to_string(realTime/TimeRatios[4]) + " weeks");//print the text
        glRasterPos3f(dSun.x+30,dSun.y+30,dSun.z+90); //sets the position of the string
        printbitmap2("    = "+to_string(realTime/TimeRatios[3]) + " days");//print the text
        glRasterPos3f(dSun.x+30,dSun.y+40,dSun.z+90); //sets the position of the string
        printbitmap2("    = "+to_string(realTime/TimeRatios[2]) + " hours");//print the text
        glRasterPos3f(dSun.x+30,dSun.y+50,dSun.z+90); //sets the position of the string
        printbitmap2("    = "+to_string(realTime/TimeRatios[1]) + " minutes");//print the text
        glRasterPos3f(dSun.x+30,dSun.y+60,dSun.z+90); //sets the position of the string
        printbitmap2("    = "+to_string(realTime/TimeRatios[0]) + " seconds");//print the text
    }

}

void updateEvents(string s) //update string s into the event vector and remove the oldest event
{
    //shift the events
    Events[0] = Events[1];
    Events[1] = Events[2];
    Events[2] = Events[3];
    Events[3] = Events[4];
    Events[4] = s;
}

void timeIndexUpdate(int i) //update the new time index on the event log. i is the index
{
    if(i == 0)
        updateEvents(">at "+to_string(time)+" s: Time ratio now set to 1 s = 1 s");
    else if(i == 1)
        updateEvents(">at "+to_string(time)+" s: Time ratio now set to 1 s = 1 min");
    else if(i == 2)
        updateEvents(">at "+to_string(time)+" s: Time ratio now set to 1 s = 1 h");
    else if(i == 3)
        updateEvents(">at "+to_string(time)+" s: Time ratio now set to 1 s = 1 day");
    else if(i == 4)
        updateEvents(">at "+to_string(time)+" s: Time ratio now set to 1 s = 1 week");
    else if(i == 5)
        updateEvents(">at "+to_string(time)+" s: Time ratio now set to 1 s = 1 month");
    else if(i == 6)
        updateEvents(">at "+to_string(time)+" s: Time ratio now set to 1 s = 1 year");
}

void spaceIndexUpdate(int i) //update the new space index on the event log. i is the index
{
    if(i == 0)
        updateEvents(">at "+to_string(time)+" s: Space ratio now set to 1 m = 1/10^9.0 m");
    else if(i == 1)
        updateEvents(">at "+to_string(time)+" s: Space ratio now set to 1 m = 1/10^9.5 m");
    else if(i == 2)
        updateEvents(">at "+to_string(time)+" s: Space ratio now set to 1 m = 1/10^9.7 m");
    else if(i == 3)
        updateEvents(">at "+to_string(time)+" s: Space ratio now set to 1 m = 1/10^10.0 m");
    else if(i == 4)
        updateEvents(">at "+to_string(time)+" s: Space ratio now set to 1 m = 1/10^10.25 m");
}

void sizeIndexUpdate(int i) //update the new size index on the event log. i is the index
{
    if(i == 0)
        updateEvents(">at "+to_string(time)+" s: Planet size ratio now set to 1 m = 1/7500000 m");
    else if(i == 1)
        updateEvents(">at "+to_string(time)+" s: Planet size ratio now set to 1 m = 1/10000000 m");
    else if(i == 2)
        updateEvents(">at "+to_string(time)+" s: Planet size ratio now set to 1 m = 1/20000000 m");
    else if(i == 3)
        updateEvents(">at "+to_string(time)+" s: Planet size ratio now set to 1 m = 1/40000000 m");
    else if(i == 4)
        updateEvents(">at "+to_string(time)+" s: Planet size ratio now set to 1 m = 1/80000000 m");
}

void eventLog() //this is the render of the event log on the screen
{
    //set colour to white
    glColor3f(1,1,1);

    //draw the arrow to the event log
    glBegin(GL_LINE_STRIP);
    glVertex3f(dSun.x, dSun.y, dSun.z);
    glVertex3f(dSun.x, dSun.y, dSun.z-30);
    glVertex3f(dSun.x-25, dSun.y-25, dSun.z-40);
    glEnd();
    glPointSize(4);
    glBegin(GL_POINTS);
    glVertex3f(dSun.x-25, dSun.y-25, dSun.z-40);
    glEnd();

    //draw title
    glRasterPos3f(dSun.x-100, dSun.y-25, dSun.z-40); //sets the position of the string
    printbitmap("Event Log");//print the text

    //draw the 5 events
    if(angle>315) //when viewing angle is greater than 315 degree
    {
        for(int i = 0; i<5; i++)//for each event
        {
            glRasterPos3f(dSun.x-230, dSun.y-40 - 12*i, dSun.z-40); //sets the position of the string
            if(i == 4) //draw the latest event at a bigger font
                printbitmap(Events[i]);
            else
                printbitmap2(Events[i]);//print the text
        }
    }
    else //when viewing angle is less than or equal to 315 degree
    {
        for(int i = 0; i<5; i++)//for each event
        {
            glRasterPos3f(dSun.x-150, dSun.y-25, dSun.z-50 - 13*i); //sets the position of the string
            if(i == 4) //draw the latest event at a bigger font
                printbitmap(Events[i]);
            else
                printbitmap2(Events[i]);//print the text
        }
    }

};

void drawRatios() //this renders the scale ratios on the screen
{
    //draw arrow to the scale ratios
    glBegin(GL_LINE_STRIP);
    glVertex3f(dSun.x, dSun.y, dSun.z);
    glVertex3f(dSun.x, dSun.y, dSun.z+60);
    glVertex3f(dSun.x-25, dSun.y+25, dSun.z+70.6);
    glEnd();
    glPointSize(4);
    glBegin(GL_POINTS);
    glVertex3f(dSun.x-25, dSun.y+25, dSun.z+70.6);
    glEnd();

    //draw the scaling ratios
    glColor3f(1,1,1);//set colour to white
    glRasterPos3f(dSun.x-130,dSun.y+25,dSun.z+70.6); //sets the position of the string
    printbitmap("Scaling Ratios");//print the text

    //s1, time ratio
    //s2, space ratio
    //s3, size ratio
    string s1, s2, s3;

    //set s1 to the corresponding ratio
    int i = timeRatioIndex;
    if(i == 0)
        s1 = "Time: 1 s = 1 s";
    else if(i == 1)
        s1 = "Time: 1 s = 1 min";
    else if(i == 2)
        s1 = "Time: 1 s = 1 h";
    else if(i == 3)
        s1 = "Time: 1 s = 1 day";
    else if(i == 4)
        s1 = "Time: 1 s = 1 week";
    else if(i == 5)
        s1 = "Time: 1 s = 1 month";
    else if(i == 6)
        s1 = "Time: 1 s = 1 year";

    //set s2 to the corresponding ratio
    i = spaceRatioIndex;
    if(i == 0)
        s2 = "Space ratio: 1 m = 1/10^9.0 m";
    else if(i == 1)
        s2 = "Space: 1 m = 1/10^9.5 m";
    else if(i == 2)
        s2 = "Space: 1 m = 1/10^9.7 m";
    else if(i == 3)
        s2 = "Space: 1 m = 1/10^10.0 m";
    else if(i == 4)
        s2 = "Space: 1 m = 1/10^10.25 m";

    //set s3 to the corresponding ratio
    i = sizeRatioIndex;
    if(i == 0)
        s3 = "Size ratio: 1 m = 1/7500000 m";
    else if(i == 1)
        s3 = "Size: 1 m = 1/10000000 m";
    else if(i == 2)
        s3 = "Size: 1 m = 1/20000000 m";
    else if(i == 3)
        s3 = "Size: 1 m = 1/40000000 m";
    else if(i == 4)
        s3 = "Size: 1 m = 1/80000000 m";

    if(angle<=335) //when viewing angle is less or equal to 335 degree
    {
        glRasterPos3f(dSun.x-130,dSun.y+25,dSun.z+60.6); //sets the position of the string
        printbitmap2(s1);//print the text
        glRasterPos3f(dSun.x-130,dSun.y+25,dSun.z+50.6); //sets the position of the string
        printbitmap2(s2);//print the text
        glRasterPos3f(dSun.x-130,dSun.y+25,dSun.z+40.6); //sets the position of the string
        printbitmap2(s3);//print the text
    }
    else //when viewing angle is greater than 335 degree
    {
        glRasterPos3f(dSun.x-130,dSun.y+40,dSun.z+60.6); //sets the position of the string
        printbitmap2(s1);//print the text
        glRasterPos3f(dSun.x-130,dSun.y+50,dSun.z+70.6); //sets the position of the string
        printbitmap2(s2);//print the text
        glRasterPos3f(dSun.x-130,dSun.y+60,dSun.z+80.6); //sets the position of the string
        printbitmap2(s3);//print the text
    }
}

struct planet //structure for planet
{
    //name
    string name;

    //mass
    float mass;

    //color
    float r;
    float g;
    float b;

    //coordinate
    displacement d;

    //velocity;
    velocity v;

    //trajectory
    vector <displacement> trajectory;

    //radius
    float rr;

    //if still exist
    bool exist;
};

struct rocket//structure for rocket
{
    //rocket name
    string name;

    //coordinate
    displacement d;

    //velocity;
    velocity v;

    //trajectory
    vector <displacement> trajectory;

    //creation time (in second)
    float t;
};

vector <planet> planets;//all the planets
vector <rocket> rockets;//all the rockets

void massControl(int i, float factor)//controls the mass of planets and the sun
{
    if((i-1)>=0) //for planets, i-1 is to convert it into the index
    {
        if(planets[i-1].mass<pow(10, 36) and factor > 1){//if the mass is less than the maximum allowed
            planets[i-1].mass = planets[i-1].mass * factor;//change the mass by the factor
            updateEvents(">at "+to_string(time) + ": " + planets[i-1].name + "'s mass has increased to " + to_string(planets[i-1].mass/pow(10,24)) + " x 10^24 kg");
        }
        else if(factor > 1) {//if the mass is greater than the maximum allowed
            updateEvents(">at "+to_string(time) + ": Maximum mass reached");
        }
        else if(planets[i-1].mass>pow(10, 22) and factor < 1){//if the mass is greater than the minimum allowed
            planets[i-1].mass = planets[i-1].mass * factor;//change the mass by the factor
            updateEvents(">at "+to_string(time) + ": " + planets[i-1].name + "'s mass has decreased to " + to_string(planets[i-1].mass/pow(10,24)) + " x 10^24 kg");
        }
        else { //if not
            updateEvents(">at "+to_string(time) + ": Minimum mass reached");
        }

    }
    if ((i-1) == -1) //for sun
    {
        if(massSun<pow(10, 36) and factor > 1){//if the mass is less than the maximum allowed
            massSun = massSun * factor;//change the mass by the factor
            updateEvents(">at "+to_string(time) + ": Sun's mass has increased to " + to_string(massSun/pow(10,24)) + " x 10^24 kg");
        }else if(factor > 1) {//change the mass by the factor
            updateEvents(">at "+to_string(time) + ": Maximum mass reached");
        }else if(massSun>pow(10, 22) and factor < 1){//if the mass is greater than the minimum allowed
            massSun = massSun * factor;//change the mass by the factor
            updateEvents(">at "+to_string(time) + ": Sun's mass has decreased to " + to_string(massSun/pow(10,24)) + " x 10^24 kg");
        }else {//if not
            updateEvents(">at "+to_string(time) + ": Minimum mass reached");
        }
    }
}

void updateRocket() //update the information of the rockets
{
    for(int i = 0; i < rockets.size(); i++)
    {
        //if this rocket will be removed
        bool removee = false;

        //trajectory
        displacement td;//temp position
        td.x = rockets[i].d.x;
        td.y = rockets[i].d.y;
        td.z = rockets[i].d.z;
        rockets[i].trajectory.push_back(td);//push back this position
        if(rockets[i].trajectory.size()>7000)//remove the excess position if there are more than 7000 of them
            rockets[i].trajectory.erase(rockets[i].trajectory.begin()+0);

        //precision factor is to adjust the calculation when dt becomes too big. It slice the dt into smaller dt
        int precisionFactor = 1;//for small dt, set precision factor to one because there is no need to adjust.
        if(timeRatioIndex == 6)//for big dt (1s = 1year), set precision factor to 365 so that dt is sliced into 365 pieces
            precisionFactor = 365;
        else if(timeRatioIndex == 5)//for 1s = 1month, set precision factor to 30 so that dt is sliced into 30 pieces
            precisionFactor = 30;

        //change in time in second
        float dtime = (time - lastTime) * TimeRatios[timeRatioIndex] / precisionFactor;//calculate the dt

        for(int it = 0; it <precisionFactor; it++)//for the number of dt
        {
            if(timeRatioIndex == 6){//add more trajectories for one second = one year
                if(it%30 == 0 and it!=0){
                    displacement td;
                    td.x = rockets[i].d.x;
                    td.y = rockets[i].d.y;
                    td.z = rockets[i].d.z;
                    rockets[i].trajectory.push_back(td);
                    if(rockets[i].trajectory.size()>130000)
                        rockets[i].trajectory.erase(planets[i].trajectory.begin()+0);
                }
            }


            //update the displacement
            rockets[i].d.x = rockets[i].d.x + dtime*rockets[i].v.x;
            rockets[i].d.y = rockets[i].d.y + dtime*rockets[i].v.y;
            rockets[i].d.z = rockets[i].d.z + dtime*rockets[i].v.z;

            //update acceleration using universal law of gravitation
            //acceleration due to sun
            float r = sqrt((rockets[i].d.x)*(rockets[i].d.x)+(rockets[i].d.y)*(rockets[i].d.y)+(rockets[i].d.z)*(rockets[i].d.z));
            float ax = rockets[i].d.x / r * -G * massSun / (r*r);
            float ay = rockets[i].d.y / r * -G * massSun / (r*r);
            float az = rockets[i].d.z / r * -G * massSun / (r*r);

            if(r>1.5*7375.9*pow(10,9))//if rocket ran into deep space
            {
                updateEvents(">at "+to_string(time)+" s: Rocket "+rockets[i].name+" has flown into the deep space and will never return.");//update event log
                removee = true;//remove it
                goto label;//skip the rest
            }

            if(r<=rSun*1.5)//if rocket ran into sun
            {
                updateEvents(">at "+to_string(time)+" s: Rocket "+rockets[i].name+" has crashed into the sun.");//update event log
                removee = true;//remove it
                goto label;//skip the rest
            }

            //acceleration due to planets
            for (int i2=0; i2<planets.size(); i2++)//for each planet
            {
                //find their distance
                r = sqrt(((rockets[i].d.x)-planets[i2].d.x)*((rockets[i].d.x)-planets[i2].d.x)+((rockets[i].d.y)-planets[i2].d.y)*((rockets[i].d.y)-planets[i2].d.y)+((rockets[i].d.z)-planets[i2].d.z)*((rockets[i].d.z)-planets[i2].d.z));
                //update using universal law of gravitation
                ax += (rockets[i].d.x-planets[i2].d.x) / r * -G * planets[i2].mass / (r*r);
                ay += (rockets[i].d.y-planets[i2].d.y) / r * -G * planets[i2].mass / (r*r);
                az += (rockets[i].d.z-planets[i2].d.z) / r * -G * planets[i2].mass / (r*r);

                //if the rocket run into a planet
                if(r<planets[i2].rr /2)
                {
                    //calculate the speed of the rocket relative to that planet
                    float v = sqrt((rockets[i].v.x-planets[i2].v.x)*(rockets[i].v.x-planets[i2].v.x) + (rockets[i].v.y-planets[i2].v.y)*(rockets[i].v.y-planets[i2].v.y) + (rockets[i].v.z-planets[i2].v.z)*(rockets[i].v.z-planets[i2].v.z))/1000;
                    //tell the user that the rocket landed or crashed
                    updateEvents(">at "+to_string(time)+" s: Neglecting air friction, Rocket "+rockets[i].name+" has landed (or crashed) on "+planets[i2].name+" with a speed of "+to_string(v)+" km/s relative to the planet");
                    removee = true;//remove the rocket
                    goto label;//skip the rest
                }
            }


            //update the velocity
            rockets[i].v.x = rockets[i].v.x + ax*dtime;
            rockets[i].v.y = rockets[i].v.y + ay*dtime;
            rockets[i].v.z = rockets[i].v.z + az*dtime;
        }

label:
        ;
        if(removee == true)//remove the rocket if needed
        {
            rockets.erase(rockets.begin()+i);
            if(i != rockets.size())//adjust the index if needed
                i--;
        }
    }
}

void createRocket() //create a new rocket
{
    timeRatioIndex = 7;//set the time ratio to a very small number (equivalent to pause)
    rocket r;//temp rocket.
    displacement d;//direction vector
    float vi;//initial velocity.

    //get name
    cout<<"Input the name of the rocket: ";
    getline(cin, r.name);
    cout<<endl;

    //get launching planet
    cout<<"Input the planet where the rocket will be launched"<<endl;
    cout<<"1 for Mercury, 2 for Venus, 3 for Earth, 4 for Mars, 5 for Jupiter"<<endl;
    cout<<"6 for Saturn, 7 for Uranus, 8 for Neptune, 9 for Pluto."<<endl;
    int i;//index for the planet.
    while(true)
    {
        cout<<"Your Choice: ";
        cin>>i;
        if(!cin.fail()  and i>=1 and i <= 9)
            break;
    }
    i = i-1;//change to the real index
    cout<<endl;

    //get the direction vector
    cout<<planets[i].name<<" is at position ("<<planets[i].d.x/ pow(10, SpaceRatios[spaceRatioIndex])<<" ,"<<planets[i].d.y/ pow(10, SpaceRatios[spaceRatioIndex])<<" ,"<<planets[i].d.z/ pow(10, SpaceRatios[spaceRatioIndex])<<").";
    cout<<"What direction do you want your rocket going?"<<endl;
    cout<<"Input the direction vector x component:";
    cin>>d.x;
    cout<<"Input the direction vector y component:";
    cin>>d.y;
    cout<<"Input the direction vector z component:";
    cin>>d.z;
    cout<<endl;

    //change d to direction vector with a magnitude of 1(in case it is not)
    d.x = d.x / sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
    d.y = d.y / sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
    d.z = d.z / sqrt(d.x*d.x + d.y*d.y + d.z*d.z);

    //get the launching location.
    r.d.x = planets[i].d.x+planets[i].rr/2 * d.x;
    r.d.y = planets[i].d.y+planets[i].rr/2 * d.y;
    r.d.z = planets[i].d.z+planets[i].rr/2 * d.z;

    //get the launching velocity.
    cout<<"Input your rocket launching speed relative to the planet you have selected(in km/s): ";
    cin>>vi;
    vi = vi * 1000;//change to m/s
    r.v.x = planets[i].v.x + vi * d.x;
    r.v.y = planets[i].v.y + vi * d.y;
    r.v.z = planets[i].v.z + vi * d.z;

    //creation time
    r.t = realTime;

    cout<<"AlL DONE!"<<endl;
    cout<<endl;

    rockets.push_back(r);

    //update on event
    updateEvents(">at "+to_string(time)+" s: Rocket "+ r.name +" is created on "+planets[i].name);

    inputRocket = false;//not inputing a rocket anymore
    timeRatioIndex = 0;//set to 1s = 1s
}

void drawRocketName(rocket r)//drawing the rocket's name
{
    glColor3f(1,1,1);//set colour to white
    glRasterPos3f(r.d.x/ pow(10, SpaceRatios[spaceRatioIndex]),r.d.y/ pow(10, SpaceRatios[spaceRatioIndex]),r.d.z/ pow(10, SpaceRatios[spaceRatioIndex])); //sets the position of the string
    printbitmap(r.name);//print the text

    if(showName == 2)//if showing the flight time and speed
    {

        glRasterPos3f(r.d.x/ pow(10, SpaceRatios[spaceRatioIndex]) - 5,r.d.y/ pow(10, SpaceRatios[spaceRatioIndex]) - 5,r.d.z/ pow(10, SpaceRatios[spaceRatioIndex])-5); //sets the position of the string
        printbitmap2("Flight time: " + to_string((realTime-r.t)/TimeRatios[6]) + " years");//print the text

        float v = sqrt(r.v.x*r.v.x + r.v.y*r.v.y + r.v.z*r.v.z)/1000;

        glRasterPos3f(r.d.x/ pow(10, SpaceRatios[spaceRatioIndex])-5,r.d.y/ pow(10, SpaceRatios[spaceRatioIndex])-10,r.d.z/ pow(10, SpaceRatios[spaceRatioIndex])-10); //sets the position of the string
        printbitmap2("Speed: " + to_string(v) + " km/s");//print the text

    }
}

void drawRocket() //render the rockets
{
    for(rocket r : rockets)//for each rocket
    {
        //rotation of the cone
        float theta, x, y, z;
        //calculate theta (in radians) using dot product
        theta = acos(r.v.z/sqrt(r.v.x*r.v.x + r.v.y*r.v.y + r.v.z*r.v.z));
        theta = theta * 180/PI;
        //calculate the rotation axis using cross product
        x = -r.v.y;
        y = r.v.x;
        z = 0;

        //draw trajectory
        glColor3d(0.5,0.5,0.5); //set color
        glBegin(GL_LINE_STRIP);
        for(displacement d : r.trajectory)//for each trajectory coordinate
        {
            glVertex3f(d.x / pow(10, SpaceRatios[spaceRatioIndex]), d.y/pow(10, SpaceRatios[spaceRatioIndex]), d.z/pow(10, SpaceRatios[spaceRatioIndex]));//draw it
        }
        glEnd();
        //draw rocket
        glColor3d(1,1,1); //set color
        glPushMatrix();//start the drawing
        glTranslatef(r.d.x/ pow(10, SpaceRatios[spaceRatioIndex]),r.d.y/ pow(10, SpaceRatios[spaceRatioIndex]),r.d.z/ pow(10, SpaceRatios[spaceRatioIndex]));//coordinate of the object
        glRotated(theta,x,y,z); //rotation
        glutSolidCone(1, 4, 30, 30); //draw
        glPopMatrix();//end the drawing
        //draw rocket name
        if(showName >0)//if show name
            drawRocketName(r);
    }
}

void updateAcceleration(float &ax, float &ay, float &az, int i) //update the acceleration of a planet due to other planets
{
    for (int i2=0; i2<planets.size(); i2++)//for each planet
    {
        if(i!=i2)//for other planets
        {
            //get r
            float r = sqrt(((planets[i].d.x)-planets[i2].d.x)*((planets[i].d.x)-planets[i2].d.x)+((planets[i].d.y)-planets[i2].d.y)*((planets[i].d.y)-planets[i2].d.y)+((planets[i].d.z)-planets[i2].d.z)*((planets[i].d.z)-planets[i2].d.z));

            //if the two planets hit each other
            if(r<(planets[i2].rr + planets[i].rr)/2)
            {
                updateEvents(">at "+to_string(time)+" s: "+planets[i].name+" has crashed into "+planets[i2].name);//update the event
                planets[i].exist = false;//this planet no longer exists
            }

            //update the acceleration using universal gravitation
            ax += (planets[i].d.x-planets[i2].d.x) / r * -G * planets[i2].mass / (r*r);
            ay += (planets[i].d.y-planets[i2].d.y) / r * -G * planets[i2].mass / (r*r);
            az += (planets[i].d.z-planets[i2].d.z) / r * -G * planets[i2].mass / (r*r);
        }
    }
}

void updatePlanet()//update the information of the planets
{
    for(int i = 0; i < planets.size(); i++)
    {
        //trajectory
        displacement td;//temp position
        td.x = planets[i].d.x;
        td.y = planets[i].d.y;
        td.z = planets[i].d.z;
        planets[i].trajectory.push_back(td);//push back this position
        if(planets[i].trajectory.size()>60000)//remove the excess position if there are more than 60,000 of them
            planets[i].trajectory.erase(planets[i].trajectory.begin()+0);

        if(planets[i].exist)//as long as the planet exist
        {
            //precision factor is to adjust the calculation when dt becomes too big. It slice the dt into smaller dt
            int precisionFactor = 1;//for small dt, set precision factor to one because there is no need to adjust.
            if(timeRatioIndex == 6)//for big dt (1s = 1year), set precision factor to 365 so that dt is sliced into 365 pieces
                precisionFactor = 365;
            else if(timeRatioIndex == 5)//for 1s = 1month, set precision factor to 30 so that dt is sliced into 30 pieces
                precisionFactor = 30;

            //change in time in second
            float dtime = (time - lastTime) * TimeRatios[timeRatioIndex] / precisionFactor;//calculate the dt
            for(int it = 0; it <precisionFactor; it++)//for the number of dt
            {

                if(timeRatioIndex == 6){//add more trajectories for one second = one year
                    if(it%30 == 0 and it!=0){
                        displacement td;
                        td.x = planets[i].d.x;
                        td.y = planets[i].d.y;
                        td.z = planets[i].d.z;
                        planets[i].trajectory.push_back(td);
                        if(planets[i].trajectory.size()>60000)//remove old trajectories
                            planets[i].trajectory.erase(planets[i].trajectory.begin()+0);
                    }
                }

                //update the displacement
                planets[i].d.x = planets[i].d.x + dtime*planets[i].v.x;
                planets[i].d.y = planets[i].d.y + dtime*planets[i].v.y;
                planets[i].d.z = planets[i].d.z + dtime*planets[i].v.z;

                //update acceleration using universal law of gravitation
                float r = sqrt((planets[i].d.x)*(planets[i].d.x)+(planets[i].d.y)*(planets[i].d.y)+(planets[i].d.z)*(planets[i].d.z));

                if(r>1.5*7375.9*pow(10,9))//if planet fly into deep space
                {
                    updateEvents(">at "+to_string(time)+" s: "+planets[i].name+" has flown into the deep space and will never return.");//update event
                    planets[i].exist = false;//planet no longer exists
                    break;
                }

                if(r<=rSun*1.5)
                {
                    updateEvents(">at "+to_string(time)+" s: "+planets[i].name+" has crashed into the sun.");//update event
                    planets[i].exist = false;//planet no longer exists
                    break;
                }

                //update acceleration due to sun using universal gravitation
                float ax = planets[i].d.x / r * -G * massSun / (r*r);
                float ay = planets[i].d.y / r * -G * massSun / (r*r);
                float az = planets[i].d.z / r * -G * massSun / (r*r);

                //update acceleration due to other planets using universal gravitation
                updateAcceleration(ax, ay, az, i);

                //update the velocity
                planets[i].v.x = planets[i].v.x + ax*dtime;
                planets[i].v.y = planets[i].v.y + ay*dtime;
                planets[i].v.z = planets[i].v.z + az*dtime;
            }
        }
    }
}

void drawPlanetName(planet p) //draw the name of the planet at index i
{
    glColor3f(1,1,1);//set colour to white
    //set location
    glRasterPos3f(p.d.x/ pow(10, SpaceRatios[spaceRatioIndex]),p.d.y/ pow(10, SpaceRatios[spaceRatioIndex])-p.rr/SizeRatios[sizeRatioIndex]*1.5,p.d.z/ pow(10, SpaceRatios[spaceRatioIndex])); //sets the position of the string
    printbitmap(p.name);//print the text

    if(showName==2)//if show mass
    {
        //set location
        glRasterPos3f(p.d.x/ pow(10, SpaceRatios[spaceRatioIndex]) - 10,p.d.y/ pow(10, SpaceRatios[spaceRatioIndex])-p.rr/SizeRatios[sizeRatioIndex]*1.5 - 5,p.d.z/ pow(10, SpaceRatios[spaceRatioIndex])-5); //sets the position of the string
        printbitmap2(to_string((p.mass/(pow(10,24)))) + " x 10^24 kg");//print the text
    }
}

void drawPlanet() //draw each planet
{
    for(planet p : planets)//for each planet
    {
        //if exist
        if(p.exist == true)
        {
            //draw trajectory
            glColor3d(0.5,0.5,0.5); //set color
            glBegin(GL_LINE_STRIP);
            for(displacement d : p.trajectory)
            {
                glVertex3f(d.x / pow(10, SpaceRatios[spaceRatioIndex]), d.y/pow(10, SpaceRatios[spaceRatioIndex]), d.z/pow(10, SpaceRatios[spaceRatioIndex]));
            }
            glEnd();
            //draw planet
            glColor3d(p.r,p.g,p.b); //set color
            glPushMatrix();//start the drawing
            glTranslatef(p.d.x/ pow(10, SpaceRatios[spaceRatioIndex]),p.d.y/ pow(10, SpaceRatios[spaceRatioIndex]),p.d.z/ pow(10, SpaceRatios[spaceRatioIndex]));//coordinate of the object
            glRotated(0,0,1,0); //rotation
            glutSolidSphere(p.rr/SizeRatios[sizeRatioIndex],100,100); //draw
            glPopMatrix();//end the drawing
            //draw planet name
            if(showName > 0)
                drawPlanetName(p);

        }
    }
}

void drawAxis() //draw the x, y, z axis
{
    //x - axis
    glColor3d(1,0,0);
    glBegin(GL_LINE_STRIP);
    glVertex3f(0,0,0);
    glVertex3f(50,0,0);
    glEnd();

    //arrow
    glPushMatrix();//start the drawing
    glTranslatef(50,0,0);//coordinate of the object
    glRotated(90,0,1,0); //rotation
    glutSolidCone(1, 3, 30, 30); //draw
    glPopMatrix();//end the drawing

    //label
    glColor3f(1,0,0);//set colour to white
    glRasterPos3f(52,0,0); //sets the position of the string
    printbitmap2("+ x");//print the text

    //y - axis
    glColor3d(0,1,0);
    glBegin(GL_LINE_STRIP);
    glVertex3f(0,0,0);
    glVertex3f(0,50,0);
    glEnd();

    //arrow
    glPushMatrix();//start the drawing
    glTranslatef(0,50,0);//coordinate of the object
    glRotated(270,1,0,0); //rotation
    glutSolidCone(1, 3, 30, 30); //draw
    glPopMatrix();//end the drawing

    //label
    glColor3f(0,1,0);//set colour to white
    glRasterPos3f(2,50,0); //sets the position of the string
    printbitmap2("+ y");//print the text

    //z - axis
    glColor3d(0,0,1);
    glBegin(GL_LINE_STRIP);
    glVertex3f(0,0,0);
    glVertex3f(0,0,50);
    glEnd();

    //arrow
    glPushMatrix();//start the drawing
    glTranslatef(0,0,50);//coordinate of the object
    glRotated(0,0,0,0); //rotation
    glutSolidCone(1, 3, 30, 30); //draw
    glPopMatrix();//end the drawing

    //label
    glColor3f(0,0,1);//set colour to white
    glRasterPos3f(2,0,50); //sets the position of the string
    printbitmap2("+ z");//print the text
}

void render() //this method draws everything
{
    glClearColor(0,0,0,0); // black background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    //set the origin of the screen
    glTranslatef(mapx, mapy, mapz);
    glRotatef(angle, anglex, angley, anglez);

    //draw axis
    if(showAxis)
        drawAxis();

    //update time
    universalTime();
    //display time
    realTime = realTime + (time-lastTime)*TimeRatios[timeRatioIndex];//real time in seconds
    if(nF3>0)
        displayTime();

    //update event log
    if(nF4>0)
        eventLog();

    //draw scale ratios
    if(nF4==2)
        drawRatios();

    //Sun
    glPushMatrix();//start the drawing
    glColor3d(1,0.75,0); //set color to yellow
    glTranslatef(dSun.x,dSun.y,dSun.z);//coordinate of the object
    glRotated(0,0,1,0); //rotation
    glutSolidSphere(3,100,100); //draw the sun
    glPopMatrix();//end the drawing
    if(showName>0)//if show sun's name
    {
        glRasterPos3f(dSun.x-5,dSun.y-10,dSun.z);
        printbitmap("Sun");
        if(showName==2)//if show sun's mass
        {
            glRasterPos3f(dSun.x-10,dSun.y-15,dSun.z-5);
            printbitmap2(to_string((massSun/(pow(10,24)))) + " x 10^24 kg");
        }
    }

    //draw each planet
    drawPlanet();

    //update each planet
    updatePlanet();

    //create rocket
    if(inputRocket)
        createRocket();

    //draw each rocket
    drawRocket();

    //update each rocket
    updateRocket();

    glutSwapBuffers();  //swap the objects with the new one
}

void perihelionVelocity(float rp, float ra, float &vp)//calculate the perihelion velocity given the perihelion radius
{
    //calculates the perihelion velocity
    vp = sqrt(2 * G * massSun * (1/rp - 1/(rp+ra)));
}

void orbitalShift(float rp, float theta, float &x2, float &z2)//calculate the shifted orbitals
{
    //shifting rp by theta degree
    x2 = rp/sqrt(1+tan(theta)*tan(theta));
    z2 = x2 * tan(theta);
}

void initialization()
{
//time
    realTime=0;

//solar system scale factors
    spaceRatioIndex = 1;
    sizeRatioIndex = 1;
    timeRatioIndex = 4;

//map control
    mapx=0;
    mapy=0;
    mapz=-400;
    angle=300;
    anglex=1;
    angley=0;
    anglez=0;
    showAxis=false;
    showName=1;
    nF3=1;
    nF4=2;
    nTab=0;

//rocket control
    inputRocket = false;

//empty the vectors
    planets.clear();
    rockets.clear();

//sun and mass control
    massSun = 1988500 * pow(10,24);
    selectedPlanet = -1;

//initialize events
    //create 5 empty events
    for(int i=0; i<5; i++)
    {
        Events.push_back(">------------------- ");
    }
    updateEvents(">at "+to_string(time)+" s: Simulation Begins");
    timeIndexUpdate(timeRatioIndex);
    spaceIndexUpdate(spaceRatioIndex);
    sizeIndexUpdate(sizeRatioIndex);

    //initialize the sun's displacement
    dSun.x = 0;
    dSun.y = 0;
    dSun.z = 0;

    ifstream InFile("planets.txt"); // read a file contains planet information
    if (InFile.fail())
    {
        cout << "File: not found"<<endl;
    }
    else
    {
        string line;//temp line to store each line of the text
        //get the number of planets
        int nPlanets;
        getline(InFile,line);
        nPlanets = stoi(line);

        planet temp;//a temporary planet
        for(int i =0; i < nPlanets; i++)//for each planet
        {
            //get the planet name
            getline(InFile,line);
            temp.name = line;

            //get the planet mass
            getline(InFile,line);
            temp.mass = stof(line) * pow(10,24);

            //get the planet's r, g, b values
            getline(InFile,line);
            temp.r= stof(line);
            getline(InFile,line);
            temp.b=stof(line);
            getline(InFile,line);
            temp.g=stof(line);

            //get the planet's perihelion radius
            getline(InFile,line);
            float rp = stof(line);

            //get the planet's aphelion radius
            getline(InFile,line);
            float ra = stof(line);

            //get the inclination angle
            getline(InFile,line);
            float theta = stof(line);

            //temp variables for initialization
            float x2;//initial x coordinate
            float z2;//intial y coordinate
            float vp;//perihelion velocity

            perihelionVelocity(rp  * pow(10,9),ra  * pow(10,9), vp);//get the perihelion velocity
            orbitalShift(rp  * pow(10, 9), theta * PI/180, x2, z2);//get the initial x and z coordinate

            //initial coordinates
            temp.d.x = x2;
            temp.d.y = 0;
            temp.d.z = z2;

            //initial velocity
            temp.v.x = 0;
            temp.v.y = vp;
            temp.v.z = 0;

            getline(InFile,line);//get the radius (in this case, the diameter because the file only contains the diameter)
            temp.rr = stof(line);
            temp.exist = true;//set existence to true
            planets.push_back(temp);//push back the planet
        }
    }
}

void reshape(int w,int h) //set up the view
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f,(float)w/(float)h,1,10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void key(unsigned char key, int x, int y)//for normal keys
{
    if(key == ' ')
        inputRocket = true; //user input rocket information

    if(!inputRocket)//while users are not inputing a rocket
    {
        switch (key) // for each key
        {
        case 9: //if pressed tab
            if(nTab == 0) // 1 time
            {//show all the ui
                showAxis = true;
                showName = 1;
                nF3 = 1;
                nF4 = 2;
                nTab++;
            }
            else
            {//disable all ui
                showAxis = false;
                showName = 0;
                nF3 = 0;
                nF4 = 0;
                nTab = 0;
            }
            break;

        case 27://if pressed esc, set perspective to a bird eye view
            mapz = -400;
            mapx = 0;
            mapy = 0;
            angle = 360;
            break;

        case '+'://if pressed +, zoom in
            mapz += 1.0;
            break;

        case '-'://if pressed -, zoom out
            mapz -= 1.0;
            break;

        case 'w'://if pressed w, increase the viewing angle
            if(angle<360)
                angle+=0.5;
            break;

        case 's'://if pressed s, decrease the viewing angle
            if(angle>270)
                angle-=0.5;
            break;

        case 'q'://if pressed q, decrease the time ratio
            if(timeRatioIndex>0)
            {
                timeRatioIndex--;
                timeIndexUpdate(timeRatioIndex);//update the event log
            }
            break;

        case 'e'://if pressed e, increase the time ratio
            if(timeRatioIndex<6)
            {
                timeRatioIndex++;
                timeIndexUpdate(timeRatioIndex);//update the event log
            }
            break;

        case 'a'://if pressed a, decrease the space ratio
            if(spaceRatioIndex>0)
            {
                spaceRatioIndex--;
                spaceIndexUpdate(spaceRatioIndex);//update the event log
            }
            break;

        case 'd'://if pressed d, increase the time ratio
            if(spaceRatioIndex<4)
            {
                spaceRatioIndex++;
                spaceIndexUpdate(spaceRatioIndex);//update the event log
            }
            break;

        case 'c'://if pressed c, increase the time ratio
            if(sizeRatioIndex>0)
            {
                sizeRatioIndex--;
                sizeIndexUpdate(sizeRatioIndex);//update the event log
            }
            break;

        case 'z'://if pressed z, decrease the time ratio
            if(sizeRatioIndex<4)
            {
                sizeRatioIndex++;
                sizeIndexUpdate(sizeRatioIndex);//update the event log
            }
            break;

        case '0'://if pressed 0, select sun
            selectedPlanet = 0;
            updateEvents(">at "+to_string(time)+" s: Selected Sun");//update the event log
            break;

        case '1'://if pressed 1, select mercury
            selectedPlanet = 1;
            updateEvents(">at "+to_string(time)+" s: Selected Mercury");//update the event log
            break;

        case '2'://if pressed 2, select venus
            selectedPlanet = 2;
            updateEvents(">at "+to_string(time)+" s: Selected Venus");//update the event log
            break;

        case '3'://if pressed 3, select earth
            selectedPlanet = 3;
            updateEvents(">at "+to_string(time)+" s: Selected Earth");//update the event log
            break;

        case '4'://if pressed 4, select mars
            selectedPlanet = 4;
            updateEvents(">at "+to_string(time)+" s: Selected Mars");//update the event log
            break;

        case '5'://if pressed 5, select Jupiter
            selectedPlanet = 5;
            updateEvents(">at "+to_string(time)+" s: Selected Jupiter");//update the event log
            break;

        case '6'://if pressed 6, select Saturn
            selectedPlanet = 6;
            updateEvents(">at "+to_string(time)+" s: Selected Saturn");//update the event log
            break;

        case '7'://if pressed 7, select Uranus
            selectedPlanet = 7;
            updateEvents(">at "+to_string(time)+" s: Selected Uranus");//update the event log
            break;

        case '8'://if pressed 8, select Neptune
            selectedPlanet = 8;
            updateEvents(">at "+to_string(time)+" s: Selected Neptune");//update the event log
            break;

        case '9'://if pressed 9, select Pluto
            selectedPlanet = 9;
            updateEvents(">at "+to_string(time)+" s: Selected Pluto");//update the event log
            break;
        }
    }

    glutPostRedisplay();
}

void keySpecial(int key, int x, int y) //for special keys
{
    switch (key)
    {
    case GLUT_KEY_RIGHT://if right arrow key pressed, move camera right
        mapx -= 1;
        break;

    case GLUT_KEY_LEFT://if left arrow key pressed, move camera left
        mapx += 1;
        break;

    case GLUT_KEY_UP://if right arrow key pressed, move camera up
        mapy -= 1;
        break;

    case GLUT_KEY_DOWN://if down arrow key pressed, move camera down
        mapy += 1;
        break;

    case GLUT_KEY_F1://if pressed f1, show or not show axis
        if(showAxis)
            showAxis = false;
        else
            showAxis = true;
        break;

    case GLUT_KEY_F2://if pressed f2, show or not show names and additional information
        if(showName<2)
            showName++;
        else
            showName = 0;
        break;

    case GLUT_KEY_F3://if pressed f3, show or not show time
        if(nF3<2)
            nF3++;
        else
            nF3 = 0;
        break;

    case GLUT_KEY_F4://if pressed f4, show or not show event log and scale ratios
        if(nF4<2)
            nF4++;
        else
            nF4 = 0;
        break;

    case GLUT_KEY_F5://if pressed f5, restart simulation
        updateEvents(">at "+to_string(time)+" s: Simulation restarted");
        initialization();
        break;

    case GLUT_KEY_F9://if pressed f9, decrease the selected planet mass
        massControl(selectedPlanet, 0.8);
        break;

    case GLUT_KEY_F10://if pressed f10, unselect the current selected planet
        if (selectedPlanet>0)
            updateEvents(">at "+to_string(time)+": Unselected " + planets[selectedPlanet-1].name);
        else if(selectedPlanet==0)
            updateEvents(">at "+to_string(time)+": Unselected Sun");
        selectedPlanet = -1;
        break;

    case GLUT_KEY_F11://if pressed f12, increase the selected planet mass
        massControl(selectedPlanet, 1.2);
        break;
    }

    glutPostRedisplay();
}

//shading and colour effect constants
const GLfloat light_ambient[]  = { 0.25f, 0.25f, 0.25f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 0.0f, 0.0f, 25.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };
int main(int argc, char* argv[])
{
    //initialize
    initialization();
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

    //window settings
    glutInitWindowPosition(0,0);
    glutInitWindowSize(1580,800); //window size
    glutCreateWindow("Rocket simulator"); //title
    glutDisplayFunc(render); //draw the object in the window
    glutIdleFunc(render);
    glutKeyboardFunc(key);//normal key
    glutSpecialFunc(keySpecial);//special key
    glutReshapeFunc(reshape);


//lighting and shading
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glutMainLoop();
    return 0;
}
