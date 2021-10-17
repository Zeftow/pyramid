#pragma once

/*
* Ici les codes des animations
*/
#include "03_gestion_imu.cpp"


/*TEST DE LA NOUVELLE ARCHITECTURE
* 
*/

void caBouge(){
    readImu();
    mapAc();
    readHmi();
    for(int k=0; k<NUM_LEDS; k++){
        //leds[k].setRGB(mAcX, mAcY, mAcZ);
        leds[k].setRGB(mAcX*sw1, mAcY*sw2, mAcZ*sw3);
    }
    FastLED.show();
}

/*COLORE LA LED SUIVANTE SUR LE RUBAN AVEC LA COULEUR DE Acc
* 
*/

void colorRuban(){
    readImu();
    mapAc();
    readHmi();
    leds[gStep%144].setRGB(mAcX*sw1, mAcY*sw2, mAcZ*sw3);
    //leds[gStep%144].setRGB(mAcX, mAcY, mAcZ);
    gStep++;
    FastLED.delay(pot2);
}



/*ARC EN CIEL YOLO SWAG LISSE
* 
*/
float w = 0;
uint8_t phi = 0;

void rainbow(int steps){
for(int i=0; i<steps; i++){
    readHmi();
    if(nextFlag){
        return;
    }
    w = i/steps;
    for(int k=0; k<NUM_LEDS; k++){
        phi = w+k/NUM_LEDS;
        R = sin8(phi);
        G=0;
        B=0;
        //phi += 85;
        //G = sin8(phi);
        // phi += 85;
        //B = sin8(phi);
        leds[k].setRGB(R, G, B);
    }
    FastLED.show();
}
}


/*ARC EN CIEL YOLO SWAG GRANULEUX
* 
*/

//Constantes aléatoires
int alpha;
int beta;
int gamma;
int kappa;

void rainbowGranuleux(int steps, int wait){
for(int i=0; i<steps; i++){
    readHmi();
        if(nextFlag){
            return;
        }
    w = 2*PI*i/steps;
    kappa = random(1);
    alpha = random(8);
    beta = random(8);
    gamma = random(8);
    for(int k=0; k<NUM_LEDS; k++){
        kappa = random(1);
        alpha = random(8);
        beta = random(8);
        gamma = random(8); 
        phi = 2*PI*k/NUM_LEDS;
        R = (1+sin(w+phi))/2*(255+beta+10);
        G = (1+sin(w+phi+2*PI/3))/2*(255+alpha+5*kappa);
        B = (1+sin(w+phi+4*PI/3))/2*(255+gamma);
        leds[k].setRGB(R, G, B);
        // delay(wait);
    }
    FastLED.show();
    
}
}

/*GLOW COLORED
* 
*/
void glow(int red, int green, int blue, int steps){
for(int i=0; i<steps; i++){
    w = 2*PI*i/steps;
    for(int k=0; k<NUM_LEDS; k++){
    R = (1-cos(w))/2*(red);
    G = (1-cos(w))/2*(green);
    B = (1-cos(w))/2*(blue);
    leds[k].setRGB(R, G, B);
    }
    FastLED.show();

}
}

/*GLOW Piloted
* 
*/
int glowSteps;
void glowRemote(){
    readImu();
    mapAcFast();
    glowSteps = mAcX;
    for(int i=0; i<STEPS; i++){
        readHmi();
        if(nextFlag){
            return;
        }
        readImu();
        mapAcFast();
        
        w = 2*PI*i/glowSteps;
        for(int k=0; k<NUM_LEDS; k++){
            R = (1-cos(w))/2*(pot1);
            G = (1-cos(w))/2*(pot2);
            B = (1-cos(w))/2*(pot3);
            leds[k].setRGB(R, G, B);
        }
        FastLED.show();

    }
}

/*PING PONG SUR LES ARRETES
* 
*/
void pingPong(){
    FastLED.clear();
    // PING
    for(int pos=1; pos<12; pos++){ //boucle for sur les leds de chaque arrête
        readHmi();
        if(nextFlag){
            return;
        }
        for(int sommet=0; sommet<12; sommet++){
        leds[12*sommet+pos].setRGB(pot1, pot2, pot3);
        leds[12*sommet+pos-1].setRGB(0, 0, 0);
        }
        FastLED.show();
        readImu();
        mapAcFast();
        delay(mAcX);
}
    // Pong
    for(int pos=11; pos>0; pos--){ //boucle for sur les leds de chaque arrête
        for(int sommet=0; sommet<12; sommet++){
        leds[12*sommet+pos].setRGB(0, 0, 0);
        leds[12*sommet+pos-1].setRGB(pot1, pot2, pot3);
        }
        //FastLED.show();
        FastLED.show();
        delay(mAcX);
    }
}
/*SNAKE SUR LE RUBAN QUI CONSTITUE LE CUBE
* 
*/
void snake(){
FastLED.clear();
for(int k=0; k<144; k++){ //boucle for sur les leds
    leds[k].setRGB(0, 5, 3);
    //orderShow(leds);
    FastLED.show();
    delay(100);
}
}


/*AFFICHE UN SOMMET?
* 
*/
void sommet(int sommet){
FastLED.clear();
for(int k=0; k<12; k++){
    leds[12*sommet+k].setRGB(10,0,10);
    FastLED.show();
    delay(100);
}
delay(500);
FastLED.clear();
for(int k=0; k<12; k++){
    leds[12*sommet+k].setRGB(10,0,10);
    FastLED.show();
    delay(300);
}
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                          CODE AXEL 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// *************************** Fonctions outils **************************

//Renvoie le symértrique du point par rapport au centre

int symetrique(int point){
int q=point/12;
int r=point%12;
return (Symetrie[q])*12+11-r;

}


//Sachant un point de l'extremité du cube, transforme celui ci pour qu'il avance dans le cube correctement selon notre pliage

void passerAuSuivant(int& point, int choix){
int k;
k=(point+1)/12;
int snad;
if (point%12==0){
    snad = 2*k;
}
else{
    snad=2*k-1;
}
if(choix==0){
    point=L1[snad];
}
else{
    point=L2[snad];
}
}


//On veut parcourir simplement le cube selon la diagonale

void allumerPointVague(int element, int Red, int Green, int Blue){
    if(element > -1 && element <12){//on fait du coin zero vers son symétrique par rapport au centre
        leds[element].setRGB(Red,Green,Blue);
        leds[48+element].setRGB(Red,Green,Blue);
        leds[47-element].setRGB(Red,Green,Blue);
    }
    else if(element >11 && element<24){
        element=element-12;
        leds[131-element].setRGB(Red,Green,Blue);
        leds[132+element].setRGB(Red,Green,Blue);
        leds[71-element].setRGB(Red,Green,Blue);
        leds[12+element].setRGB(Red,Green,Blue);
        leds[95-element].setRGB(Red,Green,Blue);
        leds[35-element].setRGB(Red,Green,Blue);

    }
    else if(element > 23 && element < 36){
        element=element-24;
        leds[119-element].setRGB(Red,Green,Blue);
        leds[96+element].setRGB(Red,Green,Blue);
        leds[72+element].setRGB(Red,Green,Blue);
    }
}


//flash de manière random des pixels avant de le déflasher

void flashRandom(int quant){
    int flashDelay = 1;//Durée du flash
    int Points[quant];
    int RPoints[quant];
    int GPoints[quant];
    int BPoints[quant];
    int tirage;
    for(int i = 0 ; i<quant ; i++){
        tirage=random(NUM_LEDS);
        Points[i]=tirage;
        //On sauvegarde les points et les couleurs correspondantes
        RPoints[i]=leds[tirage].r;
        GPoints[i]=leds[tirage].g;
        BPoints[i]=leds[tirage].b;
        leds[tirage].setRGB(255,255,255); //FLASH ICI
    }
    FastLED.show();
    delay(flashDelay);
    for(int i=0 ; i<quant ; i++){
        //On replace les couleurs
        tirage=Points[i];
        leds[tirage].setRGB(RPoints[i],GPoints[i],BPoints[i]);
    }
    FastLED.show();
}



//effectue UN pas de ballade (avance de pixel) sans éteindre le précédent

void seBallade(int& point,int& sens, int Red, int Green, int Blue){
    R = Red;
    G = Green;
    B = Blue;
    int rando;
    leds[point].setRGB(R,G,B);
    if(sens==1 && point%12==11){
        passerAuSuivant(point,rando);
        if(point%12==11){
        sens=-1;
        }
    }
    else if(sens==-1 && point%12==0){
        passerAuSuivant(point,random(2));
        if(point%12==0){
        sens=1;
        }
    }
    else{
        point+=sens;
    }
    

}

//Même chose que seBallade() mais en plus crée le symétrique par rapport au centre du cube de la led qui se ballade

void seBalladeSym(int& point,int& sens, int Red, int Green, int Blue){//même chose que se ballade mais affiche aussi le symetrique
    R = Red;
    G = Green;
    B = Blue;
    int rando;
    leds[point].setRGB(R,G,B);
    leds[symetrique(point)].setRGB(R,G,B);
    if(sens==1 && point%12==11){
        passerAuSuivant(point,rando);
        if(point%12==11){
        sens=-1;
        }
    }
    else if(sens==-1 && point%12==0){
        passerAuSuivant(point,random(2));
        if(point%12==0){
        sens=1;
        }
    }
    else{
        point+=sens;
    }
}


//atténue toute les leds d'un facteur atténuation

void decrease(float attenuation){
for(int i=0; i<NUM_LEDS;i++){
    leds[i].setRGB(leds[i].r*attenuation, leds[i].g*attenuation,leds[i].b*attenuation);
    }
    FastLED.show();

}
// ************************************** Fonctions affichables ****************************************


/*BALLADE DE POINTS SUR LE CUBE AVEC EFEFT DE FADING 
* 
*/

void Ballade(){
    // Le hmi controle le nb de points avec le compteur qui vaut entre 0 et 12.
    readHmi();
    int nombre_de_points = compteurHmi+1;
    int taille = nombre_de_points;//Nombres de points qui se balladeront
    int Points[taille];
    int sens[taille];
    int Rouges[taille];
    int Verts[taille];
    int Bleus[taille];
    for(int i=0; i<taille;i++){
        Points[i]=random(143);
        sens[i]=2*random(2)-1;
        Rouges[i]=random(256);
        delay(1);
        Verts[i]=255-random(256);
        delay(1);
        Bleus[i]=random(256);
        delay(1);
    }
    float omeg=2*PI/200;
    for(int i=0 ; i<STEPS ; i++){
        readHmi();
        if(nextFlag){
            return;
        }
        if(nombre_de_points != compteurHmi+1){
            return;
        }
        for(int k=0; k<taille;k++){
            seBallade(Points[k],sens[k],(int)(Rouges[k]*(1-cos(omeg*(i+Rouges[k])))/2),  (int)(Verts[k]*(1-cos(omeg*(i+Verts[k])+2*PI/3))/2)  ,(int)(Bleus[k]*(1-cos(omeg*(i+Bleus[k])+4*PI/3))/2));
            FastLED.show();
        }
        FastLED.show();
        mapPots(0, 100);
        delay(100 - pot1);
        decrease(attenuation*pot2/85+yoloFlag);
        flashRandom((int)pot3/3);
    }
}




/*C'EST COMME BALLADE MAIS Y'A AUSSI LE SYMETRIQUE DU POINT QUI GAMBADE, DU COUP LES POINTS DANCENT
* 
*/

void dance(){
    readHmi();
    int nombre_de_points = compteurHmi+1;
    int taille = nombre_de_points;//Nombres de points qui se balladeront
    int Points[taille];
    int sens[taille];
    int Rouges[taille];
    int Verts[taille];
    int Bleus[taille];
    for(int i=0; i<taille;i++){
        Points[i]=random(143);
        sens[i]=2*random(2)-1;
        Rouges[i]=random(256);
        delay(1);
        Verts[i]=255-random(256);
        delay(1);
        Bleus[i]=random(256);
        delay(1);
    }
    float omeg=2*PI/200;
    for(int i=0 ; i<STEPS ; i++){
        readHmi();
        if(nextFlag){
            return;
        }
        if(nombre_de_points != compteurHmi+1){
            return;
        }
        for(int k=0; k<taille;k++){
        seBalladeSym(Points[k],sens[k],(int)(Rouges[k]*(1-cos(omeg*(i+Rouges[k])))/2),  (int)(Verts[k]*(1-cos(omeg*(i+Verts[k])+2*PI/3))/2)  ,(int)(Bleus[k]*(1-cos(omeg*(i+Bleus[k])+4*PI/3))/2));//On met un petit triphasé du zgeg pour que ça change de couleur quand ça brillasse
        FastLED.show();
        }
        //flashRandom(1);//Effet étoilé
        FastLED.show();
        mapPots(0, 100);
        delay(100 - pot1);
        decrease(attenuation*pot2/80+yoloFlag);
        flashRandom((int)pot3/3);
    }
}


/* COMME dance(1) MAIS PT EN NOIR SUR FOND ARC-EN-CIEL de la fonction RAINBOW
* 
*/

void darkBallade(){
    int P0=0;
    int sens0=1;
    float w = 2*PI/STEPS;
    for(int k=0; k<NUM_LEDS; k++){
        phi = 2*PI*k/NUM_LEDS;
        R = (1+sin(w+phi))/2*(255);
        G = (1+sin(w+phi+2*PI/3))/2*(255);
        B = (1+sin(w+phi+4*PI/3))/2*(255);
        leds[k].setRGB(R,G,B);
    }
    FastLED.show();
    for(int i=0; i<STEPS; i++){
        readHmi();
        if(nextFlag){
            return;
        }
        seBalladeSym(P0,sens0,0,0,0);
        FastLED.show();

        //On fait comme deacrease sauf que là ça converge vers l'arc en ciel
        for(int k=0; k<NUM_LEDS; k++){
            phi = 2*PI*k/NUM_LEDS;
            R = (attenuation*pot2/0.85/255)*leds[k].r+(1-(attenuation*pot2/0.85/255))*(1+sin(w+phi))/2*(255);
            G = (attenuation*pot2/0.85/255)*leds[k].g+(1-(attenuation*pot2/0.85/255))*(1+sin(w+phi+2*PI/3))/2*(255);
            B = (attenuation*pot2/0.85/255)*leds[k].b+(1-(attenuation*pot2/0.85/255))*(1+sin(w+phi+4*PI/3))/2*(255);
            leds[k].setRGB(R, G, B);
            
        }
        FastLED.show();
        mapPots(0, 100);
        delay(2*pot1);
        flashRandom((int)pot3/3);
    }

}


/*CREE UNE VAGUE TRASVERSALE SUR LE CUBE
* 
*/

void vagueDiagonale(){
    int nombreDeVagues = 2;
    readHmi();
    nombreDeVagues = constrain(compteurHmi-4, 1, 8);

    int tailleDuPaquet=(int)20*pot2/255;//Taille de l'enveloppe individuelle par vagfue

    int tailleEnveloppe = tailleDuPaquet*nombreDeVagues;//Taille de la vague totale
    float Enveloppe[tailleEnveloppe];
    float phi = nombreDeVagues*2*PI/(tailleEnveloppe-1.0);
    for(int i=0; i<tailleEnveloppe;i++){
        Enveloppe[i]=0.5*(1.0-cos(phi*i))*0.5*(1.0-cos(phi*i));
    }
    //On a initialisé l'enveloppe du signaleuh
    phi=2*PI/36.0;
    for(int i=0;i<tailleEnveloppe + 36;i++){
        for(int j=0;j<tailleEnveloppe;j++){
            readHmi();
            if(nextFlag){
                return;
            }
            int Red=255*Enveloppe[j]*0.5*(1+sin(phi*(i)));
            int Green=255*Enveloppe[j]*0.5*(1+sin(phi*(i)+2*PI/3));
            int Blue=255*Enveloppe[j]*0.5*(1+sin(phi*(i)+4*PI/3));
            allumerPointVague(i-j,Red,Green,Blue);
            
        }
        FastLED.show();
        mapPots(0, 100);
        delay(pot1);
        flashRandom((int)pot3/3);
    }
}




/* COMME BALLADE MAIS LE TEMPS EST SINUSOIDAL+0.5*T
* 
*/

void BalladeDemolie(){
    // Le hmi controle le nb de points avec le compteur qui vaut entre 0 et 12.
    readHmi();
    int taille = compteurHmi+1;
    int t = 0;//Le temps
    float periode= 60.0;

    int Points[taille];
    int sens[taille];
    int Rouges[taille];
    int Verts[taille];
    int Bleus[taille];
    for(int i=0; i<taille;i++){
        Points[i]=random(143);
        sens[i]=2*random(2)-1;
        Rouges[i]=random(256);
        delay(1);
        Verts[i]=255-random(256);
        delay(1);
        Bleus[i]=random(256);
        delay(1);
    }
    float omeg=2*PI/200;
    for(int i=0 ; i<STEPS ; i++){
        readHmi();
        if(nextFlag){
            return;
        }
        if(taille != compteurHmi+1){
            return;
        }
        for(int k=0; k<taille;k++){
            seBallade(Points[k],sens[k],(int)(Rouges[k]*(1-cos(omeg*(i+Rouges[k])))/2),  (int)(Verts[k]*(1-cos(omeg*(i+Verts[k])+2*PI/3))/2)  ,(int)(Bleus[k]*(1-cos(omeg*(i+Bleus[k])+4*PI/3))/2));
            FastLED.show();
            mapPots(0, 100);
            delay(100 - pot1);
            flashRandom((int)pot3/3);
        }
        
        t++;
        delay(pot1*(1+cos(2*PI/periode*t))/2);//Le temps d'attente est pas légal
        if(t%((int)periode)==0){//On inverse le sens
            for(int k=0;k<taille;k++){
                sens[k]=-sens[k];
            }
        } 
        decrease((attenuation*pot2/0.85/255)+yoloFlag);
    }
}


/* CA FAIT DES TOURNER DES POINTS RIGOLOS AUTOUR DU CUBE
* 
*/

void caTourne(){
    readHmi();
    int nombreDeSymetries = (int)compteurHmi/2;
    int Points[nombreDeSymetries];// Points du haut
    mapPots(0, 100);
    int periode=pot2+5;
    float omeg=2*PI/((float)periode);
    int sens =1;
    for(int i=0; i<nombreDeSymetries; i++){
        Points[i]=(int)(i*47/(float)(nombreDeSymetries));
    }
    for(int t=0; t<STEPS ; t++){
        readHmi();
        if(nextFlag){
            return;
        }
        for(int i=0;i<nombreDeSymetries;i++){
            R=255*(1-cos(omeg*t))/2;
            G=255*(1-cos(omeg*t+2*PI/3.0))/2;
            B=255*(1-cos(omeg*t+4*PI/3.0))/2;
            leds[Points[i]].setRGB(R,G,B);
            leds[symetrique(Points[i])].setRGB(R,G,B);
            Points[i]=(Points[i]+sens+47)%47;
        }
        if(t%periode==0 && random(2)==1){
            sens = -sens;
        }
        FastLED.show();
        mapPots(0, 100);
        delay(pot1);
        flashRandom((int)pot3/3);
        decrease((attenuation*pot2/0.85/255)+yoloFlag);
    }


}


/*COMME RAINBOW MAIS SANS DISCONTINUITE DE COULEURS
* 
*/

void rainbowCube(){
    readHmi();
    
    float omeg=3*PI/36.0;
    float vitesse=2*PI/(2+5*compteurHmi);
    int watch = compteurHmi;
    for(int t=0; t<(STEPS/40)*40+1; t++){
        readHmi();
        if(nextFlag){
            return;
        }
        if(compteurHmi !=watch){
            return;
        }
        for(int i=0;i<36;i++){
            R=(1-cos(omeg*i+vitesse*t))/2*255;
            G=(1-cos(omeg*i+vitesse*t+2*PI/3))/2*255;
            B=(1-cos(omeg*i+vitesse*t+4*PI/3))/2*255;       
            allumerPointVague(i,R,G,B);
        }
        FastLED.show();
        mapPots(0, 100);
        delay(pot1);
        flashRandom((int)pot3/3);
    }
}