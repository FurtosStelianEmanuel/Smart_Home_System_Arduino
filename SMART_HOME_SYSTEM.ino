#include <SoftwareSerial.h>
#include "NewPing.h"
#include <FastLED.h>
/*#include <stdio.h>*/

#define NUM_LEDS 75
#define ARGB_PIN 10

#define status_pin 13
#define LED_UPDATE_FREQUENCY 255
#define OFF_COUNTDOWN 100
#define door_freq 500
#define trigPin 11
#define echoPin 12
#define ir_threshold 8
#define ir_on_th 3
#define ir_off_th 13
#define RED_PIN 5
#define GREEN_PIN 6
#define BLUE_PIN 9
#define DOOR_PIN 8
#define BUFFER_SIZE 45
#define NORMAL_PRINTS_ENABLED false
#define RGB_ANIMATION_TOKEN 'h'
#define RGB_ANIMATION_TURN_ON '+'
#define RGB_ANIMATION_TURN_OFF '-'
#define RGB_SMOOTH_CHANGE ']'
#define INPUT_BYTES_BUFFER 20
#define ESP8266_MODULE true

#define ANIMATION_MODE_BUFFER_PARTITION_SIZE 6
#define ANIMATION_MODE_BUFFER_SIZE 3
#define RELAY_PIN_1 4

byte animationBuffer[ANIMATION_MODE_BUFFER_SIZE][NUM_LEDS];
bool ANIMATION_MODE = false;
//<editor-fold desc="Variabile si clase">

double off_state = PI;

unsigned int loop_counter = 0;
short int ir_interval = 2000;
short int state = 0;
short int automatic_shutdown_counter = 0;
short int off_countdown = OFF_COUNTDOWN;
short int ir_counter = 0;
short int lr = 0;
short int lg = 0;
short int lb = 0;
int screenMirrorIntensitate = 100;
int screenMirrorScale = 1;
int move = 0;
/**
 * variabila RELAY_COUNTER retine cate perioade a facut functia sin
 */
int RELAY_COUNTER = 0;
int relayThreshold = 2;
bool mirror = false;
bool established_connection = true;
bool door_open = true;
bool DEBUG = false;
bool shutdown_intrerupted = false;
bool comm = true;
bool pornireAutomata = true;
bool oprireAutomata = true;
bool recent_door_close = false;
bool a = false;
bool b = false;
bool c = false;
bool d = false;
bool TURBO_MODE = false;
bool turnOnAnimationCompleted = true;
bool turnOffAnimationCompleted = true;

char selectedAnimation_RGB = ' ';
const char PORNIRE = '0';
const char OPRIRE = '1';
const char CULOARE_PORNIRE = 'H';
char selectedAnimation_ARGB = ' ';
const char ANIMATION_ARGB_ROMANIAN_1 = '0';
const char ANIMATION_ARGB_BLENDING = '1';
const char ANIMATION_ARGB_LIGHT_OFF = '-';
const char ANIMATION_ARGB_LIGHT_ON = '+';
const char ANIMATION_ARGB_SET_COLOR = '2';
const char ANIMATION_ARGB_FADER = '3';
char ar[BUFFER_SIZE];
char err[3] = "inv";
double intensitate = 1;

struct Color {
    int r, g, b;
public:

    Color(int r, int g, int b) {
        this -> r = r;
        this -> g = g;
        this -> b = b;
    }

    Color() {

    }
} on_color(255, 255, 255), smooth_changeColor(255, 88, 0);
CRGB leduri_argb[NUM_LEDS];
SoftwareSerial BTserial(0, 1);
NewPing sonar(trigPin, echoPin, 600);
//</editor-fold>

void testARGBStripLength(){
  for (int i=0;i<NUM_LEDS;i++){
    leduri_argb[i]=CRGB::Black;
  }
  FastLED.show();
  for (int i=0;i<NUM_LEDS;i++){
    leduri_argb[i]=CRGB::White;
    delay(500);
    FastLED.show()
  }
}

double mapd(double x, double in_min, double in_max, double out_min, double out_max) {
    return ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

void light_on() {
    /*for (int i = 0; i <= 255; i++) {
        if (i >= lr)
            pwm_write(RED_PIN, i);
        if (i >= lg)
            pwm_write(GREEN_PIN, i);
        if (i >= lb)
            pwm_write(BLUE_PIN, i);
        delay(2);
    }*/
    selectedAnimation_RGB = RGB_ANIMATION_TURN_ON;
    turnOnAnimationCompleted = false;
    if (ESP8266_MODULE) {
        Serial.println("y1");
    }

}

void light_on(Color c, int sleep) {
    if (ESP8266_MODULE) {
        Serial.println("y1");
    }
    smooth_change(c.r, c.g, c.b);
}

/**
 * primeste doua caractere in hexadecimal, practic un numar in hexadecimal si returneaza
 * valoarea lui in decimal
 * @param c1 orice caracter de la 0 la f
 * @param c2 orice caracter de la 0 la f
 * @return valori din intervalul [0,255]
 */
int getColor(char c1, char c2) {
    int v1 = -1;
    String hex = "abcdef";
    if ((c1 - '0') < 10) {
        v1 = (c1 - '0');
    } else {
        v1 = hex.indexOf(c1) + 10;
    }
    int v2 = -1;
    if ((c2 - '0') < 10) {
        v2 = (c2 - '0');
    } else {
        v2 = hex.indexOf(c2) + 10;
    }

    int total = v1 * 16 + v2;
    return total;
}

/**
 * @param sleep
 * DEPRECATED
 */
void light_on(int sleep) {
    for (int i = 0; i <= 255; i++) {
        if (i >= lr)
            pwm_write(RED_PIN, i);
        if (i >= lg)
            pwm_write(GREEN_PIN, i);
        if (i >= lb)
            pwm_write(BLUE_PIN, i);
        delay(sleep);
    }

}

void pwm_write(int pin, int val) {
    if (val > 255) {
        val = 255;
    }
    if (val < 0) {
        val = 0;
    }

    if (pin == RED_PIN)
        lr = val;
    if (pin == GREEN_PIN)
        lg = val;
    if (pin == BLUE_PIN)
        lb = val;
    analogWrite(pin, val);
}

void light_off() {

    /*int start = max(max(lr, lg), lb);
    for (int i = start; i >= 0; i--) {
        if (i <= lr)
            pwm_write(RED_PIN, i);
        if (i <= lg)
            pwm_write(GREEN_PIN, i);
        if (i < lb)
            pwm_write(BLUE_PIN, i);
        delay(2);
    }*/
    selectedAnimation_RGB = RGB_ANIMATION_TURN_OFF;
    turnOffAnimationCompleted = false;
    Serial.println("y0");
}

int readIR() {
    {//cod vechi
        /*
        digitalWrite(trigPin, LOW);
        delayMicroseconds(5);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        pinMode(echoPin, INPUT);
        duration = pulseIn(echoPin, HIGH);
        return cm = (duration / 2) / 29.1; 
         */
    }
    unsigned int uS = sonar.ping_cm();
    if (uS == 0) {
        uS = 400;
    }
    return uS;
}

void door_closed() {

    if (oprireAutomata) {
        shutdown_intrerupted = false;
        automatic_shutdown_counter = 0;
        recent_door_close = true;
        off_countdown = OFF_COUNTDOWN;
        if (NORMAL_PRINTS_ENABLED) {
            Serial.println("Inchis");
        }
    }

    if (ESP8266_MODULE) {
        Serial.println("u1");
    }
    delay(500);

}

void door_opened() {
    d = false; // oprim animatia de hang atunci cand luminile sunt pe cale sa se stinga
    digitalWrite(status_pin, LOW); //

    if (ESP8266_MODULE) {
        Serial.println("u0");
    }

    if (pornireAutomata) {
        if (lr == 0 && lg == 0 && lb == 0) {
            light_on(on_color, 2);
            off_countdown = OFF_COUNTDOWN;
        }
        if (NORMAL_PRINTS_ENABLED) {
            Serial.println("Deschis");
        }
    }
}

void door_events() {
    if (DEBUG) {
        if (NORMAL_PRINTS_ENABLED) {
            Serial.println("called door_events()");
        }
    }
    int read_d = digitalRead(DOOR_PIN);
    if (read_d == LOW && door_open) {
        door_open = false;
        door_closed();
    } else if (read_d == HIGH && !door_open) {
        door_open = true;
        door_opened();
    } else if (read_d == LOW && !shutdown_intrerupted && automatic_shutdown_counter < 1 && oprireAutomata) {
        off_countdown--;
        digitalWrite(status_pin, !digitalRead(status_pin));
        if (off_countdown < 0) {
            if (lr != 0 && lg != 0 && lb != 0) {
                d = true;
            }
            a = false;
            b = false;
            c = false;

            if (off_countdown < -15) {
                d = false;
                if (NORMAL_PRINTS_ENABLED) {
                    Serial.println("Automatic light-off");
                }
                recent_door_close = false;
                off_countdown = OFF_COUNTDOWN;
                automatic_shutdown_counter++;
                digitalWrite(status_pin, LOW);
                turnOffRelay('A');
                light_off();
            }

        }
    }
}

void setup() {

    Serial.begin(115200);
    for (int i = 0; i <= 13; i++)
        pinMode(i, OUTPUT);
    Serial.setTimeout(0);
    BTserial.begin(115200);
    BTserial.setTimeout(0);

    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(DOOR_PIN, HIGH);

    initLEDs();

}

long mymap(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

bool shouldTurnOnLight() {
    return turnOnAnimationCompleted && turnOffAnimationCompleted && (lr != 255 || lg != 255 || lb != 255);
}

bool shouldTurnOffLight() {
    return turnOnAnimationCompleted && turnOffAnimationCompleted && (lr == 255 && lg == 255 && lb == 255);
}

void handle_ir() {
    static bool frozen = false;
    if (DEBUG) {
        if (NORMAL_PRINTS_ENABLED) {
            Serial.println("called handle_ir");
        }
    }
    static int raw = 400;
    int readir = readIR();
    int ir = lerp(raw, readir, 0.99);

    if (ir <= ir_threshold) {
        ir_counter++;
        if (ir_counter >= 2) {
            state = 1;
            if (recent_door_close) {
                recent_door_close = false;
                shutdown_intrerupted = true;
                frozen = true;
                /*turnOffAnimationCompleted = true;
                turnOnAnimationCompleted = true;*/
            }
        }
        if (state && !frozen) {
            if (shouldTurnOnLight()) {
                RELAY_COUNTER++;
                if (RELAY_COUNTER >= relayThreshold) {
                    relayEvent('A', (char) (!digitalRead(RELAY_PIN_1) + '0'));
                    relayThreshold = 2 * 2;
                    RELAY_COUNTER = 0;
                    frozen = true;
                    turnOnAnimationCompleted = true;
                    light_off();
                } else {
                    light_on();
                }
            } else if (shouldTurnOffLight()) {
                light_off();
            } else {

            }
        }

    } else if (abs(readir - raw) < 10) {
        ir_counter = 0;
        state = 0;
        RELAY_COUNTER = 0;
        relayThreshold = 2;
        frozen = false;

        turnOnAnimationCompleted = true;
        turnOffAnimationCompleted = true;
    }
    digitalWrite(status_pin, state);
    raw = readir;
}

void checkSimpleAnimations() {
    /*if (a) {
        crossFade(red);
        crossFade(green);
        crossFade(blue);
        crossFade(yellow);
    } else if (b) {
        if (loop_counter % bwait == 0) {
            if (rv > 0 && bv == 0) {
                rv--;
                gv++;
            }
            if (gv > 0 && rv == 0) {
                gv--;
                bv++;
            }
            if (bv > 0 && gv == 0) {
                rv++;
                bv--;
            }
            pwm_write(RED_PIN, rv);
            pwm_write(GREEN_PIN, gv);
            pwm_write(BLUE_PIN, bv);
        }

    } else if (c) {
        if (!(loop_counter % solodelay)) {
            pwm_write(cfadepin, mymap(sin(animcounter), -1, 1, min(minfade, maxfade), max(maxfade, minfade)));
            animcounter += animspeed;
        }
    } */
    if (d) {

        const double val = sin(off_state);
        pwm_write(RED_PIN, mymap(val, -1, 1, 0, 255));
        pwm_write(GREEN_PIN, mymap(val, -1, 1, 0, 255));
        pwm_write(BLUE_PIN, mymap(val, -1, 1, 0, 255));
        off_state += 0.001;
    }
}

void RGBLooper() {
    switch (selectedAnimation_RGB) {
        case RGB_ANIMATION_TURN_ON:
            pwm_write(RED_PIN, lr + get_dir(lr, 255, 5));
            pwm_write(GREEN_PIN, lg + get_dir(lg, 255, 5));
            pwm_write(BLUE_PIN, lb + get_dir(lb, 255, 5));
            if (lr >= 255 && lg >= 255 && lb >= 255) {
                selectedAnimation_RGB = ' ';
                turnOnAnimationCompleted = true;
            }
            break;
        case RGB_ANIMATION_TURN_OFF:
            //#define RGB_TURN_ANIMATION_SPEED 5;
            pwm_write(RED_PIN, lr + get_dir(lr, 0, 5));
            pwm_write(GREEN_PIN, lg + get_dir(lg, 0, 5));
            pwm_write(BLUE_PIN, lb + get_dir(lb, 0, 5));
            if (lr <= 0 && lg <= 0 && lb <= 0) {
                selectedAnimation_RGB = ' ';
                turnOffAnimationCompleted = true;
            }
            break;
        case RGB_SMOOTH_CHANGE:
            pwm_write(RED_PIN, lr + get_dir(lr, smooth_changeColor.r, 5));
            pwm_write(GREEN_PIN, lg + get_dir(lg, smooth_changeColor.g, 5));
            pwm_write(BLUE_PIN, lb + get_dir(lb, smooth_changeColor.b, 5));
            if (lr == smooth_changeColor.r && lg == smooth_changeColor.g && lb == smooth_changeColor.b) {
                selectedAnimation_RGB = ' ';
            }
            break;
    }
}

void checkQueue() {
    if (loop_counter % ir_interval == 0) {
        handle_ir();
    }
    if (loop_counter % door_freq == 0) {
        door_events();
    }
    if (loop_counter % LED_UPDATE_FREQUENCY == 0) {
        ARGBLooper();
        RGBLooper();
    }
    checkSimpleAnimations();
}

void loop() {

    if (!ANIMATION_MODE && !TURBO_MODE) {
        checkQueue();
    }
    loop_counter++;
}

void printBackARGBSettings() {
    Serial.print("T+");
    Serial.print(BUFFER_SIZE);
    Serial.println();
}

int calculateStep(int prevValue, int endValue) {
    int step = endValue - prevValue;
    if (step) {
        step = 1020 / step;
    }
    return step;
}

int calculateVal(int step, int val, int i) {

    if ((step) && i % step == 0) {
        if (step > 0) {
            val += 1;
        } else if (step < 0) {
            val -= 1;
        }
    }
    if (val > 255) {
        val = 255;
    } else if (val < 0) {
        val = 0;
    }
    return val;
}

/*void crossFade(int color[3]) {
    int R = (color[0] * 255) / 100;
    int G = (color[1] * 255) / 100;
    int B = (color[2] * 255) / 100;

    int stepR = calculateStep(prevR, R);
    int stepG = calculateStep(prevG, G);
    int stepB = calculateStep(prevB, B);
    for (int i = 0; i <= 1020; i++) {
        redVal = calculateVal(stepR, redVal, i);
        grnVal = calculateVal(stepG, grnVal, i);
        bluVal = calculateVal(stepB, bluVal, i);
        pwm_write(RED_PIN, redVal);
        pwm_write(GREEN_PIN, grnVal);
        pwm_write(BLUE_PIN, bluVal);
        if (loop_counter % 50 == 0) {
            handle_ir();
            if (!a)
                return;
        }
        loop_counter++;
        delay(wait);

    }
    prevR = redVal;
    prevG = grnVal;
    prevB = bluVal;
    delay(hold);
}
 */

int get_dir(int start, int dest, int maxSpeed) {
    if (start < dest) {
        int arive = start + maxSpeed;
        while (arive > dest) {
            arive--;
            maxSpeed--;
        }
        return maxSpeed;
    } else if (start > dest) {
        int arive = start - maxSpeed;
        maxSpeed = -maxSpeed;
        while (arive < dest) {
            arive++;
            maxSpeed++;
        }
        return maxSpeed;
    }
    return 0;
}

int get_dir(int start, int dest) {
    if (start < dest)
        return 1;
    if (start > dest)
        return -1;
    return 0;
}

void smooth_change(int desired_r, int desired_g, int desired_b) { //lr,lg,lb
    smooth_changeColor = Color(desired_r, desired_g, desired_b);
    selectedAnimation_RGB = RGB_SMOOTH_CHANGE;
}

void disconnected() {
    oprireAutomata = true;
    pornireAutomata = true;
    //if (!door_open)
    light_off();
    turnOffRelay('A');
    selectedAnimation_ARGB = ' ';
    for (int i = 0; i < NUM_LEDS; i++) {
        leduri_argb[i] = CRGB::Black;
    }
    FastLED.show();
}

boolean disconnectString(char data[]) {
    if (data[0] == '+' && data[1] == 'D' && data[2] == 'I' && data[3] == 'S' && data[4] == 'C')
        return true;
    if (data[0] == 'O' && data[1] == 'K')
        return true;
    return false;
}

float lerp(float point1, float point2, float alpha) {
    return point1 + alpha * (point2 - point1);
}

int getASCII(char b) {
    int val = b;
    val += 128;
    return val;
}

/**
 * Aici avem limitare de maxim 128 de leduri, dar poate gasim un workaround, mai adaugam un byte pt jumpCount daca vrem mai mult de 128 de leduri
 * @param b
 * @return 
 */
int getStartingIndex(byte b) {
    int v = (int) b;
    v = v < 0 ? -v : v;
    return v;
}

/**
 * trebuie sa trimiti setarea unei culori de doua ori pentru a confirma valabilitatea comenzii
 */
int setColorCorrectCounter = 0;

int checkR = 0, checkG = 0, checkB = 0;

byte inputBytes[INPUT_BYTES_BUFFER];

byte getColor(byte raw) {
    if (raw >= 0 && raw <= 127) {
        raw = raw + 128;
    } else if (raw > 127 && raw <= 255) {
        raw = raw - 128;
    }
    return raw;
}

void turnOnRelay(char relay) {
    switch (relay) {
        case '1':
            digitalWrite(RELAY_PIN_1, HIGH);
            break;
        case 'A':
            //pana acuma doar cu relay_pin_1 avem treaba
            digitalWrite(RELAY_PIN_1, HIGH);
            break;
    }
}

void turnOffRelay(char relay) {
    switch (relay) {
        case '1':
            digitalWrite(RELAY_PIN_1, LOW);
            break;
        case 'A':
            //sa se opreasca toate releurile cand se inchide usa de exemplu
            //pana acuma doar cu relay_pin_1 avem treaba
            digitalWrite(RELAY_PIN_1, LOW);
            break;
    }
}

void relayEvent(char relay, char state) {
    state == '1' ? turnOnRelay(relay) : turnOffRelay(relay);
}

void handlebt(String data) {
    char data_arr[data.length()];
    data.toCharArray(data_arr, data.length());

    if (data_arr[0] == 'L') {
        if (data_arr[1] == '1') {
            light_on();
        } else {
            light_off();
        }
    } else if (data_arr[0] == 'K') {
        byte red = getColor(inputBytes[1]);
        byte green = getColor(inputBytes[2]);
        byte blue = getColor(inputBytes[3]);
        smooth_change(red, green, blue);
    } else if (data_arr[0] == 'J') {
        switch (data_arr[1]) {
            case PORNIRE:
                pornireAutomata = data_arr[2] == '1' ? true : false;
                break;
            case OPRIRE:
                oprireAutomata = data_arr[2] == '1' ? true : false;
                break;
            case CULOARE_PORNIRE:
                on_color.r = getColor(inputBytes[2]);
                on_color.g = getColor(inputBytes[3]);
                on_color.b = getColor(inputBytes[4]);
                break;
        }
    } else if (disconnectString(data_arr)) {
        disconnected();
    } else if (data_arr[0] == '>') {
        //Serial.print("m-a intepat cineva");
    } else if (inputBytes[0] == 'G') {
        byte red = getColor(inputBytes[1]);
        byte green = getColor(inputBytes[2]);
        byte blue = getColor(inputBytes[3]);

        pwm_write(RED_PIN, red);
        pwm_write(GREEN_PIN, green);
        pwm_write(BLUE_PIN, blue);
    } else if (data_arr[0] == 'j') {
        if (data_arr[1] == '?' || data_arr[2] == '?') {
            //Deprecated documentation
        } else {
            if (data_arr[1] != 's' && data_arr[1] != 'g' && data_arr[1] != 'b' && data_arr[1] != 'T') {
                updateSelectedAnimation(data_arr[1], data_arr, data.length());
            } else if (data_arr[1] == 's') {
                char * p = strtok(data_arr, " ");
                if (selectedAnimation_ARGB != ' ') {
                    animationFinished();
                }
                int index = -1;
                p = strtok(NULL, " ");
                if (p != NULL) {
                    index = atoi(p);
                    if (index < 0 || index > NUM_LEDS) {
                        index = -1;
                    }
                }
                p = strtok(NULL, " ");
                if (p != NULL) {
                    if (strlen(p) == 6) {
                        int r = getColor(p[0], p[1]);
                        int g = getColor(p[2], p[3]);
                        int b = getColor(p[4], p[5]);
                        if (index > -1) {
                            leduri_argb[index] = CRGB(r, b, g);
                            FastLED.show();
                        }
                    }
                }
            } else if (data_arr[1] == 'g') {
                char * p = strtok(data_arr, " ");
                if (selectedAnimation_ARGB != ' ') {
                    animationFinished();
                }
                int index = -1;
                p = strtok(NULL, " ");
                if (p != NULL) {
                    index = atoi(p);
                    if (index < 0 || index > NUM_LEDS) {
                        index = -1;
                    }
                }

                p = strtok(NULL, " ");
                int offset = 0;
                float alph = 0.35;
                while (p != NULL && index>-1) {
                    if (strlen(p) == 6) {

                        int r = (int) lerp(leduri_argb[index + offset].red, getColor(p[0], p[1]), alph);
                        int g = (int) lerp(leduri_argb[index + offset].blue, getColor(p[2], p[3]), alph);
                        int b = (int) lerp(leduri_argb[index + offset].green, getColor(p[4], p[5]), alph);
                        leduri_argb[index + offset] = CRGB(r, b, g);
                        offset++;
                    }
                    p = strtok(NULL, " ");
                }
                FastLED.show();
            }
        }
    } else if (data_arr[0] == 'T' && data_arr[1] == '-') {
        /*
        char *p = strtok(data_arr, " ");
        p = strtok(NULL, " ");
        short int c=0;
        if (p != NULL) {
            screenMirrorScale = atoi(p);
            c++;
        }
        p = strtok(NULL, " ");
        if (p != NULL) {
            screenMirrorIntensitate = atoi(p);
            intensitate = (double) screenMirrorIntensitate / 100.0;
            c++;
        }
        p=strtok(NULL," ");
        if (p!=NULL){
            move=atoi(p);
            c++;
        }
        p=strtok(NULL," ");
        if (p!=NULL){
            mirror=(atoi(p)==1 ? true:false);
            c++;
        }
        if (c == 4) {
            printBackARGBSettings();
        }
         */
        byte scara = getColor(inputBytes[2]);
        byte screenMirrorIntensitate = getColor(inputBytes[3]);
        byte offset = getColor(inputBytes[4]);
        byte oglindit = getColor(inputBytes[5]);
        byte suma = inputBytes[0];
        for (int i = 1; i < 6; i++) {
            suma += inputBytes[i];
        }
        if (suma == inputBytes[6]) {
            screenMirrorScale = scara;
            intensitate = (double) screenMirrorIntensitate / 100.0;
            ;
            move = offset;
            mirror = oglindit == 1 ? true : false;
            printBackARGBSettings();
        } else {
            Serial.println(err);
        }
    } else if (data_arr[0] == 'R') {
        relayEvent(data_arr[1], data_arr[2]);
    } else if (data_arr[0] == 'T' && data_arr[1] == '1') {
        TURBO_MODE = true;
        Serial.println("T1");
        selectedAnimation_ARGB = ' ';
        for (int i = 0; i < NUM_LEDS; i++) {
            leduri_argb[i] = CRGB::Black;
        }
    } else if (data_arr[0] == 'A' && data_arr[1] == '1') {
        ANIMATION_MODE = true;
        Serial.println("A1");
        selectedAnimation_ARGB = ' ';
        for (int i = 0; i < NUM_LEDS; i++) {
            leduri_argb[i] = CRGB::Black;
        }
    }
    if (data_arr[0] == 'T' && data_arr[1] == '0') {
        TURBO_MODE = false;
    }
    if (data_arr[0] == 'A' && data_arr[1] == '0') {
        ANIMATION_MODE = false;
        Serial.println(0x06); //il folosesc si pe post de ping-pong
    }
}



int bigCounter = 0;
int counter = 0;

void serialEvent() {
    if (!TURBO_MODE && !ANIMATION_MODE) {
        int n = Serial.readBytes(inputBytes, INPUT_BYTES_BUFFER);
        //String data = String(inputBytes);
        handlebt(inputBytes);
    } else if (TURBO_MODE) {
        int n = Serial.readBytes(ar, BUFFER_SIZE);
        if (ar[0] == 'T' && ar[1] == '0') {
            TURBO_MODE = false;
            Serial.println("T0");
            return;
        }
        int index = getStartingIndex(ar[1]);
        bigCounter = (index == 1 ? 0 : (screenMirrorScale * index - 2));


        if (n == BUFFER_SIZE && ar[0] == '<' && ar[n - 1] == '>') {

            int offset, counter;
            if (!mirror) {
                for (offset = 2, counter = index - 1; offset < BUFFER_SIZE - 1 && counter < NUM_LEDS; offset += 3, counter++) {
                    int red = getASCII(ar[offset]) * intensitate;
                    int green = getASCII(ar[offset + 1]) * intensitate;
                    int blue = getASCII(ar[offset + 2]) * intensitate;
                    if (screenMirrorScale == 1) {
                        leduri_argb[move + counter] = CRGB(red, blue, green);
                    } else {
                        for (int zoom = 1; zoom <= screenMirrorScale; zoom++) {
                            leduri_argb[move + bigCounter++] = CRGB(red, blue, green);
                        }
                    }

                }
            } else {
                for (offset = 2, counter = index - 1; offset < BUFFER_SIZE - 1 && counter < NUM_LEDS; offset += 3, counter++) {
                    int red = getASCII(ar[offset]) * intensitate;
                    int green = getASCII(ar[offset + 1]) * intensitate;
                    int blue = getASCII(ar[offset + 2]) * intensitate;
                    if (screenMirrorScale == 1) {
                        leduri_argb[NUM_LEDS - counter - 1 - move] = CRGB(red, blue, green);
                    } else {
                        for (int zoom = 1; zoom <= screenMirrorScale; zoom++) {
                            leduri_argb[NUM_LEDS - (bigCounter++) - 1 - move] = CRGB(red, blue, green);
                        }
                    }

                }
            }
            FastLED.show();

        } else {
            //Serial.println("err");
        }
    } else if (ANIMATION_MODE) {
        int n = Serial.readBytes(ar, ANIMATION_MODE_BUFFER_PARTITION_SIZE);
        if (n == ANIMATION_MODE_BUFFER_PARTITION_SIZE) { //verificare conditie de iesire [255,255,255,255,255,255]
            bool complet = true;
            for (int i = 0; i < 6; i++) {
                if (getColor(ar[i]) != 255) {
                    complet = false;
                }
            }
            if (complet) {
                ANIMATION_MODE = false;
                Serial.println("A0");
                return;
            }
        }
        if (n == ANIMATION_MODE_BUFFER_PARTITION_SIZE) {
            int index = getColor(ar[0]);
            if (index < NUM_LEDS) {
                for (int i = 1; i < ANIMATION_MODE_BUFFER_PARTITION_SIZE; i++) {
                    byte color = ar[i];
                    byte red = ((color & 0b11100000) >> 5)*36;
                    byte green = ((color & 0b00011100) >> 2)*36;
                    byte blue = (color & 0b00000011)*36;
                    if (index + i - 1 < NUM_LEDS) {
                        leduri_argb[index + i - 1] = CRGB(red, blue, green);
                    }
                }
                Serial.println(0x06);
            } else {
                Serial.println(err);
            }
        } else if (n == 1) {
            int index = getColor(ar[0]);
            if (index == 255) {
                FastLED.show();
                Serial.println(0x06);
            } else {
                if (index < NUM_LEDS) {
                    leduri_argb[index] = CRGB(0, 0, 0);
                    Serial.println(0x06);
                } else {
                    Serial.println(err);
                }
            }
        } else if (n == 2) {
            int index = getColor(ar[0]);
            byte color = ar[1];

            byte red = ((color & 0b11100000) >> 5)*36;
            byte green = ((color & 0b00011100) >> 2)*36;
            byte blue = (color & 0b00000011)*36;
            if (index < NUM_LEDS) {
                leduri_argb[index] = CRGB(red, blue, green);
                Serial.println(0x06);
            } else {
                Serial.println(err);
            }
        } else if (n == 3) {
            int indexStart = getColor(ar[0]);
            int indexEnd = getColor(ar[3]);
            byte color = ar[1];
            byte red = ((color & 0b11100000) >> 5)*36;
            byte green = ((color & 0b00011100) >> 2)*36;
            byte blue = (color & 0b00000011)*36;
            for (indexStart; indexStart <= indexEnd && indexStart < NUM_LEDS; indexStart++) {
                leduri_argb[indexStart] = CRGB(red, blue, green);
            }
            Serial.println(0x06);
        } else {
            Serial.println(err);
        }


    }

}
