#define LED_R 4 // Pin D3 --> червоний
#define LED_G 5 // Pin D5 --> зелений
#define LED_B 6 // Pin D6 --> синій

int red();
int green();
int blue();

void setup ()  {

    pinMode( LED_R, OUTPUT );
    pinMode( LED_G, OUTPUT );
    pinMode( LED_B, OUTPUT );
    analogWrite (LED_R,255);
    analogWrite (LED_G,255);
    analogWrite (LED_B,255);
    Serial.begin(9600);
    Serial.println("Type 1 to ON first RGB mode, 2 to ON second RGB mode");

}

void loop ()  {
    if (Serial.available() > 0){
        switch (Serial.read())
        {
            case '1':
                firstMode();
                break;
            case '2':
                secondMode();
                break;
        }
    }
}

void firstMode(){
    //-------------запалюєм і гасим червоний
    {
        int x = 1;
        for (int i = 0; i > -1; i = i + x){
            red(i);
            if (i == 255) x = -1;             //переключення в сторону гасіння на максимумі свічення
            delay(5);
        }
    }
//-------------запалюєм і гасим зелений
    {
        int x = 1;
        for (int i = 0; i > -1; i = i + x){
            green(i);
            if (i == 255) x = -1;             // переключення в сторону гасіння на максимумі свічення
            delay(5);
        }
    }
//-------------запалюєм і гасим синій
    {
        int x = 1;
        for (int i = 0; i > -1; i = i + x){
            blue(i);
            if (i == 255) x = -1;             // переключення в сторону гасіння на максимумі свічення
            delay(5);
        }
    }

//------------- мигаємо кожним кольором по три рази
    red(0);
    delay(100);
    red(80);
    delay(100);
    red(0);
    delay(100);
    red(160);
    delay(100);
    red(0);
    delay(100);
    red(255);
    delay(100);
    red(0);
    delay(100);
    green(80);
    delay(100);
    green(0);
    delay(100);
    green(160);
    delay(100);
    green(0);
    delay(100);
    green(255);
    delay(100);
    green(0);
    delay(100);
    blue(80);
    delay(100);
    blue(0);
    delay(100);
    blue(160);
    delay(100);
    blue(0);
    delay(100);
    blue(255);
    delay(100);
    blue(0);
//-------------мигаємо випадковими кольорами і випадковою яскравістю
    for (int i=0; i <= 50; i++){
        int color=(random(3)+1);
        if (color=1) {red(random(256)); delay(100);red(0);}
        if (color=2) {green(random(256)); delay(100);green(0);}
        if (color=3) {blue(random(256)); delay(100);blue(0);}
    }
}

//***************************************************
void red(int s){
    analogWrite (LED_R,255-s);
}
//***************************************************
void green(int s){
    analogWrite (LED_G,255-s);
}
//***************************************************
void blue(int s){
    analogWrite (LED_B,255-s);
}

void secondMode(){
    // Перший колір
    for(int i = 0; i < 15; i++) {
        analogWrite(LED_R, 0);
        analogWrite(LED_G, 0);
        analogWrite(LED_B, 255 * i / 15);
        delay(100);
        analogWrite(LED_R, 0);
        analogWrite(LED_G, 0);
        analogWrite(LED_B, 0);
    }

    // Другий колір
    for(int i = 0; i < 15; i++) {
        analogWrite(LED_R, 0);
        analogWrite(LED_G, 255 * i / 15);
        analogWrite(LED_B, 0);
        delay(100);
        analogWrite(LED_R, 0);
        analogWrite(LED_G, 0);
        analogWrite(LED_B, 0);
    }

    // Третій колір
    for(int i = 0; i < 15; i++) {
        analogWrite(LED_R, 255 * i / 15);
        analogWrite(LED_G, 0);
        analogWrite(LED_B, 0);
        delay(100);
        analogWrite(LED_R, 0);
        analogWrite(LED_G, 0);
        analogWrite(LED_B, 0);
    }
}