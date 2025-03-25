#include <advanlys.h>
#include <utility.h>
#include <formatio.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "interfata.h"
#include "toolbox.h"

//________________________________________________________________________________
// Constants
#define SAMPLE_RATE		0
#define NPOINTS			1


//________________________________________________________________________________
// Global variables
static int mainPanel = 0;
static int freqPanel = 0;

int waveInfo[2]; //waveInfo[0] = sampleRate
				 //waveInfo[1] = number of elements
double sampleRate = 0.0;  // frecventa
int npoints = 0;  // nr de elemente
int lastNPoints = 0 ;

// Daca fisierul este mai mare de 10 secunde, se retin primele sau ultimele 6 secunde.
double* waveDataTotal = 0;
double *waveData = 0; //  vectorul care contine valorile semnalului audio


// variabile pentru min, max si indicii lor
double minValue, maxValue;
int minIndex, maxIndex;
double mean;  // media
double variance;  // dispersia

int intervalStop;
int intervalStart;

double skewness;
double kurtosis;

int filterType;

// etapa 2
int samplesPerChannel = 1024;
double* convertedSpectrum = 0;
double freqPeak, powerPeak, df;

int startInt, stopInt;

double* filtSignal = 0;
double* filtWindSignal = 0;

double* convertedSpectrum1 = 0;
double freqPeak1, powerPeak1, df1;

int main (int argc, char *argv[])
{
	int error = 0;
	
	nullChk (InitCVIRTE (0, argv, 0));
	errChk(mainPanel = LoadPanel(0, "interfata.uir", MAIN_PANEL));
	errChk(freqPanel = LoadPanel(0, "interfata.uir", FREQ_PANEL));
	
	errChk (DisplayPanel (mainPanel));
	 errChk (RunUserInterface ());
	
Error:
	if (mainPanel > 0)
		DiscardPanel(mainPanel);
	return 0;
	/*
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;
	if ((mainPanel = LoadPanel (0, "interfata.uir", MAIN_PANEL)) < 0)
		return -1;
	
	if ((freqPanel = LoadPanel (0, "interfata.uir", FREQ_PANEL)) < 0)
		return -1;
	
	DisplayPanel (mainPanel);
	RunUserInterface ();
	if (mainPanel > 0)
		DiscardPanel (mainPanel);
	return 0;
	*/
}

int CVICALLBACK OnMainPanel (int panel, int event, void *callbackData,
							 int eventData1, int eventData2)
{
	GetCtrlVal(mainPanel, MAIN_PANEL_FILTER_TYPE, &filterType);
	if (filterType == 1){
		SetCtrlAttribute(mainPanel, MAIN_PANEL_IDC_WINDOW_SIZE, ATTR_DIMMED, 1);
		SetCtrlAttribute(mainPanel, MAIN_PANEL_ALPHA_NUMERIC, ATTR_DIMMED, 0);
	}
	
	else if (filterType == 2){
		SetCtrlAttribute(mainPanel, MAIN_PANEL_IDC_WINDOW_SIZE, ATTR_DIMMED, 1);
		SetCtrlAttribute(mainPanel, MAIN_PANEL_ALPHA_NUMERIC, ATTR_DIMMED, 1);
	}
	
	else if (filterType == 0){
		SetCtrlAttribute(mainPanel, MAIN_PANEL_IDC_WINDOW_SIZE, ATTR_DIMMED, 0);
		SetCtrlAttribute(mainPanel, MAIN_PANEL_ALPHA_NUMERIC, ATTR_DIMMED, 1);
	}	
	
	
	switch (event)
	{
		case EVENT_GOT_FOCUS:
			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			if (waveData){
				free(waveData);
				waveData = NULL;	
			}
			
			if (waveDataTotal){
				free(waveDataTotal);
				waveDataTotal = NULL;
			}
			free(filtSignal);
			QuitUserInterface(0);
			break;
	}
	return 0;
}

int CVICALLBACK OnSwitchPanelCB (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if (panel == mainPanel){
				SetCtrlVal(freqPanel, FREQ_PANEL_PANEL_SWITCH, 1);
				DisplayPanel(freqPanel);
				HidePanel(panel);
			}
			else if (panel == freqPanel){
				SetCtrlVal(mainPanel, MAIN_PANEL_PANEL_SWITCH, 0);
				DisplayPanel(mainPanel);
				HidePanel(panel);
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnLoadButtonCB (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			//executa script python pentru conversia unui fisierului .wav in .txt
			LaunchExecutable("python main.py");
			
			//astept sa fie generate cele doua fisiere (modificati timpul daca este necesar
			Delay(3);
			
			//incarc informatiile privind rata de esantionare si numarul de valori
			FileToArray("wafeInfo.txt", waveInfo, VAL_INTEGER, 2, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			sampleRate = waveInfo[SAMPLE_RATE];  // wveInfo[0] = 22050
			npoints = waveInfo[NPOINTS]; // waveInfo[1] = 1323000
			
			double durata = npoints / sampleRate;  // T = nr de elem / frecventa = 1323000 / 22050 = 60 secunde
			lastNPoints = npoints;
			
			if (durata >= 10.0){
			// retinem doar ultimele 6 secunde
			lastNPoints = (int)(sampleRate * 6); // 22050 * 6 = 132300 = cate valori din waveData sunt in 6 secunde = nr de valori ale semnalului in 6 secunde
			}
			
			
			// daca doresc sa ma folosesc de ultimele 6 secunde, atunci trebuie sa incep de la a 1190700 valoare pana la a 1323000 valoare din waveData
			int startIndex = npoints - lastNPoints; // 1190700
			
			//alocare memorie pentru numarul de puncte
			// waveData = waveData = (double *) calloc(npoints, sizeof(double)); - inainte
			waveData = (double *) calloc(lastNPoints, sizeof(double));
			
			if (waveData == NULL){
				MessagePopup("Error", "Alocarea memoriei pentru waveData a esuat");
				return -1;
			}
			
			// incarca intregul semnal din fisier
			waveDataTotal = (double *) calloc(npoints, sizeof(double));
			
			if (waveDataTotal == NULL){
				MessagePopup("Error", "Alocarea memoriei pentru waveDataTotal a esuat");
				return -1;
			}
			
			//incarcare din fisierul .txt in memorie (vector)
			FileToArray("waveData.txt", waveDataTotal, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			
			// copiaza ultimele 6 secunde in waveData
			memcpy(waveData, waveDataTotal + startIndex, lastNPoints * sizeof(double));
			
			// actualizam npoints pt a ne referi doar la ultimele 6 secunde
			npoints = lastNPoints;
			
			//afisare pe grapf
			PlotY(panel, MAIN_PANEL_IDC_GRAPH_RAW_DATA, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			PlotY(freqPanel, FREQ_PANEL_GRAPH_RAW_DATA, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
			// afisare graf in interval 
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, &intervalStart);
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, &intervalStop);
			
			int interval = intervalStop - intervalStart;
			int number_of_elements = interval * sampleRate;
			
			if (number_of_elements <= 0) {
				MessagePopup("Error", "Invalid interval size.");
				return 0;
			}
			
			PlotY(panel, MAIN_PANEL_GRAPH_RAW_DATA_INTERV, waveData, number_of_elements, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			PlotY(freqPanel, FREQ_PANEL_INTERV_GRAPH, waveData + (int)(startInt*sampleRate), sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			break;
	}
	return 0;
}

int CVICALLBACK OnPrevButton (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			DeleteGraphPlot(panel, MAIN_PANEL_GRAPH_RAW_DATA_INTERV, -1, VAL_DELAYED_DRAW);
			DeleteGraphPlot(panel, MAIN_PANEL_IDC_GRAPH_FILT_DATA, -1, VAL_IMMEDIATE_DRAW);
			DeleteGraphPlot(mainPanel, MAIN_PANEL_HISTOGRAM_GRAPH, -1, VAL_IMMEDIATE_DRAW);
			
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, &intervalStart);
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, &intervalStop);
			
			if (intervalStart == 0 && intervalStop == 1){
				SetCtrlAttribute(mainPanel, MAIN_PANEL_PREV_BUTTON, ATTR_DIMMED, 1);
			}
			else {
				intervalStart -= 1;
				intervalStop -= 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, intervalStart);
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
				
				SetCtrlAttribute(mainPanel, MAIN_PANEL_NEXT_BUTTON, ATTR_DIMMED, 0);
				
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_SKEWNESS, 0.0);
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_KURTOSIS, 0.0);
				
			}
			
			int interval = intervalStop - intervalStart;
			if (interval < 1 ){
				MessagePopup("Error ", "Invalid interval size.");
				intervalStart = 0;
				intervalStop = 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, intervalStart);
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			}
			if (interval > 1){
				intervalStop = intervalStart + 1;
				interval = 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			}
			int number_of_elements = interval * sampleRate;
			
			int startIndex = (int)(intervalStart * sampleRate) ;
			
			if (number_of_elements <= 0) {
				MessagePopup("Error", "Invalid interval size.");
				return 0;
			}
			
			PlotY(panel, MAIN_PANEL_GRAPH_RAW_DATA_INTERV, waveData + startIndex, number_of_elements, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MAX_VAL, 0.0);
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MAX_INDEX, 0);
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MIN_VAL, 0.0);
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MIN_INDEX, 0);
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MEAN, 0.0);
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MEDIAN, 0.0);
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_VARIANCE, 0.0);
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_ZERO_CROSSING, 0);

			break;
	}
	return 0;
}

int CVICALLBACK OnNextButton (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			DeleteGraphPlot(panel, MAIN_PANEL_GRAPH_RAW_DATA_INTERV, -1, VAL_DELAYED_DRAW);
			DeleteGraphPlot(panel, MAIN_PANEL_IDC_GRAPH_FILT_DATA, -1, VAL_IMMEDIATE_DRAW);
			DeleteGraphPlot(mainPanel, MAIN_PANEL_HISTOGRAM_GRAPH, -1, VAL_IMMEDIATE_DRAW);
						
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, &intervalStart);
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, &intervalStop);
			
			if (intervalStart == 5 && intervalStop == 6){
				SetCtrlAttribute(mainPanel, MAIN_PANEL_NEXT_BUTTON, ATTR_DIMMED, 1);
			}
			else{
				intervalStart += 1;
				intervalStop += 1;
				
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, intervalStart);
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			
				SetCtrlAttribute(mainPanel, MAIN_PANEL_PREV_BUTTON, ATTR_DIMMED, 0);
			}
			
			int interval = intervalStop - intervalStart;
			if (interval < 1 ){
				MessagePopup("Error",  "Invalid interval size.");
				intervalStart = 0;
				intervalStop = 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, intervalStart);
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			}
			
			if (interval > 1){
				intervalStop = intervalStart + 1;
				interval = 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			}
			int number_of_elements = interval * sampleRate;
			
			int startIndex = (int)(intervalStart * sampleRate) ;
			
			if (number_of_elements <= 0) {
				MessagePopup("Error", "Invalid interval size.");
				return 0;
			}
						
			PlotY(panel, MAIN_PANEL_GRAPH_RAW_DATA_INTERV, waveData + startIndex, number_of_elements, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
					
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MAX_VAL, 0.0);
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MAX_INDEX, 0);
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MIN_VAL, 0.0);
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MIN_INDEX, 0);			
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MEAN, 0.0);
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MEDIAN, 0.0);
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_VARIANCE, 0.0);
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_ZERO_CROSSING, 0);
			
			break;
	}
	return 0;
}


// functia de callback a butonului pentru determinarea si afisarea minimului si maximului
int CVICALLBACK OnMinMaxButton (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, &intervalStart);
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, &intervalStop);
			
			int interval = intervalStop - intervalStart;
			if (interval < 1 ){
				MessagePopup("Error", "Invalid interval size.");
				intervalStart = 0;
				intervalStop = 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, intervalStart);
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			}
			if (interval > 1){
				intervalStop = intervalStart + 1;
				interval = 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			}
			int number_of_elements = interval * sampleRate;
			
			if (number_of_elements <= 0) {
				MessagePopup("Error", "Invalid interval size.");
				return 0;
			}
			
			/*
			pentru intervalul [0, 1] indicii valorilor din waveData ar trebui sa fie de la 0 la 22049;
			pentru intervalul [1, 2] indicii valorilor din waveData ar trebui sa fie de la 22050 la 44099;
			pentru intervalul [2, 3] indicii valorilor din waveData ar trebui sa fie de la 44100 la 66149;
			pentru intervalul [3, 4] indicii valorilor din waveData ar trebui sa fie de la 66150 la 88199;
			pentru intervalul [4, 5] indicii valorilor din waveData ar trebui sa fie de la 88200 la 110249;
			pentru intervalul [5, 6] indicii valorilor din waveData ar trebui sa fie de la 110250 la 132299
			*/
			
			int startIndex = (int)(intervalStart * sampleRate);  
			int endIndex = (int)(intervalStop * sampleRate) - 1 ;
			
			if (endIndex >= npoints){
				MessagePopup("Error", "Interval out of bounds.");
    			return 0;
			}
						
			MaxMin1D(waveData + startIndex, number_of_elements, &maxValue, &maxIndex, &minValue, &minIndex);
			
			maxIndex += startIndex;
			minIndex += startIndex;
				
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MIN_VAL, minValue); 
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MIN_INDEX, minIndex);			
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MAX_VAL, maxValue);
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MAX_INDEX, maxIndex);
			
			break;
	}
	return 0;
}


// functia de callback a butonului pentru determinarea si afisarea dispersiei
int CVICALLBACK OnVarianceButton (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, &intervalStart);
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, &intervalStop);
			
			int interval = intervalStop - intervalStart;
			if (interval < 1 ){
				MessagePopup("Error", "Invalid interval size.");
				intervalStart = 0;
				intervalStop = 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, intervalStart);
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			}
			if (interval > 1){
				intervalStop = intervalStart + 1;
				interval = 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			}
			int number_of_elements = interval * sampleRate;
			
			int startIndex = (int)(intervalStart * sampleRate);  
						
			if (number_of_elements <= 0) {
				MessagePopup("Error", "Invalid interval size.");
				return 0;
			}
			
			Variance(waveData + startIndex, number_of_elements, &mean, &variance);
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_VARIANCE, variance);
			
			break;
	}
	return 0;
}

// functia de callback a butonului pentru determinarea si afisarea mediei
int CVICALLBACK OnMeanButton (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, &intervalStart);
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, &intervalStop);
			
			int interval = intervalStop - intervalStart;
			if (interval < 1 ){
				MessagePopup("Error", "Invalid interval size.");
				intervalStart = 0;
				intervalStop = 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, intervalStart);
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			}
			
			if (interval > 1){
				intervalStop = intervalStart + 1;
				interval = 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			}
			int number_of_elements = interval * sampleRate;
			
			int startIndex = (int)(intervalStart * sampleRate);  
			
			if (number_of_elements <= 0) {
				MessagePopup("Error", "Invalid interval size.");
				return 0;
			}
			
			Mean(waveData + startIndex, number_of_elements, &mean);
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MEAN, mean);
			
			break;
	}
	return 0;
}

// functia de callback a butonului pentru determinarea si afisarea medianei
int CVICALLBACK OnMedianButton (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	double median=0.0;  // mediana
	switch (event)
	{
		case EVENT_COMMIT:

			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, &intervalStart);
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, &intervalStop);
			
			int interval = intervalStop - intervalStart;
			if (interval < 1 ){
				MessagePopup("Error",  "Invalid interval size.");
				intervalStart = 0;
				intervalStop = 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, intervalStart);
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			}
			if (interval > 1){
				intervalStop = intervalStart + 1;
				interval = 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			}
			int number_of_elements = interval * sampleRate;
			int startIndex = (int)(intervalStart * sampleRate);  
			
			if (number_of_elements <= 0) {
				MessagePopup("Error", "Invalid interval size.");
				return 0;
			}
			
			Median(waveData + startIndex, number_of_elements, &median);
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_MEDIAN, median);
			
			break;
	}
	return 0;
}

// functia de callback a butonului pentru determinarea si afisarea histogramei
int CVICALLBACK OnHistogramButton (int panel, int control, int event,
								   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, &intervalStart);
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, &intervalStop);
			
			int interval = intervalStop - intervalStart;
			if (interval < 1 ){
				MessagePopup("Error",   "Invalid interval size.");
				intervalStart = 0;
				intervalStop = 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, intervalStart);
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			}
			if (interval > 1){
				intervalStop = intervalStart + 1;
				interval = 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			}
			int number_of_elements = interval * sampleRate;
			int startIndex = (int)(intervalStart * sampleRate);  
			
			if (number_of_elements <= 0) {
				MessagePopup("Error", "Invalid interval size.");
				return 0;
			}
			
			int* histogram = (int *)calloc(number_of_elements, sizeof(int));
			int numIntervals = 10;
			double axisArray[100];
			
			DeleteGraphPlot(mainPanel, MAIN_PANEL_HISTOGRAM_GRAPH, -1, VAL_IMMEDIATE_DRAW);
			
			Histogram(waveData + startIndex, number_of_elements, minValue, maxValue, histogram, axisArray, numIntervals);
			PlotXY(panel, MAIN_PANEL_HISTOGRAM_GRAPH, axisArray, histogram, numIntervals,VAL_DOUBLE, VAL_SIZE_T, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);

			break;
	}
	return 0;
}


int CVICALLBACK OnZeroCrossingButton (int panel, int control, int event,
									  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, &intervalStart);
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, &intervalStop);
			
			int interval = intervalStop - intervalStart;
			if (interval < 1 ){
				MessagePopup("Error", "Invalid interval size.");
				intervalStart = 0;
				intervalStop = 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, intervalStart);
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			}
			
			if (interval > 1){
				intervalStop = intervalStart + 1;
				interval = 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			}
			
			int number_of_elements = interval * sampleRate;
			int startIndex = (int)(intervalStart * sampleRate);  
			
			if (number_of_elements <= 0) {
				MessagePopup("Error", "Invalid interval size.");
				return 0;
			}
			
			int treceriPrinZero = 0;
			for (int i=1; i<number_of_elements; i++){
				// trecerea valorii semnalului din valoare pozitiva in valoare negativa sau invers
				if (waveData[startIndex + i-1] * waveData[startIndex + i] < 0)
					treceriPrinZero+=1;
			}
			SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_ZERO_CROSSING, treceriPrinZero);
			
			break;
	}
	return 0;
}

int CVICALLBACK OnAplicaButton (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{

	double alpha;
	int windowSize;
	
	double *filt = 0; // valorile filtrate 
	char *filter_type_name = (char *)malloc(20 * sizeof(char));
	
	char fileName[256];
	int bitmapID;
	
	switch (event)
	{
		case EVENT_COMMIT:
			//resetare grafic
			DeleteGraphPlot(panel, MAIN_PANEL_IDC_GRAPH_FILT_DATA, -1, VAL_DELAYED_DRAW);
			
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, &intervalStart);
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, &intervalStop);
			
			int interval = intervalStop - intervalStart;
			if (interval < 1 ){
				MessagePopup("Error", "Invalid interval size.");
				intervalStart = 0;
				intervalStop = 1;
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, intervalStart);
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, intervalStop);
			}
			int number_of_elements = interval * sampleRate;
			
			int startIndex = (int)(intervalStart * sampleRate);
			// int stopIndex = (int) (intervalStop * sampleRate) - 1;
			
			//alocare memorie pentru vectorul ce contine valorile filtrate
			filt = (double *) calloc(number_of_elements, sizeof(double));
			
			if (filt == NULL){
				MessagePopup("Error", "Alocarea memoriei pentru filt a esuat");
				return -1;
			}
			
			// determinare tip de filtru
			// 0 = mediere;  1 = filtru de ordin I
			GetCtrlVal(mainPanel, MAIN_PANEL_FILTER_TYPE, &filterType);
			if (filterType == 0){ // mediere
				
				//SetCtrlAttribute(mainPanel, MAIN_PANEL_IDC_WINDOW_SIZE,
				// determinarea dimensiunii ferestrei
				GetCtrlVal(mainPanel, MAIN_PANEL_IDC_WINDOW_SIZE, &windowSize);
				
				filter_type_name = "filtru_de_mediere";
				for (int i=0; i<number_of_elements; i++){
					
					int start;
					if ( i - windowSize / 2 < 0)
						start = 0;
					else
						start = i - windowSize / 2;
					
					int end;
					if ( i + windowSize / 2 >= number_of_elements)
						end = number_of_elements - 1;
					else
						end = i + windowSize / 2;
					
					double sum = 0.0;
					int k = 0;
					
					for (int j=start; j<=end; j++){
						sum += waveData[startIndex + j];
						k++;
					}
					
					filt[i] = sum / k;	
				}	
			}
				
			 else if (filterType == 1) {// de ordin 1
				
				// determinare valoare alpha
				GetCtrlVal(mainPanel, MAIN_PANEL_ALPHA_NUMERIC, &alpha);
				
				filter_type_name = "filtru_cu_elem_de_ord_I";
				
				// aplicare filtru
				// filt[i]=(1-alpha)*filt[i-1]+alpha*signal[i]
				
				filt[0] = waveData[startIndex];
				for (int i=1; i<number_of_elements; i++){
					filt[i] = (1 - alpha) * filt[i - 1] + alpha * waveData[startIndex + i]; 
				}
			}

			else if (filterType == 2){ // derivata
				filter_type_name = "derivata";
				for (int i=0; i<number_of_elements - 1; i++)
					filt[i] = (waveData[startIndex + i+1] - waveData[startIndex + i]) / (1.0 / sampleRate);
			}
			
			PlotY(panel, MAIN_PANEL_IDC_GRAPH_FILT_DATA, filt, number_of_elements, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
			
			// salvare grafic raw data
			sprintf(fileName, "graph\\grafic_original_in_intervalul_%d_%d.jpg", intervalStart, intervalStop);
			GetCtrlDisplayBitmap(mainPanel, MAIN_PANEL_IDC_GRAPH_RAW_DATA, 1, &bitmapID);
			SaveBitmapToJPEGFile(bitmapID, fileName, JPEG_PROGRESSIVE, 100);
			DiscardBitmap(bitmapID);
			
			// salvare grafic filtered data
			sprintf(fileName, "graph\\grafic___%s_in_intervalul_%d_%d.jpg", filter_type_name,  intervalStart, intervalStop);
			GetCtrlDisplayBitmap(mainPanel, MAIN_PANEL_IDC_GRAPH_FILT_DATA, 1, &bitmapID);
			SaveBitmapToJPEGFile(bitmapID, fileName, JPEG_PROGRESSIVE, 100);
			DiscardBitmap(bitmapID);
			
			free(filt);
			break;
	}
	return 0;
}



int CVICALLBACK OnSkewnessButton (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, &intervalStart);
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, &intervalStop);
			
			if (intervalStart == 0){
				Moment (waveData, 256, 3, &skewness);
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_SKEWNESS, skewness);
			}
			else{
				MessagePopup("Error", "Intervalul in care doreste sa se determine skewness trebuie sa fie primul deoarece acesta contine primele 256 de esantioane");
				return 0;
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnKurtosisButton (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_START, &intervalStart);
			GetCtrlVal(mainPanel, MAIN_PANEL_IDC_STOP, &intervalStop);
			
			if (intervalStart == 0){
				Moment (waveData, 256, 4, &kurtosis);
				SetCtrlVal(mainPanel, MAIN_PANEL_IDC_NUM_KURTOSIS, kurtosis);
			}
			else{
				MessagePopup("Error", "Intervalul in care doreste sa se determine kurtosis trebuie sa fie primul deoarece acesta contine primele 256 de esantioane");
				return 0;
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnAnvelopaButton (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		
			double* waveDataCopy = (double *) calloc(lastNPoints, sizeof(double));
			memcpy(waveDataCopy, waveData, lastNPoints * sizeof(double));
			double* amplitudine = (double*)malloc(npoints * sizeof(double));
			
			FastHilbertTransform(waveDataCopy, lastNPoints);
			
			for (int i = 0; i < lastNPoints; i++) {
			        amplitudine[i] = fabs(waveDataCopy[i]);
			    }
			
			PlotY(panel, MAIN_PANEL_IDC_GRAPH_ENVELOPE, waveData, npoints, 
				  VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 
				  VAL_CONNECTED_POINTS, VAL_RED);
			
			PlotY(panel, MAIN_PANEL_IDC_GRAPH_ENVELOPE, amplitudine, lastNPoints, 
				  VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 
				  VAL_CONNECTED_POINTS,  VAL_GREEN);
			
			
			free(waveDataCopy);
			free(amplitudine);
			break;
	}
	return 0;
}
// ______________________________________________________________________________________


int ComputePowerSpectrum(double inputArray[], unsigned int numberOfElements, double samplingRate, 
                         double convertedSpectrum[], double *frequencyInterval, double *powerPeak, double *freqPeak)
{
    WindowConst windConst;
    ScaledWindowEx(inputArray, numberOfElements, RECTANGLE, 0, &windConst);

    // Cream spectrul de putere
    double* autoSpectrum = malloc(numberOfElements / 2 * sizeof(double));
    AutoPowerSpectrum(inputArray, numberOfElements, 1.0 / samplingRate, autoSpectrum, frequencyInterval);
    
    // Estimam frecventa si puterea la varf
    PowerFrequencyEstimate(autoSpectrum, numberOfElements / 2, -1.0, windConst, *frequencyInterval, 7, freqPeak, powerPeak);
    
    // Convertim spectrul in VRMS
    char unitString[32] = "V";
    SpectrumUnitConversion(autoSpectrum, numberOfElements / 2, SPECTRUM_POWER, SCALING_MODE_LINEAR, 
                           DISPLAY_UNIT_VRMS, *frequencyInterval, windConst, convertedSpectrum, unitString);
    
    free(autoSpectrum);
    return 0;
}


int CVICALLBACK OnSpectruButton(int panel, int control, int event,
                                 void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
        case EVENT_COMMIT:

            GetCtrlVal(freqPanel, FREQ_PANEL_IDC_NSAMPLES, &samplesPerChannel);

            convertedSpectrum = (double *)malloc((samplesPerChannel / 2) * sizeof(double));
            if (convertedSpectrum == NULL) {
                MessagePopup("Error", "Alocare memorie esuata pentru convertedSpectrum.");
                return -1;
            }

            ComputePowerSpectrum(waveData, samplesPerChannel, sampleRate, convertedSpectrum, &df, &powerPeak, &freqPeak);

            SetCtrlVal(freqPanel, FREQ_PANEL_POWER_PEAK, powerPeak);
            SetCtrlVal(freqPanel, FREQ_PANEL_FREQ_PEAK, freqPeak);

			DeleteGraphPlot(freqPanel, FREQ_PANEL_INTERV_SPECTRUM_GRAPH, -1, VAL_IMMEDIATE_DRAW);
            PlotWaveform(freqPanel, FREQ_PANEL_INTERV_SPECTRUM_GRAPH, convertedSpectrum, samplesPerChannel / 2, VAL_DOUBLE, 1.0, 0.0, 0.0, df, 
                         VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);

			if (filtWindSignal){
				convertedSpectrum1 = (double *)malloc((samplesPerChannel / 2) * sizeof(double));
				 if (convertedSpectrum1 == NULL) {
	                MessagePopup("Error", "Alocare memorie esuata pentru convertedSpectrum1.");
	                return -1;
	            }
				ComputePowerSpectrum(filtWindSignal, samplesPerChannel, sampleRate, convertedSpectrum1, &df1, &powerPeak1, &freqPeak1);
				SetCtrlVal(freqPanel, FREQ_PANEL_POWER_PEAK1, powerPeak1);
	            SetCtrlVal(freqPanel, FREQ_PANEL_FREQ_PEAK1, freqPeak1);
				DeleteGraphPlot(freqPanel, FREQ_PANEL_SPECTRUM_FILT_GRAPH, -1, VAL_IMMEDIATE_DRAW);
	            PlotWaveform(freqPanel, FREQ_PANEL_SPECTRUM_FILT_GRAPH, convertedSpectrum, samplesPerChannel / 2, VAL_DOUBLE, 1.0, 0.0, 0.0, df, 
	                         VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);
				}
			free(convertedSpectrum);
			free(convertedSpectrum1);
            break;
    }
    return 0;
}

int CVICALLBACK OnInterval(int panel, int control, int event,
                            void *callbackData, int eventData1, int eventData2)
{
	char fileName[256];
	int bitmapID;
    switch (event)
    {
        case EVENT_COMMIT:
            GetCtrlVal(freqPanel, FREQ_PANEL_START, &startInt);
            GetCtrlVal(freqPanel, FREQ_PANEL_STOP, &stopInt);
			
            if (startInt == 0) {
                SetCtrlAttribute(freqPanel, FREQ_PANEL_PREV_BUTTON, ATTR_DIMMED, 1);
            } else {
                SetCtrlAttribute(freqPanel, FREQ_PANEL_PREV_BUTTON, ATTR_DIMMED, 0);
            }

            if (stopInt == 6) {
                SetCtrlAttribute(freqPanel, FREQ_PANEL_NEXT_BUTTON, ATTR_DIMMED, 1);
            } else {
                SetCtrlAttribute(freqPanel, FREQ_PANEL_NEXT_BUTTON, ATTR_DIMMED, 0);
            }

            if (control == FREQ_PANEL_NEXT_BUTTON && stopInt < 6) {
                startInt += 1;
                stopInt += 1;
                SetCtrlVal(freqPanel, FREQ_PANEL_START, startInt);
                SetCtrlVal(freqPanel, FREQ_PANEL_STOP, stopInt);
            }

            if (control == FREQ_PANEL_PREV_BUTTON && startInt > 0) {
                startInt -= 1;
                stopInt -= 1;
                SetCtrlVal(freqPanel, FREQ_PANEL_START, startInt);
                SetCtrlVal(freqPanel, FREQ_PANEL_STOP, stopInt);
            }
			
			DeleteGraphPlot(freqPanel, FREQ_PANEL_INTERV_GRAPH, -1, VAL_IMMEDIATE_DRAW);
			PlotY(freqPanel, FREQ_PANEL_INTERV_GRAPH, waveData + (int)(sampleRate * startInt), sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);

			GetCtrlVal(freqPanel, FREQ_PANEL_IDC_NSAMPLES, &samplesPerChannel);

            convertedSpectrum = (double *)malloc((samplesPerChannel / 2) * sizeof(double));
            if (convertedSpectrum == NULL) {
                MessagePopup("Error", "Alocare memorie esuata pentru convertedSpectrum.");
                return -1;
            }

            ComputePowerSpectrum(waveData+ (int)(sampleRate*startInt), samplesPerChannel, sampleRate, convertedSpectrum, &df, &powerPeak, &freqPeak);

            SetCtrlVal(freqPanel, FREQ_PANEL_POWER_PEAK, powerPeak);
            SetCtrlVal(freqPanel, FREQ_PANEL_FREQ_PEAK, freqPeak);

			DeleteGraphPlot(freqPanel, FREQ_PANEL_INTERV_SPECTRUM_GRAPH, -1, VAL_IMMEDIATE_DRAW);
            PlotWaveform(freqPanel, FREQ_PANEL_INTERV_SPECTRUM_GRAPH, convertedSpectrum, samplesPerChannel / 2, VAL_DOUBLE, 1.0, 0.0, 0.0, df, 
                         VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);
            

			// salvare grafic spectru
			sprintf(fileName, "spectru\\spectru_in_intervalul_%d_%d.jpg", startInt, stopInt);
			GetCtrlDisplayBitmap(freqPanel, FREQ_PANEL_INTERV_SPECTRUM_GRAPH, 1, &bitmapID);
			SaveBitmapToJPEGFile(bitmapID, fileName, JPEG_PROGRESSIVE, 100);
			DiscardBitmap(bitmapID);
			
			if (filtSignal){
				DeleteGraphPlot(freqPanel, FREQ_PANEL_FILT_GRAPH, -1, VAL_IMMEDIATE_DRAW);
	            PlotY(freqPanel, FREQ_PANEL_FILT_GRAPH, filtSignal + (int)(startInt * sampleRate),
                  	sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
				
				sprintf(fileName, "semnal_filtrat\\semnal_filtrat_in_intervalul_%d_%d.jpg", startInt, stopInt);
				GetCtrlDisplayBitmap(freqPanel, FREQ_PANEL_FILT_GRAPH, 1, &bitmapID);
				SaveBitmapToJPEGFile(bitmapID, fileName, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(bitmapID);
			
			}
			
			if (filtWindSignal){
				DeleteGraphPlot(freqPanel, FREQ_PANEL_FILT_WIND_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				PlotY(freqPanel, FREQ_PANEL_FILT_WIND_GRAPH, filtWindSignal + (int)(startInt * sampleRate), sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				
				sprintf(fileName, "semnal_filtrat_ferestruit\\semnal_filtrat_ferestruit_in_intervalul_%d_%d.jpg", startInt, stopInt);
				GetCtrlDisplayBitmap(freqPanel, FREQ_PANEL_FILT_GRAPH, 1, &bitmapID);
				SaveBitmapToJPEGFile(bitmapID, fileName, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(bitmapID);
			}
			
			if (filtWindSignal){
				DeleteGraphPlot(freqPanel, FREQ_PANEL_SPECTRUM_FILT_GRAPH, -1, VAL_IMMEDIATE_DRAW);
	            PlotWaveform(freqPanel, FREQ_PANEL_SPECTRUM_FILT_GRAPH, convertedSpectrum, samplesPerChannel / 2, VAL_DOUBLE, 1.0, 0.0, 0.0, df, 
	                         VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);
				
			}
            
            break;
    }
    return 0;
}


int CVICALLBACK OnFilterButton(int panel, int control, int event,
                               void *callbackData, int eventData1, int eventData2)
{
    int filterType, cutFreq, order, windowType;
    
    switch (event)
    {
        case EVENT_COMMIT:

            GetCtrlVal(freqPanel, FREQ_PANEL_IDC_NSAMPLES, &samplesPerChannel);
            GetCtrlVal(freqPanel, FREQ_PANEL_FILTER_TYPE, &filterType);
            GetCtrlVal(freqPanel, FREQ_PANEL_CUT_FREQ, &cutFreq);
            
            // Alocare memorie pentru semnalul filtrat
            filtSignal = (double *)calloc(npoints, sizeof(double));
            if (filtSignal == NULL)
            {
                MessagePopup("Error", "Alocare memorie esuata pentru filtSignal.");
                return -1;
            }
            
            // Aplicare filtre
            switch (filterType)
            {
                case 0:  // Filtru de mediere pe 32 de valori
                {
                    double sum = 0.0;
                    
                    // Suma initiala pentru primele 32 de valori
                    for (int i = 0; i < 32; i++)
                        sum += waveData[i];
                    
                    // Filtrarea propriu-zisa
                    for (int i = 32; i < npoints; i++)
                    {
                        filtSignal[i] = sum / 32.0;
                        sum -= waveData[i - 32];
                        sum += waveData[i];
                    }
                    break;
                }
                
                case 1:  // Filtru eliptic de grad 4 sau 5
                    GetCtrlVal(freqPanel, FREQ_PANEL_ORDER, &order);
                    
                    if (order < 4 || order > 5)
                    {
                        MessagePopup("Error", "Gradul filtrului trebuie s? fie 4 sau 5.");
                        free(filtSignal);
                        return -1;
                    }
                    
                    Elp_HPF(waveData, npoints, sampleRate, cutFreq, 0.1, 40.0, order, filtSignal);
                    break;
            }
            
            // Afisare semnal filtrat pe grafic
            DeleteGraphPlot(freqPanel, FREQ_PANEL_FILT_GRAPH, -1, VAL_IMMEDIATE_DRAW);
            GetCtrlVal(freqPanel, FREQ_PANEL_START, &startInt);
            GetCtrlVal(freqPanel, FREQ_PANEL_STOP, &stopInt);
            
            PlotY(freqPanel, FREQ_PANEL_FILT_GRAPH, filtSignal + (int)(startInt * sampleRate),
                  sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);

			//ferestruirea
			GetCtrlVal(freqPanel, FREQ_PANEL_WINDOW_TYPE, &windowType);
			double* window1 = (double*)calloc(samplesPerChannel, sizeof(double));
			for (int i=0; i<samplesPerChannel; i++)
				window1[i] = 1.0;
			
			filtWindSignal = (double*)calloc(npoints, sizeof(double));
			for (int i=0; i<npoints; i++)
				filtWindSignal[i] = filtSignal[i];
			
			switch (windowType)
			{
				case 0: // hanning
					HanWin(window1, samplesPerChannel);
					HanWin(filtWindSignal, npoints);
					break;
					
				case 1: // hamming
					HamWin(window1, samplesPerChannel);
					HamWin(filtWindSignal, npoints);
					break;
				default:
					break;
			}
			
			DeleteGraphPlot(freqPanel, FREQ_PANEL_WIND_GRAPH, -1, VAL_IMMEDIATE_DRAW);
			PlotY(freqPanel, FREQ_PANEL_WIND_GRAPH, window1, samplesPerChannel, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
			// afisare semnal filtrat si ferestruit
			DeleteGraphPlot(freqPanel, FREQ_PANEL_FILT_WIND_GRAPH, -1, VAL_IMMEDIATE_DRAW);
			PlotY(freqPanel, FREQ_PANEL_FILT_WIND_GRAPH, filtWindSignal + (int)(startInt * sampleRate), sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
            break;
    }
    return 0;
}

