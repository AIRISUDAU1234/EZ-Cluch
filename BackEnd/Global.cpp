#include "Global.h"
#include <string>

int CrossRadius = 20;

// �O���[�o���ϐ��̒�`�Ə�����
WinRTScreenCapture capture;

float MouseSensitivity = 0.40f;
int AimSpeed = 7; 

const float constant = 0.116f;
float modifier = 0.0f;

bool suspendThreads = false;
int threads = 1;
int screenWidth = GetSystemMetrics(SM_CXSCREEN); 
int screenHeight = GetSystemMetrics(SM_CYSCREEN); 
int centreScreenX, centreScreenY;

bool Radius = false;
bool Triggerbot = false;
bool AimAssist = false;
bool outlawmode = false;
bool marshallmode = false;
bool operatormode = false;
bool vandalmode1 = false;
bool vandalmode2 = false;
bool pistolmode = false;
bool sheriffmode = false;

bool head = false;
bool body = false;

bool RED = false;
bool PURPLE = false;
bool YELLOW = false;

int shadeVariation = 28; //�F�̍�

int erode_iterations = 1; 
int dilate_iterations = 1; 