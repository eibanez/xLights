/***************************************************************
 * This source files comes from the xLights project
 * https://www.xlights.org
 * https://github.com/smeighan/xLights
 * See the github commit history for a record of contributing
 * developers.
 * Copyright claimed based on commit dates recorded in Github
 * License: https://github.com/smeighan/xLights/blob/master/License.txt
 **************************************************************/

#include "LightningEffect.h"
#include "LightningPanel.h"

#include "../sequencer/Effect.h"
#include "../RenderBuffer.h"
#include "../UtilClasses.h"

#include "../../include/lightning-16.xpm"
#include "../../include/lightning-24.xpm"
#include "../../include/lightning-32.xpm"
#include "../../include/lightning-48.xpm"
#include "../../include/lightning-64.xpm"

LightningEffect::LightningEffect(int id) : RenderableEffect(id, "Lightning", lightning_16, lightning_24, lightning_32, lightning_48, lightning_64)
{
    //ctor
}

LightningEffect::~LightningEffect()
{
    //dtor
}

xlEffectPanel *LightningEffect::CreatePanel(wxWindow *parent) {
    return new LightningPanel(parent);
}

#define DOWN 0
#define UP 1
#define RIGHT 2
#define LEFT 3

void LightningEffect::SetDefaultParameters() {
    LightningPanel *lp = (LightningPanel*)panel;
    if (lp == nullptr) {
        return;
    }

    lp->BitmapButton_Lightning_TopXVC->SetActive(false);
    lp->BitmapButton_Lightning_TopYVC->SetActive(false);
    lp->BitmapButton_Number_BoltsVC->SetActive(false);
    lp->BitmapButton_Number_SegmentsVC->SetActive(false);

    SetSliderValue(lp->Slider_Number_Bolts, 10);
    SetSliderValue(lp->Slider_Number_Segments, 5);
    SetSliderValue(lp->Slider_Lightning_TopX, 0);
    SetSliderValue(lp->Slider_Lightning_TopY, 0);
    SetSliderValue(lp->Slider_Lightning_BOTX, 0);
    SetSliderValue(lp->Slider_Lightning_BOTY, 0);

    SetChoiceValue(lp->CHOICE_Lightning_Direction, "Up");

    SetCheckBoxValue(lp->CheckBox_ForkedLightning, false);
}

static inline int GetLightningEffect(const std::string &dir) {
    if (dir == "Down") {
        return DOWN;
    } else if (dir == "Up") {
        return UP;
    } else if (dir == "Left") {
        return LEFT;
    } else if (dir == "Right") {
        return RIGHT;
    } else {
        return UP;
    }
}

void LightningEffect::Render(Effect *effect, const SettingsMap &SettingsMap, RenderBuffer &buffer) {
    float oset = buffer.GetEffectTimeIntervalPosition();
    int Number_Bolts = GetValueCurveInt("Number_Bolts", 10, SettingsMap, oset, LIGHTNING_BOLTS_MIN, LIGHTNING_BOLTS_MAX, buffer.GetStartTimeMS(), buffer.GetEndTimeMS());
    int Number_Segments = GetValueCurveInt("Number_Segments", 5, SettingsMap, oset, LIGHTNING_SEGMENTS_MIN, LIGHTNING_SEGMENTS_MAX, buffer.GetStartTimeMS(), buffer.GetEndTimeMS());
    bool ForkedLightning = SettingsMap.GetBool("CHECKBOX_ForkedLightning", false);
    int topX = GetValueCurveInt("Lightning_TopX", 0, SettingsMap, oset, LIGHTNING_TOPX_MIN, LIGHTNING_TOPX_MAX, buffer.GetStartTimeMS(), buffer.GetEndTimeMS());
    int topY = GetValueCurveInt("Lightning_TopY", 0, SettingsMap, oset, LIGHTNING_TOPY_MIN, LIGHTNING_TOPY_MAX, buffer.GetStartTimeMS(), buffer.GetEndTimeMS());
    int botX = SettingsMap.GetInt("SLIDER_Lightning_BOTX", 0);
    //int botY = SettingsMap.GetInt("SLIDER_Lightning_BOTY", 0);
    int DIRECTION = GetLightningEffect(SettingsMap["CHOICE_Lightning_Direction"]);

    if (Number_Bolts == 0) {
        Number_Bolts = 1;
    }
    
    if (Number_Segments == 0) {
        Number_Segments = 1;
    }

    int curState = (buffer.curPeriod - buffer.curEffStartPer);
    int xc = buffer.BufferWi / 2;

    int StepSegment = buffer.BufferHt / Number_Bolts;
    int segment = curState%Number_Bolts;
    segment *= 4;
    if (segment>Number_Bolts) segment = Number_Bolts;
    if (curState>Number_Bolts) segment = Number_Bolts;
    xlColor color;
    buffer.palette.GetColor(0, color);
    int x1 = 0;
    int y1 = 0;
    if (DIRECTION == DOWN) {
        x1 = xc + topX;
        y1 = buffer.BufferHt - topY;
    } else if(DIRECTION==UP) {
        x1 = xc + topX;
        y1 = topY; 
    }

    int xoffset = curState * botX / 10.0;
    for(int i = 0; i <= segment; i++) {
        int j = rand() + 1;
        int x2 = 0;
        int y2 = 0;
        if(DIRECTION==UP || DIRECTION==DOWN) {
            if(i % 2 == 0) { // Every even segment will alternate direction
                if (rand() % 2 == 0) // target x is to the left
                    x2 = xc + topX - (j % Number_Segments);
                else // but randomely we reverse direction, also make it a larger jag
                    x2 = xc + topX + (2 * (j % Number_Segments));
            } else { // odd segments will
                if (rand() % 2 == 0) // move to the right
                    x2 = xc + topX + (j % Number_Segments);
                else // but sometimes move 3 units to left.
                    x2 = xc + topX - (3 * (j % Number_Segments));
            }
            if (DIRECTION==DOWN)
                y2 = buffer.BufferHt - (i * StepSegment) - topY;
            else if (DIRECTION == UP)
                y2 = (i * StepSegment) + topY;
        }
        else if (DIRECTION == LEFT || DIRECTION == RIGHT) {

        }

        LightningDrawBolt(buffer, x1 + xoffset, y1, x2 + xoffset, y2, color, curState);

        if (ForkedLightning) {
            if (i > (segment / 2)) {
                int x3 = 0;
                if (i % 2 == 1) {
                    if (rand()%2==1)
                        x3 = xc + topX - (j % Number_Segments);
                    else  x3 = xc + topX + (2 * (j % Number_Segments));
                } else {
                    if (rand() % 2 == 1)
                        x3 = xc + topX + (j % Number_Segments);
                    else
                        x3 = xc + topX - (3 * (j % Number_Segments));
                }
                LightningDrawBolt(buffer, x1 + xoffset, y1, x3 + xoffset, y2, color, curState);
            }
        }
        x1 = x2;
        y1 = y2;
    }
}

void LightningEffect::LightningDrawBolt(RenderBuffer &buffer, const int x0_, const int y0_, const int x1_, const int y1_,  xlColor& color ,int curState) {
    int x0 = x0_;
    int x1 = x1_;
    int y0 = y0_;
    int y1 = y1_;
    xlColor color2;
    HSVValue hsv = color.asHSV();
    HSVValue hsv2 = color2.asHSV();
    //   if(x0<0 || x1<0 || y1<0 || y0<0) return;

    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = (dx>dy ? dx : -dy)/2, e2;
    color2=color;
    // color2.red=color2.green=color2.blue=200;
    //   int frame_startfade = 2*20; // 2 seconds full brightness
    //   int frame_fadedone = 5*20; // 3 seconds to fade o

    color = hsv;

    hsv2.value = hsv.value * 0.90;
    color2 = hsv2;
    color2 = hsv;
    for(;;) {
        buffer.SetPixel(x0,y0, color);
        buffer.SetPixel(x0-1,y0, color2);
        buffer.SetPixel(x0+1,y0, color2);
        if (x0==x1 && y0==y1) break;
        e2 = err;
        if (e2 >-dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }
}
