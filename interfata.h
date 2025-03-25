/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  FREQ_PANEL                       1       /* callback function: OnFrequencyPanel */
#define  FREQ_PANEL_GRAPH_RAW_DATA        2       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_WIND_GRAPH            3       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_FILT_WIND_GRAPH       4       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_FILT_GRAPH            5       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_INTERV_GRAPH          6       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_SPECTRUM_FILT_GRAPH   7       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_INTERV_SPECTRUM_GRAPH 8       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_IDC_NSAMPLES          9       /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_PANEL_SWITCH          10      /* control type: binary, callback function: OnSwitchPanelCB */
#define  FREQ_PANEL_POWER_PEAK1           11      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_POWER_PEAK            12      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_NEXT_BUTTON           13      /* control type: command, callback function: OnInterval */
#define  FREQ_PANEL_FREQ_PEAK1            14      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_PREV_BUTTON           15      /* control type: command, callback function: OnInterval */
#define  FREQ_PANEL_FREQ_PEAK             16      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_SPECTRUM_BUTTON       17      /* control type: command, callback function: OnSpectruButton */
#define  FREQ_PANEL_STOP                  18      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_INTERVAL              19      /* control type: textMsg, callback function: (none) */
#define  FREQ_PANEL_START                 20      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_WINDOW_TYPE           21      /* control type: ring, callback function: (none) */
#define  FREQ_PANEL_FILTER_BUTTON         22      /* control type: command, callback function: OnFilterButton */
#define  FREQ_PANEL_FILTER_TYPE           23      /* control type: ring, callback function: (none) */
#define  FREQ_PANEL_ORDER                 24      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_CUT_FREQ              25      /* control type: numeric, callback function: (none) */

#define  MAIN_PANEL                       2       /* callback function: OnMainPanel */
#define  MAIN_PANEL_LOAD_BUTTON           2       /* control type: command, callback function: OnLoadButtonCB */
#define  MAIN_PANEL_GRAPH_RAW_DATA_INTERV 3       /* control type: graph, callback function: (none) */
#define  MAIN_PANEL_IDC_GRAPH_RAW_DATA    4       /* control type: graph, callback function: (none) */
#define  MAIN_PANEL_IDC_GRAPH_ENVELOPE    5       /* control type: graph, callback function: (none) */
#define  MAIN_PANEL_IDC_GRAPH_FILT_DATA   6       /* control type: graph, callback function: (none) */
#define  MAIN_PANEL_APLICA_BUTTON         7       /* control type: command, callback function: OnAplicaButton */
#define  MAIN_PANEL_NEXT_BUTTON           8       /* control type: command, callback function: OnNextButton */
#define  MAIN_PANEL_PREV_BUTTON           9       /* control type: command, callback function: OnPrevButton */
#define  MAIN_PANEL_FILTER_TYPE           10      /* control type: ring, callback function: (none) */
#define  MAIN_PANEL_IDC_NUM_MAX_VAL       11      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_IDC_NUM_MEDIAN        12      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_IDC_NUM_MEAN          13      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_IDC_NUM_MAX_INDEX     14      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_IDC_NUM_MIN_INDEX     15      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_IDC_NUM_MIN_VAL       16      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_DECORATION            17      /* control type: deco, callback function: (none) */
#define  MAIN_PANEL_IDC_NUM_ZERO_CROSSING 18      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_IDC_NUM_VARIANCE      19      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_HISTOGRAM_GRAPH       20      /* control type: graph, callback function: (none) */
#define  MAIN_PANEL_DECORATION_3          21      /* control type: deco, callback function: (none) */
#define  MAIN_PANEL_HISTOGRAM_BUTTON      22      /* control type: command, callback function: OnHistogramButton */
#define  MAIN_PANEL_ZERO_CROSSING_BUTTON  23      /* control type: command, callback function: OnZeroCrossingButton */
#define  MAIN_PANEL_MEDIAN_BUTTON         24      /* control type: command, callback function: OnMedianButton */
#define  MAIN_PANEL_VARIANCE_BUTTON       25      /* control type: command, callback function: OnVarianceButton */
#define  MAIN_PANEL_MEAN_BUTTON           26      /* control type: command, callback function: OnMeanButton */
#define  MAIN_PANEL_MIN_MAX_BUTTON        27      /* control type: command, callback function: OnMinMaxButton */
#define  MAIN_PANEL_ALPHA_NUMERIC         28      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_IDC_WINDOW_SIZE       29      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_IDC_STOP              30      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_INTERVAL              31      /* control type: textMsg, callback function: (none) */
#define  MAIN_PANEL_Filtrare              32      /* control type: textMsg, callback function: (none) */
#define  MAIN_PANEL_DECORATION_5          33      /* control type: deco, callback function: (none) */
#define  MAIN_PANEL_IDC_START             34      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_MINIM                 35      /* control type: textMsg, callback function: (none) */
#define  MAIN_PANEL_MAXIM                 36      /* control type: textMsg, callback function: (none) */
#define  MAIN_PANEL_SWENESS_BUTTON        37      /* control type: command, callback function: OnSkewnessButton */
#define  MAIN_PANEL_KURTOSIS_BUTTON       38      /* control type: command, callback function: OnKurtosisButton */
#define  MAIN_PANEL_IDC_NUM_KURTOSIS      39      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_IDC_ANVELOPA_BUTTON   40      /* control type: command, callback function: OnAnvelopaButton */
#define  MAIN_PANEL_IDC_NUM_SKEWNESS      41      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_PANEL_SWITCH          42      /* control type: binary, callback function: OnSwitchPanelCB */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnAnvelopaButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnAplicaButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnFilterButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnFrequencyPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnHistogramButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnInterval(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnKurtosisButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLoadButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMainPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMeanButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMedianButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMinMaxButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnNextButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPrevButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSkewnessButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSpectruButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSwitchPanelCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnVarianceButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnZeroCrossingButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif