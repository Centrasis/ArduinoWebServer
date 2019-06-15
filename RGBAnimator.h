typedef enum AnimationMode {
  amFadeUp,//only fade up and jump to down
  amFadeDown,//only fade downp and jump to up
  amFadeSmooth//fade up and down
} AnimationMode;

class RGBAnimator
{
protected:
    int pins[3];
    unsigned long oldTime = 0;
    unsigned long currentTime = 0;
    const unsigned long minDeltaTime = 10;
    AnimationMode animMode = AnimationMode::amFadeUp;
    float stepsPerSecond = 20;
    char currentColor[3];
    char targetColor[3];

    virtual void onTimer(float deltaSeconds);

    float calcColorDistance(char* color1, char* color2);

public:
    RGBAnimator(const int* pins);
    void setColor(int r, int g, int b);
    void fade2Color(int r, int g, int b, float timeInSeconds);
    void setSpeed(float stepsPerSecond);

    void update();
};