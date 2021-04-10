
#define BRIGHTNESS  100



int animationCounter = 0;
double brightness[NUM_LEDS];

//<editor-fold desc="Clase pentru animatii" defaultstate="collapsed">

class RomanianFlagAnimation {
public:
    int flags;
    int flagSize;
    int flagSpeed;
    int flagDirection;
    int flagOrientation;
    int flagBrightness;

    RomanianFlagAnimation(int flags, int flagSize, int flagSpeed, int flagDirection, int flagOrientation, int flagBrightness) {
        this->flags = flags;
        this->flagSize = flagSize;
        this->flagSpeed = flagSpeed;
        this->flagDirection = flagDirection;
        this->flagOrientation = flagOrientation;
        this->flagBrightness = flagBrightness;
    }

    void init() {
        //<editor-fold desc="body" defaultstate="collapsed">

        //<editor-fold desc="Parsing de date" defaultstate="collapsed">
        byte suma = inputBytes[0];
        for (int i = 1; i < 8; i++) {
            suma += inputBytes[i];
        }
        if (suma != inputBytes[8]) {
            Serial.println(err);
            return;
        }

        flags = getColor(inputBytes[2]);
        flagSize = getColor(inputBytes[3]);
        flagSpeed = map(getColor(inputBytes[4]), 0, 255, 0, LED_UPDATE_FREQUENCY);
        flagDirection = getColor(inputBytes[5]) == 0 ? 1 : -1;
        flagOrientation = getColor(inputBytes[6]) == 0 ? 1 : -1;
        flagBrightness = getColor(inputBytes[7]);

        selectedAnimation_ARGB = ANIMATION_ARGB_ROMANIAN_1;

        //</editor-fold>

        //<editor-fold desc="Initializarea altor variabile" defaultstate="collapsed">
        int pasteAmount = flagSize / 3;
        for (int i = 0; i < NUM_LEDS; i++) {
            leduri_argb[i] = CRGB::Black;
        }
        int i = 0;

        double bright = flagBrightness / 100.0;

        CRGB albastru = CRGB(0, 255 * bright, 0);
        CRGB galben = CRGB(255 * bright, 0, 255 * bright);
        CRGB rosu = CRGB(255 * bright, 0, 0);

        if (flagOrientation == 1) {
            for (int k = 0; k < flags; k++) {
                for (int j = 0; j < pasteAmount; j++) {
                    if (i + j < NUM_LEDS) {
                        leduri_argb[i + j] = albastru;
                    }
                }

                i += pasteAmount;

                for (int j = 0; j < pasteAmount; j++) {
                    if (i + j < NUM_LEDS) {
                        leduri_argb[i + j] = galben;
                    }
                }
                i += pasteAmount;

                for (int j = 0; j < pasteAmount; j++) {
                    if (i + j < NUM_LEDS) {
                        leduri_argb[i + j] = rosu;
                    }
                }
                i += pasteAmount + 3;
            }
        } else if (flagOrientation == -1) {
            for (int k = 0; k < flags; k++) {
                for (int j = 0; j < pasteAmount; j++) {
                    if (i + j < NUM_LEDS) {
                        leduri_argb[i + j] = rosu;
                    }
                }

                i += pasteAmount;

                for (int j = 0; j < pasteAmount; j++) {
                    if (i + j < NUM_LEDS) {
                        leduri_argb[i + j] = galben;
                    }
                }
                i += pasteAmount;

                for (int j = 0; j < pasteAmount; j++) {
                    if (i + j < NUM_LEDS) {
                        leduri_argb[i + j] = albastru;
                    }
                }
                i += pasteAmount + 3;
            }
        }
        //</editor-fold>

        //</editor-fold>
    }

    void looper() {
        //<editor-fold desc="body" defaultstate="collapsed">
        if (animationCounter % (LED_UPDATE_FREQUENCY / flagSpeed) == 0) {
            if (flagDirection == 1) {
                FastLED.show();
                CRGB aux = leduri_argb[NUM_LEDS - 1];
                for (int j = NUM_LEDS - 1; j > 0; j--) {
                    leduri_argb[j] = leduri_argb[j - 1];
                }
                leduri_argb[0] = aux;
            } else {
                FastLED.show();
                CRGB aux = leduri_argb[0];
                for (int j = 0; j < NUM_LEDS - 1; j++) {
                    leduri_argb[j] = leduri_argb[j + 1];
                }
                leduri_argb[NUM_LEDS - 1] = aux;
            }
        }
        //</editor-fold>
    }


} romanianFlagAnimation(1, 3 * 2, 250, 1, 1, 100);

class LightOffAnimation {
public:
    int animationSpeed;
    int decrement;
    int subtip;
    int sectiuni;

    LightOffAnimation(int animationSpeed, int decrement, int subtip, int sectiuni) {
        this->animationSpeed = animationSpeed;
        this->decrement = decrement;
        this->subtip = subtip;
        this->sectiuni = sectiuni;
    }

    void init() {
        //<editor-fold desc="body" defaultstate="collapsed">

        //<editor-fold desc="Parsing de date" defaultstate="collapsed">
        byte suma = inputBytes[0];
        for (int i = 1; i < 6; i++) {
            suma += inputBytes[i];
        }
        if (suma != inputBytes[6]) {
            Serial.println(err);
            return;
        }

        animationSpeed = getColor(inputBytes[2]);
        decrement = getColor(inputBytes[3]);
        subtip = getColor(inputBytes[4]);
        sectiuni = getColor(inputBytes[5]);

        selectedAnimation_ARGB = ANIMATION_ARGB_LIGHT_OFF;

        //</editor-fold>

        //<editor-fold desc="Alte initializari" defaultstate="collapsed">
        if (subtip == 2) {
            leduri_argb[0] = CRGB::Black;
            leduri_argb[NUM_LEDS - 1] = CRGB::Black;
            for (int i = 0; i < NUM_LEDS; i++) {
                brightness[i] = 0.0;
            }
        } else if (subtip == 3) {
            int pas = NUM_LEDS / sectiuni;
            for (int i = pas / 2; i < NUM_LEDS; i += pas) {
                leduri_argb[i] = CRGB::Black;
            }
            FastLED.show();
        } else if (subtip == 4) {
            int pas = NUM_LEDS / sectiuni;
            for (int i = 0; i < NUM_LEDS; i += pas) {
                leduri_argb[i] = CRGB(0, 0, 0);
            }
            FastLED.show();
        } else if (subtip == 5) {
            int pas = NUM_LEDS / sectiuni;
            for (int i = NUM_LEDS - 1; i > 0; i -= pas) {
                leduri_argb[i] = CRGB(0, 0, 0);
            }
            FastLED.show();
        }
        //</editor-fold>

        //</editor-fold>
    }

    void looper() {
        //<editor-fold desc="body" defaultstate="collapsed">
        if (animationCounter % (LED_UPDATE_FREQUENCY / animationSpeed) == 0) {
            //<editor-fold desc="LightOffAnimation.subtip 1">
            if (subtip == 1) {
                int sum = 0;
                for (int i = 0; i < NUM_LEDS; i++) {
                    int red = leduri_argb[i].red;
                    int green = leduri_argb[i].blue;
                    int blue = leduri_argb[i].green;
                    red -= decrement * 3;
                    if (red < 0) {
                        red = 0;
                    }
                    green -= decrement * 3;
                    if (green < 0) {
                        green = 0;
                    }
                    blue -= decrement * 3;
                    if (blue < 0) {
                        blue = 0;
                    }
                    sum += red + green + blue;
                    leduri_argb[i] = CRGB(red, blue, green);
                }
                if (sum <= 0) {
                    animationFinished();
                    for (int i = 0; i < NUM_LEDS; i++) {
                        leduri_argb[i] = CRGB::Black;
                    }
                }
                FastLED.show();
            }//</editor-fold>
                //<editor-fold desc="LightOffAnimation.subtip 2">
            else if (subtip == 2) {
                short int i = 0;
                while (i < NUM_LEDS / 2/*iesire de urgenta*/) {
                    i++;
                    if (leduri_argb[i].r != 0 || leduri_argb[i].g != 0 || leduri_argb[i].b != 0) {
                        break;
                    }
                }
                for (short int k = 0; k < decrement; k++) {
                    if (i + k < NUM_LEDS) {
                        leduri_argb[i + k] = CRGB::Black;
                    }
                }
                short int j = NUM_LEDS - 1;
                while (j > NUM_LEDS / 2) {
                    j--;
                    if (leduri_argb[j].r != 0 || leduri_argb[j].g != 0 || leduri_argb[j].b != 0) {
                        break;
                    }
                }
                for (short int k = 0; k < decrement; k++) {
                    if (j - k > 0) {
                        leduri_argb[j - k] = CRGB::Black;
                    }
                }
                if (i >= NUM_LEDS / 2 && j <= NUM_LEDS / 2) {
                    animationFinished();
                }
                FastLED.show();
            }//</editor-fold>
                //<editor-fold desc="LightOffAnimation.subtip 3">
            else if (subtip == 3) {
                int pas = NUM_LEDS / sectiuni;
                for (int i = 1; i < NUM_LEDS - 1; i++) {
                    bool culoareBuna = leduri_argb[i].red == 0 &&
                            leduri_argb[i].blue == 0 &&
                            leduri_argb[i].green == 0;
                    if (culoareBuna) {
                        leduri_argb[i - 1] = CRGB::Black;
                        while (i < NUM_LEDS/*iesire de urgenta*/) {
                            i++;
                            culoareBuna = leduri_argb[i].red == 0 &&
                                    leduri_argb[i].blue == 0 &&
                                    leduri_argb[i].green == 0;
                            if (!culoareBuna) {
                                break;
                            }
                        }
                        leduri_argb[i++] = CRGB::Black;
                    }
                }
                bool done = true;
                for (int i = 0; i < NUM_LEDS; i++) {
                    bool culoareBuna = leduri_argb[i].red == 0 &&
                            leduri_argb[i].blue == 0 &&
                            leduri_argb[i].green == 0;
                    if (!culoareBuna) {
                        done = false;
                        break;
                    }
                }
                if (done) {
                    animationFinished();
                }
                FastLED.show();
            }//</editor-fold>
                //<editor-fold desc="LightOffAnimation.subtip 4">
            else if (subtip == 4) {
                for (int i = 1; i < NUM_LEDS; i++) {
                    if (leduri_argb[i - 1].red == 0 &&
                            leduri_argb[i - 1].blue == 0 &&
                            leduri_argb[i - 1].green == 0 &&
                            (leduri_argb[i].red != 0 ||
                            leduri_argb[i].blue != 0 ||
                            leduri_argb[i].green != 0)
                            ) {
                        leduri_argb[i] = CRGB(0, 0, 0);
                        i++;
                    }
                }
                bool done = true;
                for (int i = 0; i < NUM_LEDS; i++) {
                    if (leduri_argb[i].red != 0 ||
                            leduri_argb[i].blue != 0 ||
                            leduri_argb[i].green != 0) {
                        done = false;
                        break;
                    }
                }
                if (done) {
                    animationFinished();
                }
                FastLED.show();
            }//</editor-fold>
                //<editor-fold desc="LightOffAnimation.subtip 5">
            else if (subtip == 5) {
                for (int i = NUM_LEDS - 2; i >= 0; i--) {
                    if (leduri_argb[i + 1].red == 0 &&
                            leduri_argb[i + 1].blue == 0 &&
                            leduri_argb[i + 1].green == 0 &&
                            (leduri_argb[i].red != 0 ||
                            leduri_argb[i].blue != 0 ||
                            leduri_argb[i].green != 0)
                            ) {
                        leduri_argb[i] = CRGB(0, 0, 0);
                        i--;
                    }
                }
                bool done = true;
                for (int i = 0; i < NUM_LEDS; i++) {
                    if (leduri_argb[i].red != 0 ||
                            leduri_argb[i].blue != 0 ||
                            leduri_argb[i].green != 0) {
                        done = false;
                        break;
                    }
                }
                if (done) {
                    animationFinished();
                }
                FastLED.show();
            }
            //</editor-fold>
        }
        //</editor-fold>
    }

} lightOffAnimation(LED_UPDATE_FREQUENCY, 1, 2, 4);

class LightOnAnimation {
public:
    int animationSpeed;
    int increment;
    int subtip;
    int sectiuni;
    Color culoare;

    LightOnAnimation(int animationSpeed, int increment, int subtip, Color culoare, int sectiuni) {
        this->animationSpeed = animationSpeed;
        this->increment = increment;
        this->subtip = subtip;
        this->culoare = culoare;
        this->sectiuni = sectiuni;
    }

    void init() {
        //<editor-fold desc="body" defaultstate="collapsed">

        //<editor-fold desc="Parsing de date" defaultstate="collapsed">

        byte suma = inputBytes[0];
        for (int i = 1; i < 9; i++) {
            suma += inputBytes[i];
        }
        if (suma != inputBytes[9]) {
            Serial.println(err);
            return;
        }

        culoare.r = getColor(inputBytes[2]);
        culoare.g = getColor(inputBytes[3]);
        culoare.b = getColor(inputBytes[4]);
        subtip = getColor(inputBytes[5]);
        animationSpeed = getColor(inputBytes[6]);
        increment = getColor(inputBytes[7]);
        sectiuni = getColor(inputBytes[8]);

        selectedAnimation_ARGB = ANIMATION_ARGB_LIGHT_ON;
        //</editor-fold>

        //<editor-fold desc="Initializarea altor variabile pentru animatii" defaultstate="collapsed">
        for (short int i = 0; i < NUM_LEDS; i++) {
            brightness[i] = 0.0;
        }
        if (subtip == 3) {
            brightness[NUM_LEDS / 2] = 1;
            brightness[NUM_LEDS / 2 + 1] = 0.3;
            brightness[NUM_LEDS / 2 + 2] = 0.2;
            brightness[NUM_LEDS / 2 + 3] = 0.15;
            brightness[NUM_LEDS / 2 + 4] = 0.05;
            brightness[NUM_LEDS / 2 + 5] = 0.01;

            brightness[NUM_LEDS / 2 - 1] = 0.3;
            brightness[NUM_LEDS / 2 - 2] = 0.2;
            brightness[NUM_LEDS / 2 - 3] = 0.15;
            brightness[NUM_LEDS / 2 - 4] = 0.05;
            brightness[NUM_LEDS / 2 - 5] = 0.01;

            for (short int i = 0; i < NUM_LEDS; i++) {
                double amount = brightness[i];
                if (amount > 0.0) {
                    short int red = culoare.r;
                    short int green = culoare.g;
                    short int blue = culoare.b;
                    red = red*amount;
                    green = green*amount;
                    blue = blue*amount;
                    leduri_argb[i] = CRGB(red, blue, green);
                }
            }
            FastLED.show();
        } else if (subtip == 4) {
            int pas = NUM_LEDS / sectiuni;
            for (int i = pas / 2; i < NUM_LEDS; i += pas) {
                leduri_argb[i] = CRGB(culoare.r, culoare.b, culoare.g);
            }
            FastLED.show();
        } else if (subtip == 5) {
            int pas = NUM_LEDS / sectiuni;
            for (int i = 0; i < NUM_LEDS; i += pas) {
                leduri_argb[i] = CRGB(culoare.r, culoare.b, culoare.g);
            }
            FastLED.show();
        } else if (subtip == 6) {
            int pas = NUM_LEDS / sectiuni;
            for (int i = NUM_LEDS - 1; i >= 0; i -= pas) {
                leduri_argb[i] = CRGB(culoare.r, culoare.b, culoare.g);
            }
            FastLED.show();
        }
        //</editor-fold>

        //</editor-fold>
    }

    void looper() {
        //<editor-fold desc="body" defaultstate="collapsed">
        if (animationCounter % (LED_UPDATE_FREQUENCY / animationSpeed) == 0) {
            //<editor-fold desc="animatie basic aprindere">
            if (subtip == 1) {
                for (int i = 0; i < NUM_LEDS; i++) {
                    short int r = leduri_argb[i].red;
                    short int g = leduri_argb[i].blue;
                    short int b = leduri_argb[i].green;
                    r += increment;
                    if (r > culoare.r) {
                        r = culoare.r;
                    }
                    g += increment;
                    if (g > culoare.g) {
                        g = culoare.g;
                    }
                    b += increment;
                    if (b > culoare.b) {
                        b = culoare.b;
                    }
                    leduri_argb[i] = CRGB(r, b, g);
                }
                bool done = true;
                for (int i = 0; i < NUM_LEDS; i++) {
                    if (culoare.r != leduri_argb[i].r || culoare.g != leduri_argb[i].b
                            || culoare.b != leduri_argb[i].g) {
                        done = false;
                    }
                }
                if (done) {
                    animationFinished();
                }
                FastLED.show();
            }//</editor-fold>
                //<editor-fold desc="infloreste 1" defaultstate="collapsed">
            else if (subtip == 2) {
                short int i = NUM_LEDS / 2;
                leduri_argb[i] = CRGB(culoare.r,
                        culoare.b,
                        culoare.g);
                bool doneRight = false, doneLeft = false;
                while (true) {
                    i++;
                    if (i >= NUM_LEDS) {
                        doneRight = true;
                        break;
                    }
                    if (leduri_argb[i].r == culoare.r &&
                            leduri_argb[i].g == culoare.b &&
                            leduri_argb[i].b == culoare.g) {
                    } else {
                        leduri_argb[i].r = culoare.r;
                        leduri_argb[i].g = culoare.b;
                        leduri_argb[i].b = culoare.g;
                        break;
                    }
                }
                i = NUM_LEDS / 2;
                while (true) {
                    i--;
                    if (i < 0) {
                        doneLeft = true;
                        break;
                    }
                    if (leduri_argb[i].r == culoare.r &&
                            leduri_argb[i].g == culoare.b &&
                            leduri_argb[i].b == culoare.g) {
                    } else {
                        leduri_argb[i].r = culoare.r;
                        leduri_argb[i].g = culoare.b;
                        leduri_argb[i].b = culoare.g;
                        break;
                    }
                }
                FastLED.show();
                if (doneLeft && doneRight) {
                    selectedAnimation_ARGB = ' ';
                    if (NORMAL_PRINTS_ENABLED) {
                        Serial.println("Am oprit animatia");
                    }
                }
            }//</editor-fold>
                //<editor-fold desc="infloreste 2" defaultstate="collapsed">
            else if (subtip == 3) {
                for (short int i = NUM_LEDS - 1; i > NUM_LEDS / 2; i--) {
                    brightness[i] = brightness[i - 1];
                }
                for (short int i = 0; i < NUM_LEDS / 2; i++) {
                    brightness[i] = brightness[i + 1];
                }
                short int toateColorate = 0;
                for (short int i = 0; i < NUM_LEDS; i++) {
                    double amount = brightness[i];
                    if (amount == 1.0) {
                        toateColorate++;
                    }
                    if (amount > 0.0) {
                        short int red = culoare.r;
                        short int green = culoare.g;
                        short int blue = culoare.b;
                        red = red*amount;
                        green = green*amount;
                        blue = blue*amount;
                        leduri_argb[i] = CRGB(red, blue, green);
                    }
                }
                if (toateColorate == NUM_LEDS) {
                    animationFinished();
                }
                FastLED.show();
            }//</editor-fold>
                //<editor-fold desc="infloreste 3" defaultstate="collapsed">
            else if (subtip == 4) {
                int pas = NUM_LEDS / sectiuni;
                for (int i = 1; i < NUM_LEDS - 1; i++) {
                    bool culoareBuna = leduri_argb[i].red == culoare.r &&
                            leduri_argb[i].blue == culoare.g &&
                            leduri_argb[i].green == culoare.b;
                    if (culoareBuna) {
                        leduri_argb[i - 1] = CRGB(culoare.r, culoare.b, culoare.g);
                        while (i < NUM_LEDS/*iesire de urgenta*/) {
                            i++;
                            culoareBuna = leduri_argb[i].red == culoare.r &&
                                    leduri_argb[i].blue == culoare.g &&
                                    leduri_argb[i].green == culoare.b;
                            if (!culoareBuna) {
                                break;
                            }
                        }
                        leduri_argb[i++] = CRGB(culoare.r, culoare.b, culoare.g);
                    }
                }
                bool done = true;
                for (int i = 0; i < NUM_LEDS; i++) {
                    bool culoareBuna = leduri_argb[i].red == culoare.r &&
                            leduri_argb[i].blue == culoare.g &&
                            leduri_argb[i].green == culoare.b;
                    if (!culoareBuna) {
                        done = false;
                        break;
                    }
                }
                if (done) {
                    animationFinished();
                }
                FastLED.show();
            }//</editor-fold>
                //<editor-fold desc="infloreste in dreapta" defaultstate="collapsed">
            else if (subtip == 5) {

                for (int i = 1; i < NUM_LEDS; i++) {
                    if (leduri_argb[i - 1].red == culoare.r &&
                            leduri_argb[i - 1].blue == culoare.g &&
                            leduri_argb[i - 1].green == culoare.b &&
                            (leduri_argb[i].red != culoare.r ||
                            leduri_argb[i].blue != culoare.g ||
                            leduri_argb[i].green != culoare.b)
                            ) {
                        leduri_argb[i] = CRGB(culoare.r, culoare.b, culoare.g);
                        i++;
                    }
                }
                bool done = true;
                for (int i = 0; i < NUM_LEDS; i++) {
                    if (leduri_argb[i].red != culoare.r ||
                            leduri_argb[i].blue != culoare.g ||
                            leduri_argb[i].green != culoare.b) {
                        done = false;
                        break;
                    }
                }
                if (done) {
                    animationFinished();
                }
                FastLED.show();
            }//</editor-fold>
                //<editor-fold desc="infloreste in stanga" defaultstate="collapsed">
            else if (subtip == 6) {
                for (int i = NUM_LEDS - 2; i >= 0; i--) {
                    if (leduri_argb[i + 1].red == culoare.r &&
                            leduri_argb[i + 1].blue == culoare.g &&
                            leduri_argb[i + 1].green == culoare.b &&
                            (leduri_argb[i].red != culoare.r ||
                            leduri_argb[i].blue != culoare.g ||
                            leduri_argb[i].green != culoare.b)
                            ) {
                        leduri_argb[i] = CRGB(culoare.r, culoare.b, culoare.g);
                        i--;
                    }
                }
                bool done = true;
                for (int i = 0; i < NUM_LEDS; i++) {
                    if (leduri_argb[i].red != culoare.r ||
                            leduri_argb[i].blue != culoare.g ||
                            leduri_argb[i].green != culoare.b) {
                        done = false;
                        break;
                    }
                }
                if (done) {
                    animationFinished();
                }
                FastLED.show();
            }
            //</editor-fold>
        }
        //</editor-fold>
    }

} lightOnAnimation(LED_UPDATE_FREQUENCY, 10, 3, Color(255, 0, 205), 4);

class BlendingAnimation {
public:
    int animationSpeed;
    int subtip;
    uint8_t brightness;
    CRGBPalette16 currentPalette;
    TBlendType currentBlending;

    BlendingAnimation(int animationSpeed, int subtip, uint8_t brightness) {
        this->animationSpeed = animationSpeed;
        this->subtip = subtip;
        this->brightness = brightness;
    }
    //<editor-fold desc="functii" defaultstate="collapsed">

    void SetupTotallyRandomPalette() {
        for (int i = 0; i < 16; i++) {
            currentPalette[i] = CHSV(random8(), 255, random8());
        }
    }

    void FillLEDsFromPaletteColors(uint8_t colorIndex) {
        for (int i = 0; i < NUM_LEDS; i++) {
            leduri_argb[i] = ColorFromPalette(currentPalette,
                    colorIndex,
                    brightness,
                    currentBlending);
            colorIndex += 3;
        }
    }

    void SetupBlackAndWhiteStripedPalette() {
        //<editor-fold desc="body" defaultstate="collapsed">
        fill_solid(currentPalette, 16, CRGB::Black);
        currentPalette[0] = CRGB::White;
        currentPalette[4] = CRGB::White;
        currentPalette[8] = CRGB::White;
        currentPalette[12] = CRGB::White;
        //</editor-fold>
    }

    void SetupPurpleAndGreenPalette() {
        //<editor-fold desc="body" defaultstate="collapsed">
        CRGB purple = CHSV(HUE_PURPLE, 255, 255);
        CRGB green = CHSV(HUE_GREEN, 255, 255);
        CRGB black = CRGB::Black;
        currentPalette = CRGBPalette16(
                green, green, black, black,
                purple, purple, black, black,
                green, green, black, black,
                purple, purple, black, black);
        //</editor-fold>
    }
    //</editor-fold>

    void init() {
        //<editor-fold desc="body" defaultstate="collapsed">

        //<editor-fold desc="Parsing de date" defaultstate="collapsed">
        byte suma = inputBytes[0];
        for (int i = 1; i < 5; i++) {
            suma += inputBytes[i];
        }
        if (suma != inputBytes[5]) {
            Serial.println(err);
            return;
        }

        this->animationSpeed = map(getColor(inputBytes[2]), 0, 255, 0, LED_UPDATE_FREQUENCY);
        this->subtip = getColor(inputBytes[3]);
        this->brightness = getColor(inputBytes[4]);

        selectedAnimation_ARGB = ANIMATION_ARGB_BLENDING;
        
        //</editor-fold>

        //<editor-fold desc="Alte initializari" defaultstate="collapsed">
        switch (subtip) {
            case 1:
                currentPalette = RainbowColors_p;
                currentBlending = LINEARBLEND;
                break;
            case 2:
                currentPalette = RainbowStripeColors_p;
                currentBlending = NOBLEND;
                break;
            case 3:
                currentPalette = RainbowStripeColors_p;
                currentBlending = LINEARBLEND;
                break;
            case 4:
                SetupPurpleAndGreenPalette();
                currentBlending = LINEARBLEND;
                break;
            case 5:
                SetupTotallyRandomPalette();
                currentBlending = LINEARBLEND;
                break;
            case 6:
                SetupBlackAndWhiteStripedPalette();
                currentBlending = NOBLEND;
                break;
            case 7:
                SetupBlackAndWhiteStripedPalette();
                currentBlending = LINEARBLEND;
                break;
            case 8:
                currentPalette = CloudColors_p;
                currentBlending = LINEARBLEND;
                break;
            case 9:
                currentPalette = PartyColors_p;
                currentBlending = LINEARBLEND;
                break;
        }
        //</editor-fold>

        //</editor-fold>
    }

    void looper() {
        //<editor-fold desc="body" defaultstate="collapsed">
        if (animationCounter % (LED_UPDATE_FREQUENCY / animationSpeed) == 0) {
            FastLED.show();
            static uint8_t startIndex = 0;
            startIndex = startIndex + 1;
            FillLEDsFromPaletteColors(startIndex);
        }
        //</editor-fold>
    }

} blendingAnimation(LED_UPDATE_FREQUENCY, 1, 255);

class FaderAnimation {
public:
    double angle = 0;
    int BASE_INCREMENT = 255;


    int subtip;
    double increment;
    int animationSpeed;
    Color color;

    int min = 0, max = 255;
    int sectiuni, dimensiune;

    FaderAnimation(int subtip, double increment, int animationSpeed, Color color) {
        this->subtip = subtip;
        this->increment = increment;
        this->animationSpeed = animationSpeed;
        this->color = color;
    }

    /*
    FaderAnimation(int n, ...) {
        va_list listaArgumente;
        va_start(listaArgumente, n);
        this->subtip = va_arg(listaArgumente, int);
        if (subtip == 0) {
            this->increment = va_arg(listaArgumente, double);
            this->animationSpeed = va_arg(listaArgumente, int);
            this->color = color;
        }
        va_end(listaArgumente);
    }
     */

    void init(int subtip) {
        //<editor-fold desc="body" defaultstate="collapsed">
        byte suma;
        if (subtip == 0) {
            int dimensiune = 11;
            suma = inputBytes[0];
            for (int i = 1; i < dimensiune - 1; i++) {
                suma += inputBytes[i];
            }
            if (suma != inputBytes[dimensiune - 1]) {
                Serial.println(err);
                return;
            }
            this->subtip = subtip;
            this->increment =
                    (double) getColor(inputBytes[3]) / this->BASE_INCREMENT;
            this->animationSpeed = getColor(inputBytes[4]);
            this->color = Color(getColor(inputBytes[5]),
                    getColor(inputBytes[6]),
                    getColor(inputBytes[7]));
            this->min = getColor(inputBytes[8]);
            this->max = getColor(inputBytes[9]);
            this->angle = 1.5 * PI;
            selectedAnimation_ARGB = ANIMATION_ARGB_FADER;
        } else if (subtip == 1) {
            int dimensiune = 9;
            suma = inputBytes[0];
            for (int i = 1; i < dimensiune - 1; i++) {
                suma += inputBytes[i];
            }
            if (suma != inputBytes[dimensiune - 1]) {
                Serial.println(err);
                return;
            }
            this->subtip = subtip;
            this->animationSpeed = getColor(inputBytes[3]);
            this->color = Color(
                    getColor(inputBytes[4]),
                    getColor(inputBytes[5]),
                    getColor(inputBytes[6])
                    );

            this->dimensiune = getColor(inputBytes[7]);

            this->increment = 0.62/*831*/;
            this->angle = 0;
            for (int i = 0; i < NUM_LEDS; i++) {
                leduri_argb[i] = CRGB::Black;
            }

            selectedAnimation_ARGB = ANIMATION_ARGB_FADER;

        }
        //</editor-fold>
    }

    void looper() {
        //<editor-fold desc="body" defaultstate="body">
        if (animationCounter % (LED_UPDATE_FREQUENCY / animationSpeed) == 0) {
            if (subtip == 0) {
                double multi = mapd(sin(angle), -1, 1, 0, 1);
                multi = mapd(multi,
                        0,
                        1,
                        mapd(min, 0, 255, 0, 1),
                        mapd(max, 0, 255, 0, 1));
                angle += increment;
                for (int i = 0; i < NUM_LEDS; i++) {
                    leduri_argb[i] = CRGB(
                            (int) color.r*multi,
                            (int) color.b*multi,
                            (int) color.g * multi);
                }
                FastLED.show();
            } else if (subtip == 1) {
                double direction = sin(angle);
                int n = NUM_LEDS;
                int i = n / 2;

                if (direction > 0) {
                    int j = i;
                    while (leduri_argb[j].r == color.r &&
                            leduri_argb[j].b == color.g &&
                            leduri_argb[j].g == color.b
                            && j - i <= dimensiune / 2) {
                        j++;
                    }

                    if (j < n) {
                        leduri_argb[j] = CRGB(color.r, color.b, color.g);
                    }

                    if (n - j - 1 >= 0) {
                        leduri_argb[n - j - 1 ] = CRGB(color.r, color.b, color.g);
                    }


                } else if (direction < 0) {
                    int j = i;
                    while ((leduri_argb[j].r == color.r &&
                            leduri_argb[j].b == color.g &&
                            leduri_argb[j].g == color.b)
                            && j - i <= dimensiune / 2) {
                        j++;
                    }
                    if (j - 1 >= 0) {
                        leduri_argb[j - 1] = CRGB::Black;
                    }
                    if (n - j >= 0) {
                        leduri_argb[n - j ] = CRGB::Black;
                    }


                }
                leduri_argb[i] = CRGB(color.r, color.b, color.g);
                if (angle + 6.28 / dimensiune > 6.28) {
                    angle = 0;
                }
                angle += 6.28 / dimensiune;
                FastLED.show();
            }
        }
        //</editor-fold>
    }

} faderAnimation(0, 0.05, LED_UPDATE_FREQUENCY, Color(255, 0, 0));


//</editor-fold>

void initLEDs() {
    //<editor-fold desc="body" defaultstate="collapsed">
    FastLED.addLeds < WS2811, ARGB_PIN, GRB > (leduri_argb, NUM_LEDS);
    for (int i = 0; i < NUM_LEDS; i++) {
        leduri_argb[i] = CRGB::Black;
    }
    FastLED.show();

    //</editor-fold>
}

void updateSelectedAnimation(char selectedAnimation, char data_arr[], int n) {
    //<editor-fold desc="body" defaultstate="collapsed">
    byte lightOnRed = 0, lightOnGreen = 0, lightOnBlue = 0;
    animationCounter = 0;
    int i;
    byte suma;

    switch (selectedAnimation) {
        case ANIMATION_ARGB_ROMANIAN_1:
            romanianFlagAnimation.init();
            break;
        case ANIMATION_ARGB_BLENDING:
            blendingAnimation.init();
            break;
        case ANIMATION_ARGB_LIGHT_OFF:
            lightOffAnimation.init();
            break;
        case ANIMATION_ARGB_LIGHT_ON:
            lightOnAnimation.init();
            break;
        case ANIMATION_ARGB_FADER:
            faderAnimation.init(getColor(inputBytes[2]));
            break;
        default:
            selectedAnimation_ARGB = ' ';
            lightOnRed = getColor(inputBytes[1]);
            lightOnGreen = getColor(inputBytes[2]);
            lightOnBlue = getColor(inputBytes[3]);
            suma = inputBytes[0];

            for (i = 1; i < 4; i++) {
                suma += inputBytes[i];
            }

            if (suma == inputBytes[4]) {
                for (i = 0; i < NUM_LEDS; i++) {
                    leduri_argb[i] = CRGB(lightOnRed, lightOnBlue, lightOnGreen);
                }
                FastLED.show();
            } else {
                Serial.println(err);
            }
            break;
    }
    if (selectedAnimation == ' ') {
        selectedAnimation_ARGB = ' ';
    }
    //</editor-fold>
}

void animationFinished() {
    selectedAnimation_ARGB = ' ';
    if (NORMAL_PRINTS_ENABLED) {
        Serial.println("Am oprit animatia");
    }
}

void ARGBLooper() {
    switch (selectedAnimation_ARGB) {
        case ANIMATION_ARGB_ROMANIAN_1:
            romanianFlagAnimation.looper();
            break;
        case ANIMATION_ARGB_BLENDING:
            blendingAnimation.looper();
            break;
        case ANIMATION_ARGB_LIGHT_OFF:
            lightOffAnimation.looper();
            break;
        case ANIMATION_ARGB_LIGHT_ON:
            lightOnAnimation.looper();
            break;
        case ANIMATION_ARGB_FADER:
            faderAnimation.looper();
            break;
    }
    animationCounter++;
}