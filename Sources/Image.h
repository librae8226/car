#ifndef __IMAGE_H__
#define __IMAGE_H__

#define ROW_VALUE 40
#define COLUMN_VALUE 100
#define THRESHOLDUPLIMIT 0x80
#define THRESHOLDLIMIT  0x40
#define THRESHOLDCHECK 10

extern int Line_Center[ROW_VALUE]; // Black center
extern int Line_Center_bak[ROW_VALUE]; // Black center backup
extern unsigned char buff[ROW_VALUE][COLUMN_VALUE]; // for image sample
extern unsigned char data_b[COLUMN_VALUE];
extern unsigned char SAMPLE_INTERVAL;
                              
extern unsigned char *app; // pointer for getting image
extern unsigned char CaptureCommand;
extern unsigned char *p_buff; // pointer for processing image
extern float threshold, thresholdtemp;
//extern int RowToProcess;
extern int blacksum;
extern int blackposcenter;
extern int blackposavg;
extern int blackpossum;           
extern int blackfiltered;              
//extern int lastblack;
extern unsigned char get_lost_time;
extern unsigned char image_lost;//for imergent breaking
extern unsigned int stat[16];
extern unsigned char search_l, search_r;
extern unsigned char search_range;
extern int row_counter; //row counter
extern int row; //actual row number while sampling
extern int prediction;
extern int valid_prediction_value;
extern int pre_valid;   //previous valid_prediction_value
extern int pre_d_valid;
extern int slope_var;
extern int leftpeak, rightpeak;
extern int leftpeakpos, rightpeakpos;
extern int max_slope_var_pos;
extern int farcurve;
extern int farslope1, farslope2;
extern int nearcurve;
extern int nearslope1, nearslope2;
extern int slope_product_near;
extern int slope_product_far;
extern unsigned char nearest_linecenter, previous_nearest_linecenter;
extern unsigned char permit_nearest_linecenter_err;
extern int line_dir, pre_line_dir;   //Line_Center[35] - Line_Center[39], avoid being disturbed by cross line
extern unsigned char permit_line_dir_err;
extern unsigned char image_core, pre_image_core;   //center of the whole image
extern unsigned char permit_image_core_err;
extern int nearest_valid_line;
extern int continuous_valid_line;
extern unsigned char find_start;
extern int routetype;
extern int scheme;
extern int counter_straight, counter_ss, counter_bs, counter_left, counter_right, counter_slope;
extern int edge_ll[ROW_VALUE];  //for start line checking
extern int edge_rr[ROW_VALUE];  //for start line checking
extern int start_counter;
extern int image_counter;

volatile extern Bool flag_image_lost;   //bool, being set while nearest_linecenter differ far from previous one.
                                        //if set, hold previous control parameters
volatile extern Bool flag_line_dir_err;
volatile extern Bool g_SampleFlag; // VS flag
volatile extern Bool flag;
volatile extern Bool findroute;
volatile extern Bool flag_start;    //set while find start line

unsigned char GetThreshold(unsigned char x);

int GetBlackPos(unsigned char RowToProcess); 

void Check_start(void);

void ImageProcess(void); //Final filter. Get the Line_Center[ROW_VALUE]

float GetWeight(int i, int scheme);

#endif